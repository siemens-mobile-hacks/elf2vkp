#include "main.h"
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdarg>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cassert>

static const uint8_t ELF_MAGIC_HEADER[] = {
	0x7f, 0x45, 0x4c, 0x46,  /* 0x7f, 'E', 'L', 'F' */
	0x01,                    /* Only 32-bit objects. */
	0x01,                    /* Only LSB data. */
	0x01,                    /* Only ELF version 1. */
};

int main(int argc, char *argv[]) {
	argparse::ArgumentParser program("elf2vkp", "1.0.0");

	program.add_argument("-i", "--input")
		.help("Path to patch.elf")
		.required()
		.nargs(1);
	program.add_argument("-o", "--output")
		.help("Path to patch.vkp")
		.default_value("-")
		.required()
		.nargs(1);
	program.add_argument("-f", "--fullflash")
		.help("Path to fullflash.bin")
		.default_value("")
		.nargs(1);
	program.add_argument("-b", "--base")
		.help("Firmware base address")
		.default_value("A0000000")
		.nargs(1);
	program.add_argument("--header")
		.help("Add patch header")
		.default_value("")
		.nargs(1);
	program.add_argument("--header-from-file")
		.help("Add patch header (from file)")
		.default_value("")
		.nargs(1);
	program.add_argument("--section-names")
		.help("Show section names in .vkp")
		.default_value(false)
		.implicit_value(true)
		.nargs(0);
	program.add_argument("--old-print-format")
		.help("Output .vkp with old elf2vkp.exe formatting")
		.default_value(false)
		.implicit_value(true)
		.nargs(0);
	program.add_argument("--chunk-size")
		.help("Maximum bytes per one line")
		.default_value(16)
		.nargs(1)
		.scan<'i', int>();

	try {
		program.parse_args(argc, argv);

		std::string inputFile = program.get<std::string>("--input");
		std::string outputFile = program.get<std::string>("--output");
		std::string fullflashFile = program.get<std::string>("--fullflash");
		std::string header = program.get<std::string>("--header");
		std::string headerFromFile = program.get<std::string>("--header-from-file");

		Config config = {
			.base = (uint32_t) stoll(program.get<std::string>("--base"), NULL, 16),
			.oldPrintFormat = program.get<bool>("--old-print-format"),
			.showSectionNames = program.get<bool>("--section-names"),
			.chunkSize = program.get<int>("--chunk-size"),
		};

		auto chunks = getPatchDataFromELF(config, inputFile, fullflashFile);
		auto patchSourceCode = generatePatch(config, chunks);

		if (header != "") {
			patchSourceCode = header + (config.oldPrintFormat ? "\r\n" : "\n") + patchSourceCode;
		}

		if (headerFromFile != "") {
			patchSourceCode = readFile(headerFromFile) + patchSourceCode;
		}

		if (outputFile == "-") {
			std::cout << patchSourceCode;
		} else {
			std::ofstream out(outputFile);
			out << patchSourceCode;
			out.close();
		}
	} catch (const std::exception &err) {
		std::cerr << "ERROR: " << err.what() << "\n";
		return 1;
	}

	return 0;
}

std::string generatePatch(const Config &config, const std::vector<PatchData> &chunks) {
	std::string patchFile;
	bool oldDataEqualFF = false;
	std::string eol = config.oldPrintFormat ? "\r\n" : "\n";
	for (auto &c: chunks) {
		if (isOldDataEqualFF(c)) {
			if (!config.oldPrintFormat && &c != &chunks[0])
				patchFile += eol;
			if (!oldDataEqualFF) {
				oldDataEqualFF = true;
				patchFile += "#pragma enable old_equal_ff" + eol;
			}
		} else {
			if (oldDataEqualFF) {
				oldDataEqualFF = false;
				patchFile += "#pragma disable old_equal_ff" + eol;
			}
			if (!config.oldPrintFormat && &c != &chunks[0])
				patchFile += eol;
		}

		if (config.showSectionNames) {
			patchFile += config.oldPrintFormat ?
				";" + c.name + eol :
				"; " + c.name + eol;
		}

		for (uint32_t i = 0; i < c.size; i += config.chunkSize) {
			patchFile += config.oldPrintFormat ?
				strprintf("0x%08X: ", c.addr + i - config.base) :
				strprintf("%07X: ", c.addr + i - config.base);
			if (c.oldData.size() > 0 && !oldDataEqualFF) {
				for (uint32_t j = i; j < std::min(i + config.chunkSize, c.size); j++) {
					patchFile += strprintf("%02X", c.oldData[j]);
				}
				patchFile += " ";
			}
			for (uint32_t j = i; j < std::min(i + config.chunkSize, c.size); j++) {
				patchFile += strprintf("%02X", c.newData[j]);
			}
			patchFile += eol;
		}
	}

	if (oldDataEqualFF) {
		oldDataEqualFF = false;
		patchFile += "#pragma disable old_equal_ff" + eol;
	}

	return patchFile;
}

