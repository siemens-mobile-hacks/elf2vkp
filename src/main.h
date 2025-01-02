#pragma once

#include <string>
#include <cstdint>
#include <qemu/elf.h>

#if defined(_WIN32) || defined(__MINGW32__)
	#define SIZET_FMT "%Iu"
#else
	#define SIZET_FMT "%zu"
#endif

struct Config {
	uint32_t base;
	bool oldPrintFormat;
	bool showSectionNames;
	int chunkSize;
	bool sonyEricsson;
};

struct PatchData {
	std::string name;
	uint32_t addr;
	uint32_t size;
	std::vector<uint8_t> oldData;
	std::vector<uint8_t> newData;
};

std::string readFile(const std::string &path);
std::vector<uint8_t> readBinaryFile(const std::string &path);
#if defined(_MSC_VER)
std::string strprintf(const char *format, ...);
#else
std::string strprintf(const char *format, ...)  __attribute__((format(printf, 1, 2)));
#endif
std::vector<Elf32_Shdr *> getSectionsFromSegment(std::vector<uint8_t> &buffer, Elf32_Ehdr *ehdr, Elf32_Phdr *phdr);

std::vector<std::string> strSplit(const std::string &sep, const std::string &str);
std::string strJoin(const std::string &sep, const std::vector<std::string> &lines);

std::vector<PatchData> getPatchDataFromELF(const Config &baseOffset, const std::string &elfFile, const std::string &fullflashFile);
std::vector<uint8_t> getOldData(FILE *fp, uint32_t offset, uint32_t size);
bool isOldDataEqualFF(const PatchData &pd);
std::string generatePatch(const Config &baseOffset, const std::vector<PatchData> &chunks);

template<typename T>
T *getSafePtr(std::vector<uint8_t> &buffer, size_t offset) {
	if (offset + sizeof(T) > buffer.size())
		throw std::runtime_error(strprintf("Unexpected ELF file EOF at " SIZET_FMT, offset + sizeof(T)));
	return reinterpret_cast<T *>(&buffer[offset]);
}
