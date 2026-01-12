# AJIS and JSON Compatibility

AJIS is a strict superset of JSON syntax.

AJIS is designed to **complement JSON, not replace it**.

## Compatibility Rules

- Any valid JSON document is a valid AJIS document.
- AJIS parsers must accept standard JSON input.

## AJIS Extensions Beyond JSON

- comments (`//`, `/* ... */`)
- binary literals
- explicit char values
- hexadecimal and binary integers
- digit separators in numbers
- canonical serialization rules

## Non-Compatible Features

- JSON parsers cannot read AJIS extensions.
- AJIS does not support JavaScript-specific semantics.

## AJIS -> JSON Conversion (Information Loss)

AJIS can be converted to JSON, but some information may be lost:

- `binary` must be encoded (e.g., base64) into a JSON string.
- `char` may be represented as a JSON string (length 1).
- numeric formatting (digit separators, hex/bin notation) is not preserved.

JSON is a useful interchange format for interoperability, while AJIS is intended
for stricter typed data and deterministic tooling.
