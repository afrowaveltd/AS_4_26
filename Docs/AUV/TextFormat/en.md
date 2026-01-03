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
