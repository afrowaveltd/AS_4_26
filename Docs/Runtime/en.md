# Runtime

The runtime is the living core of AS 4_26.

It is the part of the system that understands data, executes rules, coordinates storage, and exposes behavior to all interfaces.
The runtime is not an application and not an operating system kernel.

It is a **userspace data runtime**.

---

## What the runtime is

The AS 4_26 runtime is a long-lived process that:

* owns the active data state,
* mediates all access to storage,
* executes commands and queries,
* enforces system rules and invariants,
* serves multiple clients at the same time.

All interaction with data goes through the runtime.
There is no "direct access" that bypasses it.

---

## Userspace-first design

The runtime runs entirely in userspace.

The operating system kernel is used only for:

* file access,
* block device access,
* memory management,
* networking,
* scheduling.

The runtime does not depend on kernel extensions, drivers, or privileged execution.

This design:

* improves portability,
* simplifies development and debugging,
* avoids platform lock-in,
* allows the runtime to be embedded inside other applications.

---

## Kernel as transport

From the runtime’s perspective, the kernel is an I/O transport layer.

The kernel provides:

* bytes in,
* bytes out,
* signals and events.

The kernel does not define:

* data structures,
* storage semantics,
* execution rules,
* system behavior.

All meaning lives in the runtime.

---

## Relationship to AfFS

AfFS (Afrowave Flexible Storage) is the persistent memory of the runtime.

The runtime:

* defines *what* is stored,
* defines *when* it is stored,
* defines *how* it is interpreted.

AfFS:

* defines *where* bytes live,
* defines *how* they are laid out,
* ensures persistence and integrity.

AfFS never makes decisions about data meaning.
The runtime never assumes a specific storage backend.

---

## One runtime, many interfaces

The runtime exposes its behavior through multiple interfaces:

* CLI (command-line interface),
* TUI (terminal user interface),
* Web UI (via Kestrel),
* APIs for programmatic access.

All interfaces are clients.

They:

* send commands,
* submit queries,
* receive results.

No interface contains hidden logic.
No interface has privileged access.

---

## Commands and queries

Interaction with the runtime is based on:

* commands: actions that may change system state,
* queries: requests for information.

Both commands and queries:

* are expressed using AUV values,
* are validated by the runtime,
* produce structured results.

This makes interaction:

* scriptable,
* inspectable,
* predictable.

---

## Talking to the system

The runtime is designed to be interacted with like a system, not like a library.

Examples of interaction styles include:

* shell-like commands,
* pipelines and scripting,
* interactive questioning.

The goal is to reduce the gap between:

* human intention,
* system execution.

---

## Long-lived by design

The runtime is designed to run continuously.

It:

* maintains in-memory state,
* coordinates background work,
* reacts to events,
* serves multiple users and tools.

Stopping and starting the runtime is a controlled operation.

---

## Deterministic core

The core logic of the runtime is deterministic.

Given the same:

* input commands,
* data state,
* configuration,

it produces the same results.

Sources of non-determinism (time, I/O latency, concurrency) are isolated from core logic.

---

## Personalities and the runtime

Personalities configure how the runtime behaves externally.

They may define:

* available commands,
* defaults and policies,
* UI presentation,
* access rules.

They do not change:

* data contracts,
* storage semantics,
* core execution rules.

The runtime remains the same.

---

## Embedding and hosting

Because the runtime is userspace-based, it can be:

* run as a standalone service,
* embedded into another application,
* hosted inside a larger system.

Hosting does not change runtime behavior.

---

## Relationship to specifications

This document explains the runtime conceptually.

Formal behavior is defined by:

* AUV specifications,
* AfFS architecture and layout specifications,
* command and protocol specifications.

Implementations must follow the specifications.

---

## Summary

The runtime is the heart of AS 4_26.

It connects:

* data (AUV),
* persistence (AfFS),
* interaction (CLI, TUI, Web, APIs).

Everything flows through it.