bool isOldDataEqualFF(const PatchData &pd) {
	if (pd.oldData.size() == 0)
		return false;
	for (auto byte: pd.oldData) {
		if (byte != 0xFF)
			return false;
	}
	return true;
}

std::vector<PatchData> getPatchDataFromELF(const Config &config, const std::string &elfFile, const std::string &fullflashFile) {
	std::vector<uint8_t> elf = readBinaryFile(elfFile);
	FILE *fullflashFp = nullptr;

	if (fullflashFile != "") {
		fullflashFp = fopen(fullflashFile.c_str(), "r");
		if (!fullflashFp)
			throw std::runtime_error(strprintf("Can't open fullflash file: %s", fullflashFile.c_str()));
	}

	auto *ehdr = getSafePtr<Elf32_Ehdr>(elf, 0);
	assert(ehdr->e_ehsize == sizeof(Elf32_Ehdr));
	assert(ehdr->e_phentsize == sizeof(Elf32_Phdr));
	assert(ehdr->e_shentsize == sizeof(Elf32_Shdr));

	if (memcmp(ehdr->e_ident, ELF_MAGIC_HEADER, sizeof(ELF_MAGIC_HEADER)) != 0)
		throw std::runtime_error("Invalid ELF magic.");

	if (ehdr->e_machine != EM_ARM)
		throw std::runtime_error("Only ARM ELF's is supported.");

	auto *sectionNamesTableHeader = getSafePtr<Elf32_Shdr>(elf, ehdr->e_shoff + ehdr->e_shentsize * ehdr->e_shstrndx);
	const char *sectionNames = reinterpret_cast<const char *>(&elf.at(sectionNamesTableHeader->sh_offset));

	std::vector<PatchData> chunks;

	for (size_t i = 0; i < ehdr->e_phnum; i++) {
		auto *phdr = getSafePtr<Elf32_Phdr>(elf, ehdr->e_phoff + ehdr->e_phentsize * i);
		if (phdr->p_type == PT_DYNAMIC)
			throw std::runtime_error("ELF's with dynamic relocations is not supported!");
		if (phdr->p_type != PT_LOAD)
			throw std::runtime_error(strprintf("Invalid phdr type: %d", phdr->p_type));
		if (phdr->p_filesz == 0) // skip empty sections
			continue;
		assert(phdr->p_filesz == phdr->p_memsz);
		assert(phdr->p_vaddr == phdr->p_paddr);
	}

	for (size_t i = 0; i < ehdr->e_shnum; i++) {
		auto *shdr = getSafePtr<Elf32_Shdr>(elf, ehdr->e_shoff + ehdr->e_shentsize * i);

		if (shdr->sh_type != SHT_PROGBITS || shdr->sh_size == 0 || !(shdr->sh_flags & SHF_ALLOC))
			continue;

		chunks.push_back({
			.name = sectionNames + shdr->sh_name,
			.addr = shdr->sh_addr,
			.size = shdr->sh_size,
			.oldData = getOldData(fullflashFp, shdr->sh_addr - config.base, shdr->sh_size),
			.newData = std::vector<uint8_t>(elf.begin() + shdr->sh_offset, elf.begin() + shdr->sh_offset + shdr->sh_size)
		});
	}

	if (fullflashFp)
		fclose(fullflashFp);

	return chunks;
}

