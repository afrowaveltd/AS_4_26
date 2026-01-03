# Commands

Commands are how humans and tools talk to the AS 4_26 runtime.

This document defines the command model conceptually.
It is not a wire-format specification.

---

## Goals

The command system must be:

* easy to type and remember,
* consistent across Web UI, embedded terminal, CLI, and TUI,
* scriptable and automatable,
* safe by default (especially for destructive actions),
* fully observable (structured outputs, clear error semantics).

---

## Command vs Query

AS 4_26 uses two primary interaction categories:

* **Query**: reads state, does not change state.
* **Command**: may change state.

In practice:

* a query returns information,
* a command performs an action and returns a structured result.

Both are expressed using AUV values.

---

## Case-insensitive control language

The control language (keywords and built-in command names) is **case-insensitive**.

Examples:

* `SELECT`, `select`, and `sElect` are treated the same.
* `HELP`, `help`, and `HeLp` are treated the same.

This rule applies to:

* command keywords,
* built-in command names,
* reserved words.

Everything else is case-sensitive unless explicitly defined otherwise.

---

## Names: human vs normalized

AS 4_26 distinguishes between:

* **Human names**: what users type and see (descriptive, friendly).
* **Normalized names**: what the system uses internally (stable and predictable).

Normalized names exist to:

* avoid ambiguity,
* support consistent indexing,
* allow multiple human representations for the same entity.

The exact normalization rules are defined by the relevant subsystem (for example storage).

---

## Identity: stable internal IDs

Every important entity should have a stable internal identifier.

Examples:

* files and directories,
* stored objects,
* records,
* volumes,
* users and roles,
* background jobs.

### Why IDs matter

IDs allow the system to:

* refer to entities unambiguously,
* rename things without breaking references,
* optimize lookup,
* store metadata efficiently.

### Recommended approach

Use an ID size that does not create a future ceiling.

* **128-bit IDs** are recommended for long-term uniqueness and scalability.
* IDs should be treated as opaque identifiers.

IDs may be represented as:

* hex,
* base32,
* or another human-safe encoding.

The system should allow referencing entities by:

* human name,
* normalized name,
* or ID.

---

## Metadata flags (type, permissions, attributes)

Many systems pack metadata into bitfields.
This is useful, but must be used carefully.

### Principles

* Do not bake policies into IDs.
* Keep IDs stable and opaque.
* Store metadata explicitly, versioned, and extensible.

### What can be flagged

It is valid to have compact flags for:

* entity kind (file, directory, object, link),
* permissions (Unix-like rwx model or equivalent),
* attributes (hidden, warning-protected, app-protected).

These flags should live in:

* a header record,
* or a metadata record,

not inside the identifier itself.

This keeps:

* IDs future-proof,
* metadata evolvable,
* and recovery logic simpler.

---

## Input and output model

Every command produces structured output.

* outputs are AUV values,
* errors are structured AUV values,
* text rendering is a presentation layer.

This allows:

* machine-readable output for scripts,
* rich UI rendering in Web/TUI,
* consistent logging and auditing.

---

## Safety model

Destructive commands must be safe by default.

Safety mechanisms may include:

* confirmations,
* dry-run modes,
* explicit force flags,
* policy checks,
* role-based restrictions.

A personality may tighten safety rules.
A personality must not bypass core safety rules.

---

## Idempotence and retry

The runtime must expect:

* retries,
* duplicate submissions,
* interrupted sessions.

Where possible:

* commands should be idempotent,
* operations should be resumable,
* results should include enough information to continue safely.

This supports:

* robust automation,
* crash-safe workflows,
* predictable recovery.

---

## Sessions and context

Commands may run:

* statelessly (one command at a time),
* or within a session (a continuous terminal connection).

Sessions may provide:

* authentication context,
* current working context (like a directory or namespace),
* active personality and policy context.

Sessions must not change the meaning of commands.
They only provide convenience and context.

---

## Relationship to specifications

This document defines the command model conceptually.

Formal details will be defined by specifications such as:

* AUV specification,
* command grammar and parsing rules,
* transport protocol rules.

---

## Summary

The command system is the universal control surface of AS 4_26.

It is:

* case-insensitive for keywords,
* consistent across all interfaces,
* AUV-based for input and output,
* safe by default,
* designed for retry and recovery.
