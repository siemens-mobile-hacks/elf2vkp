%global debug_package %{nil}
%define _unpackaged_files_terminate_build 0

Name: elf2vkp
Version: 1.0
Release: 0
Summary: Tool for converting .elf to .vkp patches.
License: MIT
Source0: %{name}-%version.tar.gz
URL: https://github.com/siemens-mobile-hacks/elf2vkp
BuildRequires: gcc
BuildRequires: gcc-c++
BuildRequires: git
BuildRequires: cmake

%description
Tool for converting .elf to .vkp patches.

%prep
%setup -q

%build
%cmake
%cmake_build

%install
%cmake_install

%check
%ctest

%files
/usr/bin/elf2vkp
%license LICENSE
%doc README.md

%changelog
