# AJIS – Afrowave JSON‑Inspired Serialization

AJIS (Afrowave JSON‑Inspired Serialization) is a human‑readable text format for AUV values.

It is **inspired by JSON**, but it is **not JSON**.

AJIS exists to provide:

* a clear, editable, and debuggable representation of AUV values,
* deterministic parsing without ambiguity,
* features that JSON intentionally does not provide (typed numbers, binary literals).

AJIS is intended for:

* configuration files,
* import/export,
* debugging and inspection,
* documentation examples.

AJIS is **not** the internal storage or transport format.
The canonical internal format remains **AUV Wire**.

---

## Relationship to JSON

AJIS borrows familiar ideas from JSON:

* object and array syntax,
* quoted strings,
* boolean literals,
* `null`.

AJIS intentionally diverges where JSON is ambiguous or limiting:

* explicit numeric typing,
* hexadecimal and binary literals,
* explicit binary encodings,
* clear character literals.

Calling AJIS “JSON” would be misleading.

---

## Core principles

AJIS is:

* human‑readable,
* deterministic,
* case‑sensitive (except for keywords),
* lossless with respect to AUV types.

Every valid AJIS value maps to exactly one AUV value.

---

## Whitespace and formatting

* Whitespace is insignificant except inside strings.
* Newlines, spaces, and tabs may be used freely.
* Formatting has no semantic meaning.

---

## Keywords

AJIS keywords are case‑insensitive:

* `true`, `false` → Bool
* `null` → Null

---

## Numeric literals

AJIS provides explicit numeric typing.

### Integer literals (Int64)

Forms:

* Decimal: `42`, `-10`
* Hexadecimal: `0xFF`, `0xDEAD_BEEF`
* Binary: `0b10101010`
* Octal (optional): `0o755`

Rules:

* `_` MAY be used as a digit separator in all numeric bases.
* Examples: `1_000`, `0xAA_BB`, `0b0000_1111`.
* All integer literals map to **AUV Int64**.

---

### Floating‑point literals (Float64)

Forms:

* Decimal with fraction: `10.0`, `-3.14`
* Scientific notation: `1e3`, `-2.5E-4`
* Optional suffix: `f` or `F`

Rules:

* Any literal containing `.` or exponent maps to **AUV Float64**.

---

## String literals

Strings are enclosed in double quotes:

```text
"hello world"
```

Rules:

* Strings represent **AUV String** values.
* Strings use UTF‑8 encoding.
* Escape sequences follow JSON conventions (`\n`, `\t`, `\uXXXX`).

---

## Character literals

Characters represent **AUV Char** values.

Forms:

* Single quoted: `'A'`
* Unicode form: `U+0041`

Rules:

* Character literals MUST represent exactly one Unicode scalar value.

---

## Binary literals

Binary data maps to **AUV Binary**.

Supported forms:

* Hex block:

```text
hex"DE AD BE EF"
```

* Base64 block:

```text
b64"3q2+7w=="
```

Rules:

* Whitespace inside hex blocks is ignored.
* Binary literals have no implicit text encoding.

---

## Arrays

Arrays are ordered lists of values:

```text
[ 1, 2, 3 ]
```

Arrays map to **AUV Array**.

---

## Objects

Objects are key‑value mappings:

```text
{
  "name": "Alice",
  "age": 30,
  "active": true
}
```

Rules:

* Keys MUST be strings.
* Keys are case‑sensitive.
* Duplicate keys are not allowed.

Objects map to **AUV Object**.

---

## Comments

AJIS supports comments for human readability.

Forms:

* Single‑line: `// comment`
* Block: `/* comment */`

Comments are ignored by the parser.

---

## Determinism and safety

AJIS parsing MUST be deterministic:

* no implicit type guessing,
* no locale‑dependent behavior,
* no silent coercion.

Invalid input MUST result in a parse error.

---

## Relationship to AUV Wire

AJIS is a textual view of AUV.

* AJIS → AUV → AUV Wire
* AUV Wire → AUV → AJIS

AJIS does not replace AUV Wire.

---

## Summary

AJIS is a JSON‑inspired, AUV‑native text format.

It:

* looks familiar,
* removes ambiguity,
* supports explicit typing,
* and remains human‑friendly.

It is a tool for people.
The system itself speaks AUV Wire.

---

## 8. Well-known typed wrappers (non-normative)

AUV intentionally keeps its base type set small.
Higher layers may use well-known wrapper objects to represent additional logical types.

This section is **non-normative**: it recommends conventions but does not change AUV itself.

Recommended wrapper shape:

* An Object with reserved keys:

  * `"$t"`: String type name
  * `"$v"`: AUV value payload
  * optional additional fields (e.g., `"unit"`, `"tz"`, `"scale"`)

Examples:

* Timestamp (epoch milliseconds):

  * `{ "$t": "time", "$v": 1735862400000, "unit": "ms", "tz": "UTC" }`

* Decimal (arbitrary precision, as string):

  * `{ "$t": "decimal", "$v": "1234.5600" }`

* UUID (canonical AUV form is Binary(16)):

  * `{ "$t": "uuid", "$v": <Binary(16)> }`

