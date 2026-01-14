# AJIS Tool Architecture

Overview of AJIS components.

## Pipeline

```
Input → Reader → Lexer → Parser → Serializer → Writer → Output
```

## Components

### Lexer (✅ Done in C)

Tokenizes the input text.

**Features:**
- All AJIS tokens
- Comments (line + block)
- Numbers (dec, hex, bin, oct)
- String escape sequences
- Zero-copy spans
- Error reporting (line, column)

### Parser (📋 Planned)

Creates an AST from tokens.

### Serializer (📋 Planned)

Converts AST to AJIS/JSON text.

### Reader/Writer (📋 Planned)

File/Stream I/O.

## Design Principles

1. **Modularity** - Independent components
2. **Zero-copy** - Spans instead of copies
3. **Strict parsing** - All errors reported
4. **Determinism** - Canonical form
5. **Security** - Depth/size limits
