# ELF2VKP

This program:

1. Converts each .elf section into a V-Klay patch string.

2. Add old patch data if a fullflash path is specified.

The main reason for the existence of this program is that the sources of the old elf2vkp.exe have been lost.

# DOWNLOAD
- Windows: download .exe in [Releases](https://github.com/siemens-mobile-hacks/elf2vkp/releases).
- ArchLinux: `yay -S elf2vkp-git`
- Ubuntu/Debian: download .deb in [Releases](https://github.com/siemens-mobile-hacks/elf2vkp/releases).
- Build from sources:
	```bash
	# Ubuntu/Debian
	fakeroot debian/rules binary

	# OSX, Linux, Unix, MinGW, Windows MSVC
	cmake -B build -DCMAKE_BUILD_TYPE=Release
	cmake --build build
	```

# USAGE

```
Usage: elf2vkp [--help] [--version] --input VAR --output VAR [--fullflash VAR] [--base VAR] [--header VAR]... [--header-from-file VAR] [--section-names] [--chunk-size VAR] [--format VAR] [--no-substract-base-addr] [--no-pragma] [--use-crlf]

Optional arguments:
  -h, --help                shows help message and exits
  -v, --version             prints version information and exits
  -i, --input               Path to patch.elf [required]
  -o, --output              Path to patch.vkp [default: "-"]
  -f, --fullflash           Path to fullflash.bin [default: ""]
  -b, --base                Firmware base address [default: "A0000000"]
  -H, --header              Add patch header [default: {}] [may be repeated]
  --header-from-file        Add patch header (from file) [default: ""]
  --section-names           Show section names in .vkp
  --chunk-size              Maximum bytes per one line [default: 16]
  -F, --format              Patch output format: v-klay, armdebugger, sony-ericsson [default: "v-klay"]
  --no-substract-base-addr  Disable substracting base address from patch addr
  --no-pragma               Disable use of #pragma in the patch
  --use-crlf                Use windows \r\n instead of Unix \n
```

### Convert patch.elf to patch.vkp with old data
```
$ elf2vkp -i patch.elf -o patch.vkp -f EL71sw45.bin
```

### Convert patch.elf to patch.vkp without old data
```
$ elf2vkp -i patch.elf -o patch.vkp
```

### Convert patch.elf to patch.vkp (Sony Ericcson)
```
$ elf2vkp -F sony-ericsson -i patch.elf -o patch.vkp -f firmware.bin
```
