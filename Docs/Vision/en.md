# Vision

## AS 4_26 in one sentence

AS 4_26 is the wisdom of the past in a system of the future: a userspace DbOS runtime you can talk to like a shell, and trust like a storage engine.

## What it is

AS 4_26 is a **data-first runtime** that feels like an operating system for knowledge and structured information.

You can interact with it in multiple ways:

- write commands like in **Bash** or **command.com**,
- use a **web UI** (with an embedded terminal),
- or ask it questions directly via APIs.

It is designed to be hostable inside other applications and portable across platforms.

## What it is NOT

- Not a new kernel.
- Not something that needs special drivers.
- Not a platform-locked product.
- Not a commercial lock-in system.

## The core idea

Most systems treat data as something that belongs “inside” applications.  
AS 4_26 treats data as something that belongs to the **runtime itself**, with consistent rules:

- universal values (AUV),
- predictable storage behavior (AfS),
- and multiple clients (web / CLI / TUI) acting as interchangeable interfaces.

## Why this matters

A stable data runtime can make software more consistent:

- same data behavior across platforms,
- the same “language” for storage and transport,
- and a system that can grow without rewriting everything.

## Storage philosophy (AfS)

AS 4_26 uses a userspace storage engine called **AfS (Afrowave Storage)**.

- The kernel provides I/O transport.
- AfS decides how data and files are structured and stored.
- AfS can run on top of a normal file (great for fast start and testing),
  or on a raw partition (for serious deployments).

## Personalities

AS 4_26 can be packaged as **personalities**.

A personality is a predefined “way the system behaves”:

- command sets and defaults,
- UI layout,
- policies,
- and presentation.

Personalities do not change the core rules in Specs.

## Implementations

- **.NET reference implementation**: fast start and validation.
- **BOS-compatible implementation**: the same concepts without .NET.

The goal is one shared truth: **Specs** define the contracts, implementations follow them.
