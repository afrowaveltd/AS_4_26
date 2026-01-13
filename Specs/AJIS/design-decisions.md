# AJIS Design Decisions

This document records important design decisions for AJIS.

---

## DD-001: Case Sensitivity of Object Keys

**Decision:**  
Object keys are always case-sensitive.

**Rationale:**  

- Deterministic behavior.
- Matches JSON semantics.
- Avoids Unicode case-folding complexity.
- Case-insensitive behavior can be implemented at application or writer level.

---

## DD-002: Unicode Normalization

**Decision:**  
AJIS does not define Unicode normalization rules.

**Rationale:**  

- Keeps the format simple.
- Avoids locale-dependent behavior.
- Preserves byte-level fidelity.

---

## DD-003: Binary Data Support

**Decision:**  
AJIS supports native binary literals.

**Rationale:**  

- Enables embedding of glyphs, assets, and small binary payloads.
- Avoids misuse of strings for binary data.

---

## DD-004: Parser Configuration via Input

**Decision:**  
AJIS input data must not change parser behavior.

**Rationale:**  

- Prevents dialect fragmentation.
- Improves security and interoperability.

---

## DD-005: Strict Core, Optional Recovery

**Decision:**  
AJIS v1 parsing is strict and normative. Recovery is non-normative.

**Rationale:**  

- Ensures deterministic behavior and interoperability.
- Avoids silent fixes and ambiguous interpretation.
- Recovery belongs to tooling (formatters, linters, importers), not the protocol.

---

## DD-006: Digit Separators in Numbers

**Decision:**  
AJIS permits digit separators in numbers: ASCII space, comma, or underscore.
Only one separator type may be used per number.

**Rationale:**  

- Improves readability in configs and debug output.
- Supports common conventions without introducing locale-dependent behavior.
- Prevents ambiguity by forbidding mixed separators.
- Decimal separator remains `.` only; comma is never a decimal separator.

## DD-007: String handling modes (RAW vs DECODED)

AJIS distinguishes between two string handling modes at parse time:

### RAW mode (default, normative)

- String tokens carry a raw span into the original input buffer.
- No unescaping or decoding is performed by the lexer or parser.
- The parser guarantees correct string literal syntax (quotes and escapes).
- This mode avoids allocations and is suitable for high-performance and low-level use cases.

### DECODED mode (optional)

- String contents are unescaped and decoded into a target buffer.
- Requires a caller-provided allocation strategy (arena, allocator callback, etc.).
- Intended for higher-level consumers that prefer convenience over raw performance.

RAW mode is the normative baseline for AJIS v1. DECODED mode is optional and non-normative.

---

## DD-008: Multiline strings (non-normative option)

By default, AJIS string literals follow JSON-compatible rules:

- Unescaped newline characters inside string literals are considered invalid.

An optional non-normative parser option MAY allow multiline strings:

- Raw newline characters inside string literals are accepted.
- Newlines are normalized internally to the escape sequence `\n`.
- Canonical output MUST escape newlines as `\n`.
- Round-trip preservation of raw newlines is not guaranteed.

This option exists to support practical use cases such as large text blocks,
translations, and documentation strings, while preserving deterministic
canonical output.
