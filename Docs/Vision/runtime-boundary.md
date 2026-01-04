# Runtime Boundary (AS426)

This document defines the **explicit boundary of the Runtime layer** in AS426.
It is normative: it constrains responsibilities, permissions, and dependencies.

The Runtime exists to **execute meaning**, not to manage bytes.

---

## 1. Purpose

The Runtime is responsible for:

* interpreting typed data and schemas
* executing higher-level operations deterministically
* coordinating queries, transformations, and workflows
* exposing stable APIs to tools, UI, and services

The Runtime **does not own persistence**.

---

## 2. What the Runtime IS

The Runtime:

* is platform-independent
* is deterministic by design
* operates on validated, typed inputs
* produces explicit, observable outputs
* treats storage as an external service

It is the **semantic engine** of AS426.

---

## 3. What the Runtime IS NOT

The Runtime must never:

* perform raw I/O
* access disks, files, or block devices
* parse or depend on binary layout details
* manage checksums, alignment, or durability
* bypass AfFS contracts

These responsibilities belong to lower layers.

---

## 4. Inputs

The Runtime consumes:

* typed records (from AfFS)
* schemas and metadata
* queries or execution requests
* validated inter-language programs

All inputs must be:

* versioned
* validated
* explicit

---

## 5. Outputs

The Runtime produces:

* query results
* transformed records
* execution reports
* error objects
* audit traces

All outputs are:

* explicit
* serializable
* reproducible

---

## 6. Determinism Contract

Given:

* the same input data
* the same schema
* the same inter-language program

The Runtime must:

* produce the same output
* in the same order
* with the same observable side effects

Non-determinism is forbidden unless explicitly declared.

---

## 7. Error Contract

Runtime errors:

* are values, not exceptions
* are typed and versioned
* never hide lower-layer failures

The Runtime must surface:

* validation errors
* execution errors
* dependency failures

---

## 8. Security Contract

The Runtime:

* executes only validated programs
* enforces capability-based access
* never escalates privileges
* cannot access storage directly

Security decisions are explicit and auditable.

---

## 9. Observability

The Runtime must expose:

* execution traces
* timing information
* resource usage summaries
* decision paths

Observability is required for trust and debugging.

---

## 10. Layer Boundaries

The Runtime may call:

* AfFS APIs (logical, not physical)
* schema services
* inter-language evaluators

The Runtime must not call:

* Backend I/O
* OS APIs
* platform-specific services

---

## 11. Invariants

* Runtime has no hidden side effects
* Runtime never mutates storage directly
* Runtime behavior is explainable
* Runtime is restart-safe

---

End of document.
