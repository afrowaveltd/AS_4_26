# AJIS - Developer Guide

Manual for developers working on the AJIS project.

## Setup

### C Environment
```bash
cd Tools/AJIS/c
gcc -I include src/ajis_lexer.c tests/test.c -o test_lexer
./test_lexer --all
```

### .NET Environment
```bash
cd src/runtime/dotnet
dotnet build As426.sln
dotnet test AJIS.Tests/
```

## Coding Standards

### C
- 4-space indent
- Descriptive function names
- Inline comments for non-trivial logic
- Error handling everywhere

### C# (.NET)
- 4-space indent
- PascalCase for classes/methods
- camelCase for local variables
- Nullable reference types enabled

## Workflow

1. **Pick an issue** - GitHub Issues
2. **Branching** - `git checkout -b feature/issue-123`
3. **Development** - Write code and tests
4. **Testing** - `make test` or `dotnet test`
5. **Commit** - `git commit -m "feat: description"`
6. **Push & PR** - Open a pull request

## Commit Messages

Format: `type: description`

- feat: New feature
- fix: Bug fix
- docs: Documentation
- test: Tests
- perf: Performance

## Contribution Checklist

- [ ] Code follows standards
- [ ] Tests pass
- [ ] No compiler warnings
- [ ] Documentation updated
- [ ] Clear commit message
- [ ] No sensitive data included
