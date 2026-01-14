# AJIS - C Implementation

Minimalist AJIS lexer implementation.

## Status

| Component | Status |
|-----------|--------|
| Lexer | ✅ Done |
| Parser | 📋 Planned |
| Serializer | 📋 Planned |

## Features

✅ **Supported:**
- All AJIS tokens
- Comments (// and /* */)
- Numbers (decimal, hex, binary, octal)
- Digit separators
- String escape sequences
- Zero-copy token spans
- Error reporting (line, column)

## Compilation

```bash
cd Tools/AJIS/c
gcc -I include src/ajis_lexer.c tests/test.c -o bin/test_lexer
./bin/test_lexer --all
```

## API

```c
ajis_input input = {
    .data = "{\"key\": 42}",
    .length = 12,
    .offset = 0,
    .line = 1,
    .column = 1
};

ajis_lexer_options opts = {0};
ajis_lexer lexer;
ajis_lexer_init(&lexer, &input, opts);

ajis_token token;
ajis_error error;

while (ajis_lexer_next(&lexer, &token, &error) == AJIS_OK) {
    if (token.type == AJIS_TOKEN_EOF) break;
}
```

## Documentation

- [Getting Started](./docs/getting-started.md)
- [Format Specification](./docs/format-specification.md)
- [Tool Architecture](./docs/tool-architecture.md)
- [Examples](./docs/examples.md)
- [Developer Guide](./docs/developer-guide.md)
- [Documentation Index](./docs/README.md)
