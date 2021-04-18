# gen-rack

Export gen~ patches to VCV Rack modules.



# Requirements

- Max 8
- VCV Development Environment (currently v1.1.16)
  - See this [link](https://manual.vcvrack.com/Building#setting-up-your-development-environment) or below for details

## Windows

The VCV build toolchain leverages `MSYS2` which aims to provide a native build environment for Windows based on open source software. For more, read [here](https://www.msys2.org/docs/what-is-msys2/).

With the MSYS2 MinGW 64-bit shell, update the package manager with `pacman -Syu`. Restart the shell and install packages:
```
pacman -Su git wget make tar unzip zip mingw-w64-x86_64-gcc mingw-w64-x86_64-gdb mingw-w64-x86_64-cmake autoconf automake mingw-w64-x86_64-libtool mingw-w64-x86_64-jq python
```

## Mac

Make sure you have [Homebrew](https://brew.sh/) installed before getting the packages:
```
brew install git wget cmake autoconf automake libtool jq python
```

# License

Licensed under the terms of GPL-3.