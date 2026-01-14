# AJIS v1 Specification

## 1. Scope

This document defines **AJIS version 1**, a text-based serialization format.

AJIS defines:

- syntax,
- data model,
- parsing rules,
- and canonical serialization rules.

AJIS does **not** define:

- execution semantics,
- templating or macro systems,
- variable substitution,
- schema validation,
- locale-dependent behavior,
- dynamic parser configuration via input data.

AJIS is a **data format**, not a programming language.

---

## 2. Design Goals

AJIS is designed to:

- be a strict and deterministic data format,
- be easily parsed in C without external dependencies,
- support binary data natively,
- remain a strict superset of JSON syntax,
- avoid implicit type conversions,
- avoid whitespace-sensitive syntax,
- extend JSON by adding missing data primitives without introducing ambiguity.

---

## 3. Non-Goals

AJIS explicitly does **not** aim to:

- replace programming languages,
- embed expressions or logic,
- provide Unicode normalization rules,
- define case-insensitive maps,
- define naming policies (camelCase/snake_case/PascalCase),
- provide schema validation rules.

Naming policies belong to serializers or application-level mapping.

---

## 4. Document Model

Each AJIS document represents **exactly one value**.

---

## 5. Data Model Overview

AJIS values map directly to the AUV data model.

Supported value types:

- `null`
- `bool`
- `int64`
- `float64`
- `string` (UTF-8)
- `char` (single Unicode scalar)
- `binary`
- `array`
- `object`

---

## 6. Strings and Encoding

- All strings are encoded as UTF-8.
- Escape sequences follow JSON rules.
- AJIS does not define Unicode normalization.
- Byte sequences are preserved as-is.

---

## 7. Object Keys

- Object keys are strings.
- Object keys are **case-sensitive** (byte-for-byte).
- Duplicate keys are an error.
- No Unicode case-folding or normalization is performed.

AJIS does not define case-insensitive maps.

---

## 8. Arrays

- Arrays may contain values of different types.
- AJIS does not enforce homogeneous arrays.

---

## 9. Comments

AJIS allows comments:

- line comments: `// ...` until end of line
- block comments: `/* ... */` (non-nested)

Comments are ignored by the parser.

Comments MAY appear anywhere whitespace is permitted (i.e., between tokens).
Comments MUST NOT appear inside tokens.

Examples (valid):

```ajis
{ "a" /* key */: /* value */ 1, "b": 2 }
```

Examples (invalid):

```ajis
tr/*x*/ue
```

---

## 10. Numbers

AJIS supports explicit numeric formats:

- signed 64-bit integers,
- 64-bit floating-point numbers,
- hexadecimal and binary integer literals,
- optional digit separators.

### 10.1 Integer literals

- Decimal integers: `-?[0-9]+`
- Hexadecimal integers: `-?0x[0-9A-Fa-f]+`
- Binary integers: `-?0b[01]+`

Leading zeros in decimal integers are not permitted (e.g., `01` is invalid).
`0` is permitted.

### 10.2 Floating-point literals

Floating-point literals follow JSON-style rules using:

- decimal point `.` (dot)
- optional exponent `e` or `E`

AJIS uses `.` as the only decimal separator.

### 10.3 Digit separators

AJIS permits digit separators in numbers to improve readability.

Permitted digit separators:

- underscore `_` (U+005F)
- comma `,` (U+002C)
- ASCII space ` ` (U+0020)

#### General Rules

- A separator MAY appear only between digits.
- A separator MUST NOT appear at the beginning or end of the digit sequence.
- A separator MUST NOT appear adjacent to the decimal point or exponent marker.
- A separator MUST NOT appear immediately after a base prefix (`0x`, `0b`, `0o`).
- Only one separator type MAY be used within a single number.
- Double separators (consecutive separators) are not permitted.
- Separators have no semantic meaning and are ignored during numeric conversion.

#### Base-Specific Grouping Rules

Different number bases have specific grouping requirements to maintain consistency and readability:

**Decimal numbers:**
- Grouping by 3 digits (thousands separator convention)
- First group: 1-3 digits
- Subsequent groups: exactly 3 digits
- Examples: `1_000`, `12_345_678`, `1,000,000`, `999 999 999`

