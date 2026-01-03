# AS 4_26

**AS 4_26 is “the wisdom of the past in a system of the future.”**  
A data-first runtime that behaves like an operating system for knowledge, without being a traditional OS.

It is **not “just a database”**. It is a **living data system**: you can talk to it using a shell (Bash-like or `command.com`-like CLI), use a web UI with an embedded terminal, or ask it questions directly.

AS 4_26 is designed to be:

- **hostable** (runs in userspace, kernel I/O is only transport),
- **portable across platforms**,
- and **packaged as “personalities”** (behavior + UI + policies, without changing the core).

## Repository map

- **Docs/**  
  Human documentation (Afrowave style): understandable for non-programmers, complete for developers.  
  Each area has:
  - `plan.md` – what we want to build
  - `state.md` – what is true today
  - `en.md` – explanation & documentation

- **Specs/**  
  Hard contracts: formats, invariants, and rules that all implementations must follow.

- **src/**  
  Implementations and programs:
  - reference implementation in **.NET** (fast start),
  - BOS-compatible implementations (no .NET),
  - UI clients (web / CLI / TUI),
  - personalities and experiments.

- **Tools/**  
  Development tools (disk utilities, AUV helpers, dev scripts).

## Start here

1) Vision: `Docs/Vision/en.md`  
2) Principles: `Docs/Principles/en.md`  
3) Glossary: `Docs/Glossary/en.md`  
4) AUV overview: `Docs/AUV/en.md`  
5) AUV contract: `Specs/AUV.Spec.md`

## Non-goals (important)

- This is **not** a new kernel and does **not** require special drivers.
- This is **not** a platform-limited product.
- This is **not** a commercial lock-in product.