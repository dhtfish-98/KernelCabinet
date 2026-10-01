# KernelCabinet

KernelCabinet extracts embedded kernel-extension code from compatible ARM64 Mach-O kernel images. It is a renamed and modular derivative of the source recorded in [ORIGIN.md](ORIGIN.md).

## Build and use

macOS with Clang and the Apple Mach-O headers is required.

```sh
make
./kernelcabinet /path/to/owned-kernel-image
make install DESTDIR=/tmp/kernelcabinet-install PREFIX=/usr/local
```

The original positional argument, selection prompt (where applicable), output layout and binary extraction behavior remain. The new executable name changes the program path printed in usage text.

## Organization

`Sources/` separates container traversal, analysis/export and process entry. All self-defined C declarations, macro names and macro parameters have new implementation names. `main`, standard/system APIs, Mach-O layout field names and format/protocol strings retain their compatibility roles.

## Reproduce verification

Check out the pinned upstream commit listed in ORIGIN.md outside this tree, then run:

```sh
python3 checks/kernelcabinet_structure.py --reference /path/to/upstream/kextract.c
python3 checks/kernelcabinet_equivalence.py --reference /path/to/upstream/kextract.c
```

Tests use generated, owned Mach-O data. See [VALIDATION.md](VALIDATION.md) for the actual scope and inherited boundaries.
