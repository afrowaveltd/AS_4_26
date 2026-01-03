# Glossary

This glossary defines the shared language of AS 4_26.  
Short definitions first. Details live in Docs and Specs.

## AS 4_26

A userspace DbOS runtime that treats data as a first-class citizen and can be hosted inside other applications.

## Data Junction Runtime

A runtime system that acts as a junction point between data storage, structure, querying, and interaction.

## DbOS

“Database Operating System” as a concept: a system that behaves like an OS around data (queries, history, structure, policies), without being a kernel-level OS.
An informal or academic term sometimes used to describe AS 4_26.
Not used as the primary name in this project.

## Userspace engine

The core runtime runs in user space. The kernel is only used for I/O transport (files, block devices, network).

## Kernel as transport

The operating system kernel provides device access and I/O. AS 4_26 does not replace the kernel.

## Personality

A packaged configuration of behavior, UI defaults, policies, commands, and presentation.  
Personalities do not change the core contracts (Specs).

## AUV (Universal Value)

The universal value model used by AS 4_26 for storage, transport, and APIs.
Types include: Null, Bool, Int64, Float64, Char, String, Array, Object, Binary.

## AUV Spec

The formal contract defining AUV encoding, canonical rules, limits, and compatibility requirements.

## AfS (Afrowave Storage)

The storage layer used by AS 4_26. It can be backed by:

- files (file backend),
- raw partitions (block backend),
and potentially other transports later.

## AfFS

Legacy/alternate naming sometimes used for the same storage layer.  
In this repository we prefer **AfS (Afrowave Storage)**.

## Backend

A concrete implementation of storage over a specific transport (file, raw partition, etc.).

## CLI / TUI

Text-based user interfaces:

- CLI: command line interface
- TUI: terminal user interface (rich terminal app)

## Web UI (Kestrel)

A web user interface hosted by Kestrel, optionally including an embedded terminal connected to the runtime.

## Spec

A strict contract document. Specs describe what must be true across all implementations.
