# Runtime – Plan

This document defines the intended scope and milestones for the AS 4_26 runtime.
It captures what we want to build (and why), and how we will evolve it without losing clarity.

This is a plan, not a specification.

---

## What the runtime must do

The runtime is the living core of AS 4_26. It must:

* host the active data state,
* expose a unified command/query surface,
* coordinate persistence through AfFS,
* enforce safety rules and invariants,
* serve multiple clients (CLI, TUI, Web) consistently,
* support deterministic recovery flows.

---

## Milestone philosophy

We build in layers.

1. Define stable contracts (Docs → Specs).
2. Implement the smallest correct core.
3. Add ergonomics and performance as controlled layers.

Safety is never an optional layer.

---

## v0 (Documentation lock-in)

Goal: make the system implementable without improvisation.

Deliverables:

* Docs: Vision, Principles, Glossary
* Docs: AUV (plan + en)
* Docs: Runtime (en + plan)
* Docs: Storage, AfFS Architecture, AfFS DiskLayout (concept)
* Docs: UI, Personalities, Commands
* Specs skeletons created (headings + versioning policy placeholders)

Exit criteria:

* conceptual circle closed,
* terminology stable,
* no missing “core meaning” documents.

---

## v1 (Minimal correct runtime)

Goal: a minimal, correct runtime that can store data, survive crashes, and be operated from CLI.

### v1.1 Core execution

* command parser (case-insensitive keywords)
* command dispatcher
* query pipeline
* structured AUV input/output everywhere
* consistent error model

### v1.2 Storage integration

* open/mount volume
* read/write basic entities through AfFS
* flush and durability barriers
* recovery on startup
* rollback to last published state (must-have)

### v1.3 Observability

* structured logs (AUV-friendly)
* trace IDs for commands
* clear error codes and messages

### v1.4 CLI

* non-interactive mode (scripting)
* stable machine-readable output

Exit criteria:

* a volume can be created and reopened,
* commands can create/read/update/delete entities safely,
* crash simulation shows deterministic recovery,
* rollback works.

---

## v2 (Multi-client and ergonomics)

Goal: same runtime capabilities exposed to multiple interfaces with better UX.

### v2.1 Sessions

* session context (identity, working scope)
* consistent authentication hooks (even if stubbed initially)

### v2.2 Web UI (reference)

* Kestrel-hosted admin cockpit
* embedded terminal as a real client

### v2.3 TUI

* interactive navigation
* safe workflows

### v2.4 Command UX helpers

* history (with filtering modes)
* inline help and completions (intellisense-like)
* discoverability commands (help, describe, examples)

Exit criteria:

* Web UI + embedded terminal operate the same runtime
* TUI can be launched locally and through the embedded terminal
* command help/completions are consistent across clients

---

## v3 (Performance and advanced storage)

Goal: controlled optimizations and multi-backend strategies.

Possible targets:

* multi-backend volumes (tiering: SSD hot / HDD cold)
* placement policies based on object type/size/access patterns
* caching strategies (read caches, metadata caches)
* background compaction / reclaim

Exit criteria:

* optimizations do not weaken safety
* behavior remains deterministic and explainable

---

## Hard rules (non-negotiable)

* rollback is a core feature, not optional
* durability is defined by publish/commit boundaries
* recovery must be deterministic and repeatable
* UI clients never bypass the runtime
* contracts (Specs) define truth; implementations follow

---

## Open questions (tracked, not blocked)

* exact command grammar style (SQL-like, shell-like, hybrid)
* transport protocol for remote clients
* authentication and authorization model
* how personalities package command sets and UI defaults
* how tiering and placement policies are expressed

---

## Next documents

After this plan:

* formalize AUV.Spec
* formalize AfFS.DiskLayout
* define command grammar and protocol specs
* begin .NET reference implementation
