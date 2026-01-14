# AJIS - .NET Implementation

Complete implementation of AJIS in C# for .NET applications.

## Status

| Component | Status | Priority |
|-----------|--------|----------|
| Lexer | 📋 Future | P1 |
| Parser | 📋 Future | P1 |
| Serializer | 📋 Future | P2 |
| Reader/Writer | 📋 Future | P2 |

## Design Goals

✅ Modern .NET (async/await, LINQ)
✅ Zero-allocation hot paths
✅ Performance-first
✅ JSON compatibility
✅ Rich diagnostics

## Structure (Planned)

```
AJIS.Core/
├── Lexing/
├── Parsing/
├── Serialization/
├── IO/
└── Errors/

AJIS.Tests/
├── LexerTests.cs
├── ParserTests.cs
└── SerializerTests.cs
```

## Planned API

```csharp
// Reading
var value = AjisParser.Parse("{\"name\": \"test\"}");

// Writing
var json = AjisSerializer.Serialize(value);

// Files
var data = await AjisFile.ReadAsync("config.ajis");
await AjisFile.WriteAsync("output.ajis", data);

// Streaming
using var reader = AjisReader.Create(File.OpenRead("data.ajis"));
while (var value = await reader.ReadAsync(); value != null)
{
    Process(value);
}
```

## Development Phases

- **Phase 1:** Foundation (Q1 2024)
- **Phase 2:** Parsing (Q2 2024)
- **Phase 3:** Serialization (Q3 2024)
- **Phase 4:** I/O & Advanced (Q4 2024)
- **Phase 5:** Tooling (2025+)

## Integration

```bash
cd src/runtime/dotnet
dotnet build As426.sln
dotnet test AJIS.Tests/
```

## Documentation

- [Getting Started](../c/docs/getting-started.md)
- [Format Specification](../c/docs/format-specification.md)
- [Tool Architecture](../c/docs/tool-architecture.md)
- [Examples](../c/docs/examples.md)

## Reference

- [AJIS v1 Spec](../../Specs/AJIS/ajis-v1.md)
- [Design Decisions](../../Specs/AJIS/design-decisions.md)

---

**Status:** Pre-release, design phase
