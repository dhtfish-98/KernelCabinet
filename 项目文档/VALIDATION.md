# Validation

## Current defensive rewrite — 2026-10-02

- Current sources compile with Clang using `-Wall -Wextra -pedantic -std=c99`.
- `python3 checks/kernelcabinet_safety.py` passes 138 process cases under AddressSanitizer and UndefinedBehaviorSanitizer. The fixture matrix includes supported raw extraction, bounds/command/record failures, read-only input preservation, special files, exclusive output creation, symlinks, metadata control characters and repeated structural mutations.
- Valid output is checked against the exact input segment bytes and mode 0600. Existing output and symlink targets remain unchanged. No fixture instructions are executed.
- Current CLI returns nonzero for malformed/unsupported input and failures. It no longer preserves the predecessor's zero error statuses, metadata-derived names or in-buffer normalization.

## Historical predecessor

Before this rewrite, 126 process comparisons on 63 owned fixtures passed at O0/O2 and 4 executable function bodies matched the pinned upstream after identifier normalization. Those results describe the previous modular derivative. RENAME_MAP.json and the historical check scripts preserve that provenance; they do not prove current code is equivalent or universally safe.

## OPEN

Only the documented compatible table layout is supported. Tests do not establish behavior for every kernel version, all malformed bytes or a real device. No loading/execution/signing result or CVP eligibility is claimed. Earlier v1.0.0 source/binary assets are historical. See DEFENSIVE_SCOPE.md.
