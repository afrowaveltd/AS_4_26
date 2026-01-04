# AS426 Decision Log (Living)

This document is the **project memory** for AS426.
It records architecture decisions that must remain stable across time, threads, and implementations.

It is intentionally short, factual, and repeatable.

---

## How to use this log

* Add a new entry whenever we make a decision that influences multiple layers.
* Keep each entry small.
* Prefer *clear constraints* over long explanations.

### Entry format (ADR-lite)

* **ID:** ADR-YYYYMMDD-NN
* **Status:** Proposed | Accepted | Superseded
* **Decision:** one sentence
* **Context:** 2–5 bullet points
* **Consequences:** 2–6 bullet points
* **Related docs:** links/paths

---

## Decisions

### ADR-20260104-01

* **Status:** Accepted
* **Decision:** Implement bottom → top, and never implement a higher layer before the lower layer has tests and observable outputs.
* **Context:**

  * AS426 must remain resumable across threads.
  * Storage correctness requires deterministic, testable primitives.
* **Consequences:**

  * Early milestones prioritize scan/verify/report tooling.
  * “Truth first” tools become part of the base system.
* **Related docs:**

  * `Docs/Vision/plan.md`
  * `Docs/AfFS/Backend-IOLayer/en.md`
  * `Docs/AfFS/Binary-Layout-v1/en.md`

### ADR-20260104-02

* **Status:** Accepted
* **Decision:** AfFS is database-oriented storage: it stores typed records (Data Kind + Size Class + Access Profile), not generic files-first semantics.
* **Context:**

  * Inspired by AS/400 principles for a database OS in 2026.
  * Fragmentation and query performance must be handled by design.
* **Consequences:**

  * Segments declare intent and avoid mixed workloads.
  * Binary layout headers carry type and access metadata.
* **Related docs:**

  * `Docs/AfFS/Data-Kinds-and-Size-Classes/en.md`
  * `Docs/AfFS/Storage-Access-Profiles/en.md`
  * `Docs/AfFS/Binary-Layout-v1/en.md`

### ADR-20260104-03

* **Status:** Accepted
* **Decision:** Confine all platform dependencies to the Backend I/O Layer; no higher AfFS layer may call platform APIs directly.
* **Context:**

  * AfFS must run on host kernels and future AfNAS-OS.
* **Consequences:**

  * Backends must expose structured error outcomes and durability boundaries.
  * Higher layers can stay deterministic and portable.
* **Related docs:**

  * `Docs/AfFS/Backend-IOLayer/en.md`

### ADR-20260104-04

* **Status:** Accepted
* **Decision:** Redundancy uses two equivalent copies (A/B) with checksums; automatic repair is allowed only when correctness is provable.
* **Context:**

  * Trust requires visible degradation, not silent guesses.
  * Progressive media failure must be survivable.
* **Consequences:**

  * Scrubbing never guesses; repair is append-only and reported.
  * Mirror disagreement requires deterministic resolution rules.
* **Related docs:**

  * `Docs/AfFS/Failure-Scenarios/en.md`
  * `Docs/AfFS/Verification-and-Scrubbing/en.md`

---

## Appendices

### A) Runtime Boundary (template)

Create/maintain as: `Docs/Vision/runtime-boundary.md`

* **Purpose:** what Runtime is responsible for
* **Not responsible for:** explicit “no-go” responsibilities
* **Inputs:** what Runtime consumes
* **Outputs:** what Runtime produces
* **Determinism contract:** what must be reproducible
* **Error contract:** how errors are represented and surfaced
* **Security contract:** what it may and may not do
* **Observability:** required reports/metrics
* **Layer boundaries:** what Runtime must not bypass

### B) Inter-Language Principles (template)

Create/maintain as: `Docs/Vision/inter-language-principles.md`

* **Purpose:** why an inter-language exists
* **Nature:** declarative vs imperative (and why)
* **Side-effects:** allowed vs forbidden
* **Serialization:** must be serializable, versioned
* **Determinism:** evaluation must be reproducible
* **Safety:** validation rules before execution
* **Versioning:** forward/backward compatibility expectations
* **Observability:** traces, proofs, audit

---

End of document.
