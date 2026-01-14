# AJIS - Quick Start

This document allows you to understand AJIS and start using it in just 5 minutes.

## What is AJIS?

AJIS (Afrowave JSON Inspired Serializer) is a data serialization format:
- 100% compatible with JSON
- Adds comments, improved numeric formats, and binary data support
- Deterministic and secure
- Easy to parse

## Simplest Example

```ajis
{
  "name": "Alice",
  "age": 30
}
```

This is **valid AJIS** - and also **valid JSON**.

## Comments

```ajis
{
  // Line comment
  "name": "Bob",
  /* Block comment */
  "age": 25
}
```

## Numeric Formats

```ajis
{
  "decimal": 255,
  "hex": 0xFF,
  "binary": 0b11111111,
  "octal": 0o377
}
```

## Digit Separators

```ajis
{
  "millions": 1_000_000,
  "bytes": 0xFF_AA_BB,
  "bits": 0b1111_0000
}
```

## Binary Data

```ajis
{
  "hex_data": hex"DEADBEEF",
  "base64": b64"SGVsbG8gV29ybGQ="
}
```

## Next Steps

1. [Format Specification](./format-specification.md) - Details
2. [Examples](./examples.md) - Practical Examples
3. [Tool Architecture](./tool-architecture.md) - Implementation
