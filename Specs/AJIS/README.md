# AJIS – Afrowave JSON-inspired Serialization

AJIS is a lightweight, deterministic serialization format used within the AS426 ecosystem.

It is inspired by JSON but intentionally extends it with:

- strict and explicit data types,
- binary-safe literals,
- comments,
- and canonical serialization rules.

AJIS is designed to be:

- easy to parse in low-level languages (C),
- convenient to use in higher-level languages (.NET, Python, JS),
- deterministic and implementation-independent.

AJIS is a **data format**, not a programming language.

## Documents in this directory

- `ajis-v1.md`  
  Normative specification of AJIS version 1.

- `compat-json.md`  
  Description of JSON compatibility and differences.

- `grammar-notes.md`  
  Informal grammar notes for implementers.

- `design-decisions.md`  
  Recorded design decisions and rationale (ADR-style).

- `test-vectors/`  
  Canonical test inputs and expected outputs.

---

AJIS is a **data format**, not a programming language.
