# ELF2VKP

This program:

1. Converts each .elf section into a V-Klay patch string.

2. Add old patch data if a fullflash path is specified.

The main reason for the existence of this program is that the sources of the old elf2vkp.exe have been lost.

# INSTALL

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
