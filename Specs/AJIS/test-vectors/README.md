# AJIS Test Vectors

This directory contains normative test vectors for AJIS v1.

Test vectors are used to:

- validate parser correctness,
- verify canonical serialization,
- ensure consistent behavior across implementations.

## Directory Structure

- `valid/`  
  Inputs that MUST be accepted by an AJIS parser.

- `invalid/`  
  Inputs that MUST be rejected by an AJIS parser.

- `canonical/`  
  Pairs of input and expected canonical output.

## Rules

- All test vectors are UTF-8 encoded.
- Parsers must reject any input listed in `invalid/`.
- Canonical outputs must match byte-for-byte.

Test vectors are part of the AJIS specification.
