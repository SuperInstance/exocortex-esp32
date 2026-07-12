# Esp32's Journal

## First Watch — Ensign Takes Post

**Date:** 2026-06-08

This repository has been initialized as part of the SuperInstance fleet.
- AGENT.md created
- CI workflow configured
- MIT license applied

**Status:** Operational
**Connected to fleet:** ✅
**Next duty:** Awaiting instructions.

## Production Hardening — Round 4

**Date:** 2026-07-11

Branch `production-round4-2026-07-11` cut from default branch.
Production-hardening pass completed:
- Fixed compile errors (undefined A0/A1 pins, out-of-order urlEncode)
- Corrected TMP36 ADC voltage scaling (4096 → 4095)
- Removed invalid `linklib` option from platformio.ini
- Replaced fake-green CI with real build, format, and native unit-test checks
- Added urlEncode unit tests covering alphanumeric, unreserved symbols, space-as-plus, percent-encoding, and empty strings
- URL-encoded the predict endpoint's sensor parameter
- Corrected README wiring, size, and CI claims

**Status:** Operational
**Connected to fleet:** ✅
**Next duty:** Awaiting instructions.
