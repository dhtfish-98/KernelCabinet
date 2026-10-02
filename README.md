# KernelCabinet

防御用途、实际能力及验证范围见 [DEFENSIVE_SCOPE.md](DEFENSIVE_SCOPE.md)。

KernelCabinet locates embedded extension metadata in compatible, authorized ARM64 Mach-O kernel images and copies selected raw `__TEXT_EXEC` bytes for static analysis. Source lineage and GPL terms remain in [ORIGIN.md](ORIGIN.md) and LICENSE.

## Build and use

macOS, Clang and Apple Mach-O headers are required.

```sh
make
./kernelcabinet /path/to/owned-kernel-image
python3 checks/kernelcabinet_safety.py
```

Run in a private output directory. The input is read into a bounded private buffer and preserved. Choose an index from the validated table; output is `kernelcabinet-index-N.bin`, created exclusively with mode 0600. Existing files and symlinks are rejected. A write error returns failure and may leave an incomplete new file. The output is raw segment data, not a reconstructed loadable kext.

## Supported format and behavior changes

This implementation expects an ARM64 Mach-O with `__TEXT` file offset zero and compatible `__PRELINK_INFO` kmod tables. File size is limited to 1 GiB, record count to 4096. Commands, sections, table offsets, embedded headers and record ranges are checked before use. Unsupported layouts and malformed inputs fail with nonzero status.

The October 2026 defensive rewrite replaces the inherited unbounded mapping/traversal and input-derived output name. It intentionally changes output names, permissions, byte normalization and error status. Earlier AST/output equivalence reports and v1.0.0 packages describe the predecessor and are historical. Build current source for this policy.

## Verification

`checks/kernelcabinet_safety.py` compiles this source with ASan/UBSan and runs 138 owned process cases, including valid extraction, truncated files, corrupt commands/tables/addresses, unterminated names, special files, output collisions, symlinks and escaped metadata. The historical structure/equivalence scripts remain for provenance, and are not current acceptance gates. See [VALIDATION.md](VALIDATION.md).
