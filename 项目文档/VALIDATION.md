# Validation

## Current maintenance — 2026-10-04

- This revision, based on the public HEAD, rejects a selection line containing an embedded NUL before EOF. The new process case returned success and created output on the preceding public implementation; it now returns failure without an output file.
- One shared budget limits the total number of declared Mach-O load commands traversed across the root and embedded headers. Seven constrained-budget process cases distinguish failure during the second root scan, each of two embedded scans, and selection re-scan from successful complete extraction. These seven cases are not sanitizer builds; they check the accounting logic without allocating a large kernel image. The default one-million-command threshold has no real-kernel compatibility corpus yet; legitimate larger images may be rejected.
- The full current safety suite passes 139 ASan/UBSan process cases plus seven constrained-budget process cases. Both valid and rejected input files remain byte-for-byte unchanged; outputs are created only for valid selections. This verifies owned synthetic fixtures, not a live kernel or every layout.

## Defensive rewrite baseline — 2026-10-02

- Current sources compile with Clang using `-Wall -Wextra -pedantic -std=c99`.
- `python3 checks/kernelcabinet_safety.py` passes 138 process cases under AddressSanitizer and UndefinedBehaviorSanitizer. The fixture matrix includes supported raw extraction, bounds/command/record failures, read-only input preservation, special files, exclusive output creation, symlinks, metadata control characters and repeated structural mutations.
- Valid output is checked against the exact input segment bytes and mode 0600. Existing output and symlink targets remain unchanged. No fixture instructions are executed.
- Current CLI returns nonzero for malformed/unsupported input and failures. It no longer preserves the predecessor's zero error statuses, metadata-derived names or in-buffer normalization.

## Historical predecessor

Before this rewrite, 126 process comparisons on 63 owned fixtures passed at O0/O2 and 4 executable function bodies matched the pinned upstream after identifier normalization. Those results describe the previous modular derivative. RENAME_MAP.json and the historical check scripts preserve that provenance; they do not prove current code is equivalent or universally safe.

## OPEN

Only the documented compatible table layout is supported. Tests do not establish behavior for every kernel version, all malformed bytes or a real device. No loading/execution/signing result or CVP eligibility is claimed. Earlier v1.0.0 source/binary assets are historical. See DEFENSIVE_SCOPE.md.
