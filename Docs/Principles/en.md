# Principles

This document defines the fundamental principles of AS 4_26.  
They are not implementation details, and they are not optional.

If a future design decision conflicts with these principles, the design must change.

---

## 1. Data is the first-class citizen

AS 4_26 is built around data, not applications.

Applications, interfaces, and tools exist to interact with data.  
Data does not exist to serve applications.

---

## 2. Userspace-first architecture

The AS 4_26 core runs entirely in userspace.

The operating system kernel is used only as an I/O transport layer:

- files
- block devices
- network

AS 4_26 does not replace the kernel and does not require special drivers.

---

## 3. Storage is a system, not a file format

AfFS (Afrowave Flexible Storage) is a storage engine, not a traditional filesystem.

It may be backed by:

- a regular file,
- a raw partition,
- a whole disk,
- or other transports in the future.

Storage behavior adapts to the nature of the stored data and the underlying medium.

---

## 4. One data model everywhere (AUV)

All data inside AS 4_26 is represented using AUV (Universal Values).

The same model is used for:

- storage
- APIs
- transport
- queries
- scripting

There are no “special cases” hidden behind APIs.

---

## 5. Specification before implementation

Formal specifications define the truth of the system.

Implementations:

- may differ internally,
- may be written in different languages,
- must follow the same Specs.

Specs are stable. Implementations evolve.

---

## 6. Deterministic core behavior

Given the same input and the same data state, the core must behave predictably.

Non-determinism (timing, concurrency, I/O variability) must be isolated from the core logic.

---

## 7. Interfaces are clients, not the system

CLI, TUI, Web UI, and APIs are all clients of the same runtime.

No interface is privileged.
No interface contains hidden logic.

---

## 8. Talk to the system like a human

AS 4_26 must be usable through:

- shell-style commands,
- scriptable interfaces,
- direct queries.

Interaction should feel closer to talking to a system than calling an API.

---

## 9. Personalities configure behavior, not truth

Personalities define:

- defaults,
- command sets,
- UI layout,
- policies,
- presentation.

They do not change:

- storage rules,
- data contracts,
- specifications.

---

## 10. Portable by design

AS 4_26 must be able to run:

- on different operating systems,
- on different architectures,
- inside other applications.

Portability is not a feature; it is a requirement.

---

## 11. No artificial platform lock-in

AS 4_26 is not tied to a single vendor, OS, or ecosystem.

Any platform dependency must be isolated and replaceable.

---

## 12. Auditability over opacity

System behavior should be observable and inspectable.

When possible:

- changes are traceable,
- states are explainable,
- actions can be reasoned about.

---

## 13. Simplicity beats cleverness

Clear rules and explicit behavior are preferred over clever abstractions.

If something is difficult to explain, it is probably wrong.

---

## 14. Growth without rewrite

The system must be able to grow in capability without forcing a full redesign.

Evolution is expected.
Rewrites are a failure mode.

---

## 15. Documentation is part of the system

Documentation is not optional and not external.

If it is not documented, it does not exist.
