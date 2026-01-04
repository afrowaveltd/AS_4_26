# Inter-Language Principles (AS426)

This document defines the **principles and invariants** of the AS426 inter-language.
The inter-language is the formal communication medium between:

* Runtime
* tools and UI
* automation and orchestration layers

It is **not** a user-facing programming language.
It is a **semantic contract language**.

---

## 1. Purpose

The inter-language exists to:

* express *intent* and *meaning* in a portable form
* allow deterministic execution by the Runtime
* provide auditability and explainability
* decouple high-level logic from implementation details

The inter-language is the "nervous system" of AS426.

---

## 2. Nature of the Language

The inter-language is:

* **declarative**, not imperative
* **data-centric**, not control-centric
* **side-effect constrained**
* **fully serializable**

It describes *what is to be done*, not *how to do it*.

---

## 3. Determinism

Given:

* the same inter-language program
* the same input data
* the same schema and metadata

Execution must:

* produce the same result
* in the same order
* with the same observable trace

Non-deterministic constructs are forbidden.

---

## 4. Side Effects

Allowed side effects:

* producing explicit outputs
* emitting audit records
* producing derived data (via Runtime-controlled mechanisms)

Forbidden side effects:

* raw I/O
* time-based behavior
* randomness
* hidden state mutation

---

## 5. Validation and Safety

Before execution, every inter-language program must be:

* syntactically valid
* semantically valid
* schema-compatible
* capability-authorized

Invalid programs must never execute partially.

---

## 6. Versioning and Compatibility

Inter-language programs:

* carry an explicit version
* are forward- and backward-compatible where possible
* must fail explicitly on incompatible versions

Compatibility rules are part of the language contract.

---

## 7. Error Model

Errors in inter-language execution:

* are values, not exceptions
* are typed and structured
* include context and cause

Errors must be representable as data.

---

## 8. Observability and Audit

Execution must produce:

* a trace of evaluated steps
* inputs and outputs per step
* decision rationale (where applicable)

Audit data must be:

* complete
* reproducible
* inspectable after the fact

---

## 9. Security Model

The inter-language:

* operates under explicit capabilities
* cannot escalate privileges
* cannot bypass Runtime validation

All permissions are explicit and auditable.

---

## 10. What the Inter-Language IS NOT

The inter-language is not:

* a scripting language
* a general-purpose programming language
* a bytecode VM
* a storage access language

These concerns belong elsewhere.

---

## 11. Invariants

* Every program is serializable
* Every execution is explainable
* Every effect is explicit
* Every error is visible
* No hidden state exists

---

End of document.