**Binary numbers (`0b`):**
- Grouping by 4 bits (nibble/hex-digit alignment)
- First group: 1-4 bits
- Subsequent groups: exactly 4 bits
- Examples: `0b1010_1111`, `0b10_1010_0011`, `0b1111 0000 1010`

**Hexadecimal numbers (`0x`):**
- Grouping by 2 OR 4 hex digits (byte or word alignment)
- First group determines the grouping mode:
  - 1-2 digits → subsequent groups must be 2 digits
  - 3-4 digits → subsequent groups must be 4 digits
- All subsequent groups must match the established size
- Examples: `0xFF_AA_00` (by-2), `0xDEAD_BEEF` (by-4), `0xF_AA_BB_CC` (by-2)

**Octal numbers (`0o`):**
- No strict grouping requirement (flexible)
- Separators may appear between any digits for visual clarity
- Examples: `0o777_123`, `0o7_55`, `0o1_234_567`

#### Examples (valid)

Decimal:
- `1_000_000`
- `1,000,000`
- `1 000 000`
- `12_345_678`

Binary:
- `0b1010_1111`
- `0b10_1010_0011`
- `0b1111,0000,1010`

Hexadecimal:
- `0xFF_AA_00_BB` (grouping by 2)
- `0xDEAD_BEEF` (grouping by 4)
- `0xF_AA` (grouping by 2, first group is 1 digit)

Octal:
- `0o777_123`
- `0o7_55`

#### Examples (invalid)

- `1,000_000` (mixed separators: comma and underscore)
- `1 000,000` (mixed separators: space and comma)
- `1._000` (adjacent to decimal point)
- `1,` (trailing separator)
- `_1000` (leading separator)
- `0b10_11` (binary grouping by 2 instead of 4)
- `0b1010_10` (inconsistent binary grouping: 4 then 2)
- `0xFF_AABB` (hex mixing grouping-by-2 and grouping-by-4)
- `0xFFAA_BB` (hex inconsistent: 4 then 2)























































Future versions must remain backward compatible where possible.This document specifies AJIS version 1.## 15. Versioning---Canonical form is required for test vectors.- deterministic output.- normalized whitespace,- no trailing commas,- object keys are ordered lexicographically (byte-wise),AJIS defines a canonical serialization:## 14. Canonical Form---canonical serialization.but recovery behavior is outside the scope of AJIS v1 and MUST NOT affectImplementations MAY provide a non-normative recovery mode for tooling purposes,- Parsers MUST reject any input that does not conform to this specification.Strict parsing is normative:## 13. Parsing Strictness---Binary values are intended for small to medium payloads.- base64 (`b64"..."`)- hexadecimal (`hex"..."`)AJIS supports binary literals:## 12. Binary Values---The exact literal form is reserved for full definition after v1 stabilization.AJIS supports a `char` value representing a single Unicode scalar value.## 11. Char Values---- `1__000` (double separator)- `1e_5` (separator adjacent to exponent)- `0x_DEAD` (separator immediately after prefix)- `0x_DEAD` (separator immediately after prefix)
- `1e_5` (separator adjacent to exponent)
- `1__000` (double separator)

---

## 11. Char Values

AJIS supports a `char` value representing a single Unicode scalar value.

The exact literal form is reserved for full definition after v1 stabilization.

---

## 12. Binary Values

AJIS supports binary literals:

- hexadecimal (`hex"..."`)
- base64 (`b64"..."`)

Binary values are intended for small to medium payloads.

---

## 13. Parsing Strictness

Strict parsing is normative:

- Parsers MUST reject any input that does not conform to this specification.

Implementations MAY provide a non-normative recovery mode for tooling purposes,
but recovery behavior is outside the scope of AJIS v1 and MUST NOT affect
canonical serialization.

---

## 14. Canonical Form

AJIS defines a canonical serialization:

- object keys are ordered lexicographically (byte-wise),
- no trailing commas,
- normalized whitespace,
- deterministic output.

Canonical form is required for test vectors.

---

## 15. Versioning

This document specifies AJIS version 1.

Future versions must remain backward compatible where possible.