std::vector<uint8_t> getOldData(FILE *fp, uint32_t offset, uint32_t size) {
	std::vector<uint8_t> buffer;
	if (fp) {
		uint32_t readCount = 0;
		buffer.resize(size);
		if (fseek(fp, offset, SEEK_SET) != 0)
			throw std::runtime_error(strprintf("Fullflash unexpected EOF at 0x%08X", offset));
		while (!feof(fp) && readCount < size) {
			int ret = fread(&buffer[readCount], 1, size - readCount, fp);
			if (ret > 0) {
				readCount += ret;
			} else if (ret < 0) {
				throw std::runtime_error(strprintf("Fullflash read error (0x%08X): %s", offset + readCount, strerror(errno)));
			}
		}
		if (readCount < size)
			throw std::runtime_error(strprintf("Fullflash unexpected EOF at 0x%08X", offset + readCount));
	}
	return buffer;
}

std::vector<Elf32_Shdr *> getSectionsFromSegment(std::vector<uint8_t> &buffer, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr) {
	std::vector<Elf32_Shdr *> sections;
	for (size_t i = 0; i < ehdr->e_shnum; i++) {
		auto *shdr = getSafePtr<Elf32_Shdr>(buffer, ehdr->e_shoff + ehdr->e_shentsize * i);
		if (shdr->sh_offset >= phdr->p_offset && shdr->sh_offset < phdr->p_offset + phdr->p_filesz)
			sections.push_back(shdr);
	}
	return sections;
}

std::string readFile(const std::string &path) {
	FILE *fp = fopen(path.c_str(), "r");
	if (!fp) {
		throw std::runtime_error("fopen(" + path + ") error: " + strerror(errno));
	}

	char buff[4096];
	std::string result;
	while (!feof(fp)) {
		int readed = fread(buff, 1, sizeof(buff), fp);
		if (readed > 0)
			result.append(buff, readed);
	}
	fclose(fp);

	return result;
}

std::vector<uint8_t> readBinaryFile(const std::string &path) {
	FILE *fp = fopen(path.c_str(), "r");
	if (!fp) {
		throw std::runtime_error("fopen(" + path + ") error: " + strerror(errno));
	}

	size_t maxFileSize = std::filesystem::file_size(path);
	std::vector<uint8_t> bytes;
	bytes.resize(maxFileSize);

	size_t readed = 0;
	while (!feof(fp) && readed < maxFileSize) {
		int ret = fread(&bytes[readed], 1, std::min(static_cast<size_t>(4096), maxFileSize - readed), fp);
		if (ret > 0) {
			readed += ret;
		} else if (ret < 0) {
			throw std::runtime_error("fread(" + path + ") error: " + strerror(errno));
		}
	}
	fclose(fp);

	return bytes;
}

std::string strprintf(const char *format, ...) {
	va_list v;

	std::string out;

	va_start(v, format);
	int n = vsnprintf(nullptr, 0, format, v);
	va_end(v);

	if (n <= 0)
		throw std::runtime_error("vsnprintf error...");

	out.resize(n);

	va_start(v, format);
	vsnprintf(&out[0], out.size() + 1, format, v);
	va_end(v);

	return out;
}

std::string strJoin(const std::string &sep, const std::vector<std::string> &lines) {
	std::string out;
	size_t length = lines.size() > 1 ? sep.size() * (lines.size() - 1) : 0;
	for (auto &line: lines)
		length += line.size();

	out.reserve(length);

	bool first = true;
	for (auto &line: lines) {
		if (first) {
			first = false;
			out += line;
		} else {
			out += sep + line;
		}
	}

	return out;
}

std::vector<std::string> strSplit(const std::string &sep, const std::string &str) {
	std::vector<std::string> result;
	size_t last_pos = 0;
	while (true) {
		size_t pos = str.find(sep, last_pos);
		if (pos == std::string::npos) {
			result.push_back(str.substr(last_pos));
			break;
		} else {
			result.push_back(str.substr(last_pos, pos - last_pos));
			last_pos = pos + 1;
		}
	}
	return result;
}
