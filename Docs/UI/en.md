# UI

In AS 4_26, UI is not the system.

UI is a set of clients that interact with the runtime.
The runtime is the source of truth.

This separation makes the system:

* portable,
* consistent,
* scriptable,
* and testable.

---

## UI principles

All AS 4_26 user interfaces follow the same principles:

* UI is a client.
* No UI contains hidden logic.
* No UI has privileged access.
* The same commands and queries must work everywhere.

The UI exists to:

* present,
* assist,
* and control.

Not to redefine system behavior.

---

## UI surfaces

AS 4_26 provides multiple UI surfaces:

* **Web UI** (Kestrel hosted)
* **CLI** (command line interface)
* **TUI** (terminal user interface)

Each surface is optional, but together they cover:

* automation (CLI),
* interactive work (TUI),
* discoverability and administration (Web UI).

---

## Web UI (Kestrel)

The Web UI is the primary “administration cockpit”.

Goals:

* clear navigation and overview,
* search and inspection,
* configuration,
* monitoring,
* safe operations with confirmations.

The Web UI is hosted by Kestrel in the .NET reference implementation.

Other implementations may use other servers, but must expose the same runtime capabilities.

---

## Embedded terminal

The Web UI includes an embedded terminal.

This terminal is not a fake simulation.
It is a real client connected to the same command and query system as the CLI.

Benefits:

* one place to inspect and operate,
* two tools open at the same time (GUI + terminal),
* safe admin workflows with transparency.

---

## CLI

The CLI is the automation interface.

It must support:

* scripting,
* piping,
* machine-readable output,
* stable command behavior.

The CLI is expected to be:

* small,
* predictable,
* fast.

---

## TUI

The TUI is the interactive terminal application.

It focuses on:

* fast navigation,
* rich terminal UX,
* keyboard-driven workflows,
* operating the system remotely.

The TUI is the answer to:

* “I want a UI, but I’m on a terminal-only system.”

---

## One command model

All UI surfaces use the same command and query model.

* commands change state,
* queries read state.

Both are expressed using AUV values.

This ensures:

* consistency across interfaces,
* easy test automation,
* shared documentation,
* shared tooling.

---

## Human-first, developer-complete documentation

AS 4_26 UI documentation must:

* be understandable by non-programmers,
* provide everything developers need.

This includes:

* screenshots and examples (later),
* common workflows,
* explanations of safety behaviors,
* CLI examples that match UI operations.

---

## Security boundaries

UI clients must respect security boundaries.

The runtime decides:

* permissions,
* policies,
* authentication requirements.

UI surfaces only present and request.

---

## Summary

UI in AS 4_26 is a family of clients:

* Web UI for administration and overview,
* embedded terminal for power and transparency,
* CLI for automation,
* TUI for fast interactive terminal workflows.

All of them speak the same language to the runtime.
