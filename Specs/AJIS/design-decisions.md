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
