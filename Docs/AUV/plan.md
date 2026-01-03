# AUV – Plan

This document describes the intent, scope, and planning boundaries of AUV (Universal Values).
It explains what AUV is meant to solve and what the formal specification must define.

This is a planning document, not a specification.

---

## Why AUV exists

Most systems use different data representations at different layers:

- one format for storage,
- another for APIs,
- another for transport,
- another for scripting.

This fragmentation increases complexity, creates hidden conversions, and makes systems harder to reason about.

AUV exists to provide **one universal value model** used consistently across the entire AS 4_26 runtime.

---

## Core goals of AUV

AUV must:

- be simple enough to explain to non-programmers,
- be strict enough to serve as a system-wide contract,
- work equally well for storage, transport, APIs, and scripting,
- remain stable over time.

AUV is not optimized for minimal size or maximum speed.
It is optimized for **clarity, predictability, and universality**.

---

## Scope of AUV

AUV defines:

- a closed set of value types,
- how those values are represented conceptually,
- how they are serialized and transported,
- how equality and comparison are defined,
- how limits and safety boundaries are enforced.

AUV does not define:

- query languages,
- storage indexing strategies,
- execution models,
- UI representations.

---

## Value types (conceptual)

The AUV model consists of the following value types:

- Null  
- Bool  
- Int64  
- Float64  
- Char  
- String  
- Array  
- Object  
- Binary  

These types are intentionally minimal.
All higher-level concepts must be built on top of them.

---

## Canonical behavior

The specification must define canonical behavior for:

- numeric representation (including edge cases),
- string encoding,
- object key uniqueness and ordering,
- array ordering guarantees,
- binary data handling,
- null semantics.

Canonical rules exist to ensure that:

- the same data has the same meaning everywhere,
- different implementations behave the same way.

---

## Equality and comparison

The specification must define:

- how equality is evaluated for each type,
- how comparisons behave (where applicable),
- how special cases are handled (e.g. NaN, signed zero).

Ambiguous behavior is not acceptable.

---

## Serialization and wire format

The specification must define:

- a versioned wire format,
- clear type tagging,
- length boundaries,
- endianness rules,
- forward and backward compatibility rules.

The wire format must allow:

- streaming,
- partial decoding,
- validation without full materialization.

---

## Limits and safety

The specification must define explicit limits, including:

- maximum nesting depth,
- maximum size of values,
- maximum object key length,
- maximum array length.

Limits are part of the contract and exist for safety and predictability.

---

## Compatibility and evolution

AUV must be designed to evolve without breaking existing data.

The specification must define:

- how new versions are introduced,
- how older implementations behave when encountering newer data,
- which parts of the format are reserved for future use.

---

## Relationship to other parts of the system

AUV is a foundational layer.

It is used by:

- AfFS (Afrowave Flexible Storage),
- runtime APIs,
- CLI and scripting,
- inter-process and inter-system communication.

No part of the system may bypass AUV.

---

## What comes next

After this planning phase:

- `Docs/AUV/en.md` will explain AUV in human-friendly terms with examples,
- `Specs/AUV.Spec.md` will define the formal contract.

Once the specification is stable, implementations may begin.