Implementations MAY support additional wrappers, but SHOULD keep the wrapper surface minimal.

---

## 9. Test vectors (AUV Wire v1)

Notes:

* Hex bytes are written as space-separated pairs.
* VarUInt lengths shown here all fit in one byte.

### 9.1 Primitive values

* Null

  * AJIS: `null`
  * Wire: `00 00`

* Bool (true)

  * AJIS: `true`
  * Wire: `01 01 01`

* Int64 (1)

  * AJIS: `1`
  * Wire: `02 08 01 00 00 00 00 00 00 00`

* Float64 (1.0)

  * AJIS: `1.0`
  * Wire: `03 08 00 00 00 00 00 00 F0 3F`

* Char ('A')

  * AJIS: `'A'`
  * Wire: `04 04 41 00 00 00`

* String ("hi")

  * AJIS: `"hi"`
  * Wire: `05 02 68 69`

* Binary (DE AD BE EF)

  * AJIS: `hex"DE AD BE EF"`
  * Wire: `06 04 DE AD BE EF`

### 9.2 Composite values

* Array ([1, true])

  * AJIS: `[ 1, true ]`
  * Payload: `02 08 01 00 00 00 00 00 00 00 01 01 01`
  * Wire: `07 0D 02 08 01 00 00 00 00 00 00 00 01 01 01`

* Object ({"a": 1})

  * AJIS: `{ "a": 1 }`
  * Payload: `05 01 61 02 08 01 00 00 00 00 00 00 00`
  * Wire: `08 0D 05 01 61 02 08 01 00 00 00 00 00 00 00`

### 9.3 Boundary and special cases

#### Int64 boundaries

* Zero

  * AJIS: `0`
  * Wire: `02 08 00 00 00 00 00 00 00 00`

* Minus one

  * AJIS: `-1`
  * Wire: `02 08 FF FF FF FF FF FF FF FF`

* Int64 max

  * AJIS: `9223372036854775807`
  * Wire: `02 08 FF FF FF FF FF FF FF 7F`

* Int64 min

  * AJIS: `-9223372036854775808`
  * Wire: `02 08 00 00 00 00 00 00 00 80`

#### Float64 special values (IEEE 754)

* +0.0

  * AJIS: `0.0`
  * Wire: `03 08 00 00 00 00 00 00 00 00`

* -0.0

  * AJIS: `-0.0`
  * Wire: `03 08 00 00 00 00 00 00 00 80`

* +Infinity

  * AJIS: `inf`
  * Wire: `03 08 00 00 00 00 00 00 F0 7F`

* -Infinity

  * AJIS: `-inf`
  * Wire: `03 08 00 00 00 00 00 00 F0 FF`

* NaN (canonical quiet NaN)

  * AJIS: `nan`
  * Wire: `03 08 00 00 00 00 00 00 F8 7F`

#### UTF-8 strings (non-ASCII)

* Czech character

  * AJIS: `"č"`
  * UTF-8: `C4 8D`
  * Wire: `05 02 C4 8D`

* Emoji

  * AJIS: `"🙂"`
  * UTF-8: `F0 9F 99 82`
  * Wire: `05 04 F0 9F 99 82`

#### Nested structures (exact lengths)

* Nested array

  * AJIS: `[ [1], [true, null] ]`
  * Wire:

    * `07 13 07 0A 02 08 01 00 00 00 00 00 00 00 07 05 01 01 01 00 00`

* Nested object

  * AJIS: `{ "a": { "b": 1 } }`
  * Wire:

    * `08 12 05 01 61 08 0D 05 01 62 02 08 01 00 00 00 00 00 00 00`

Notes:

* Object key order is canonical UTF-8 lexicographic order.
* Length bytes in the examples above are exact.

---

## 10. Real-world test vector (ApiResponse-like)

This example demonstrates a typical structured response object with a nested `data` object.

### 10.1 AJIS

```text
{
  "data": {
    "explain": "Lists active jobs and prints extra details.",
    "risk": "low",
    "script": "jobs list -please"
  },
  "message": "",
  "success": true
}
```

### 10.2 Wire (AUV Wire v1)

Canonical object key order:

* outer: `data`, `message`, `success`
* inner `data`: `explain`, `risk`, `script`

Wire:

`08 7B 05 04 64 61 74 61 08 5C 05 07 65 78 70 6C 61 69 6E 05 2B 4C 69 73 74 73 20 61 63 74 69 76 65 20 6A 6F 62 73 20 61 6E 64 20 70 72 69 6E 74 73 20 65 78 74 72 61 20 64 65 74 61 69 6C 73 2E 05 04 72 69 73 6B 05 03 6C 6F 77 05 06 73 63 72 69 70 74 05 11 6A 6F 62 73 20 6C 69 73 74 20 2D 70 6C 65 61 73 65 05 07 6D 65 73 73 61 67 65 05 00 05 07 73 75 63 63 65 73 73 01 01 01`

---

## 11. AJIS float literals for special values (non-normative)

AJIS MAY provide dedicated literals for IEEE 754 special values:

* `inf`, `-inf`
* `nan`

If supported, these literals MUST map to the canonical Float64 encodings used in section 9.3.
