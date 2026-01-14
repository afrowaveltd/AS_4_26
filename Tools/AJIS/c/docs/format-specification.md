# AJIS Format Specification

Detailed description of the AJIS format.

## Data Types

AJIS supports:
- **null** - Absence of value
- **bool** - true/false
- **int64** - Integer
- **float64** - Floating point number
- **string** - UTF-8 text
- **binary** - Binary data (hex/b64)
- **array** - Heterogeneous array
- **object** - Key-value maps

## Numbers

- Decimal: `123`, `-45`
- Hex: `0xFF`, `0xDEADBEEF`
- Binary: `0b1010`, `0b1111_0000`
- Octal: `0o755`, `0o644`
- Separators: `1_000_000`, `0xFF_AA`

## Strings

UTF-8 in double quotes with escape sequences:
- `\n` - New line
- `\t` - Tabulator
- `\\` - Backslash
- `\u0041` - Unicode

## Comments

- `// Line comment`
- `/* Block comment */`

## JSON Compatibility

Every JSON is a valid AJIS.
