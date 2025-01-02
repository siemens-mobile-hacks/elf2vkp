# ELF2VKP

This program:

1. Converts each .elf section into a V-Klay patch string.

2. Add old patch data if a fullflash path is specified.

The main reason for the existence of this program is that the sources of the old elf2vkp.exe have been lost.

# DOWNLOAD
- Windows: download .exe in [Releases](https://github.com/siemens-mobile-hacks/elf2vkp/releases).
- ArchLinux: `yay -S elf2vkp-git`
- [Ubuntu/Debian/Fedora/OpenSUSE repository](https://software.opensuse.org//download.html?project=home%3AZhumarin&package=elf2vkp)
- Old Ubuntu/Debian: download .deb in [Releases](https://github.com/siemens-mobile-hacks/elf2vkp/releases).
- Build from sources:
	```bash
	# Ubuntu/Debian
	fakeroot debian/rules binary

	# OSX/Linux/Unix
	mkdir build
	cd build
	cmake ..
	make -j$(nproc)
	
	# Windows (MinGW)
	mkdir build
	cd build
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/Toolchain-x86_64-w64-mingw32.cmake
	make -j$(nproc)
	```

# USAGE
```
Usage: elf2vkp [--help] [--version] --input VAR --output VAR [--fullflash VAR] [--base VAR] [--header VAR] [--header-from-file VAR] [--section-names] [--old-print-format]

Optional arguments:
  -h, --help          shows help message and exits 
  -v, --version       prints version information and exits 
  -i, --input         Path to patch.elf [required]
  -o, --output        Path to patch.vkp [default: "-"]
  -f, --fullflash     Path to fullflash.bin [default: ""]
  -b, --base          Firmware base address [default: "A0000000"]
  --header            Add patch header [default: ""]
  --header-from-file  Add patch header (from file) [default: ""]
  --section-names     Show section names in .vkp 
  --old-print-format  Output .vkp with old elf2vkp.exe formatting 
```

### Convert patch.elf to patch.vkp with old data
```
$ elf2vkp -i patch.elf -o patch.vkp -f EL71sw45.bin
```

### Convert patch.elf to patch.vkp without old data
```
$ elf2vkp -i patch.elf -o patch.vkp
```
