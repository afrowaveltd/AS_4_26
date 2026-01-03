# Storage

Storage in AS 4_26 is built around one non-negotiable rule:

**data must not suffer.**

Performance matters, but safety comes first.
Anything that risks corruption, loss, or ambiguity is treated as a critical defect.

---

## What “Storage” means in AS 4_26

Storage is the full persistence stack:

* the runtime’s storage lifecycle (open/close/flush/recover),
* AfFS (Afrowave Flexible Storage) as the userspace storage engine,
* backends that provide byte and block transport (file, partition, disk),
* integrity, recovery, and rollback guarantees.

Storage is not a single file format.
It is a coordinated system that stays correct even when reality is rough.

---

## Design goals

Storage must:

1. remain consistent under interruptions,
2. detect corruption early,
3. recover deterministically,
4. support rollback as a first-class operation,
5. be portable across platforms and backends,
6. adapt to the physical characteristics of the underlying medium.

---

## Backends: where bytes come from

A backend is an I/O transport provider.

Typical backends:

* **file backend**: storage-in-a-file (fast start, easy testing, portable),
* **partition backend**: raw block device (serious deployments),
* **disk backend**: whole drive (specialized use cases).

Backends provide:

* read blocks,
* write blocks,
* flush barriers,
* error reporting.

Backends do not interpret meaning.
They move bytes.

---

## AfFS: how bytes become durable structure

AfFS is the persistence engine that builds structure over backend blocks.

AfFS:

* defines layout rules,
* stores metadata explicitly,
* publishes changes only when safe,
* recovers after failure,
* supports rollback.

The runtime interacts with AfFS; AfFS interacts with the backend.

---

## Hardware-aware efficiency (without sacrificing safety)

Storage performance depends heavily on how well we align to the medium.

Where possible, AfFS will choose block sizes as multiples of the underlying device’s preferred I/O sizes:

* sector size and alignment,
* physical vs logical block size,
* typical optimal transfer sizes.

This allows:

* fewer read-modify-write cycles,
* better throughput,
* reduced wear on SSDs,
* more predictable performance.

This hardware-awareness must remain a **compatibility-safe optimization**:

* correctness never depends on it,
* layout remains portable,
* the system can fall back to a safe default.

---

## Rollback is a must-have

Rollback is not optional.

AS 4_26 assumes that:

* crashes happen,
* power loss happens,
* partial writes happen,
* human mistakes happen,
* bugs are discovered later.

Therefore the storage system must support rollback as a core safety tool.

Rollback provides:

* recovery from incomplete operations,
* safe interruption handling,
* a strategy for dealing with newly discovered defects,
* confidence during development and migrations.

Rollback must be designed into the commit model, not bolted on.

---

## Commit model and durability

Storage changes are applied using a publish model:

1. write new data and metadata,
2. validate,
3. publish a new root/commit record,
4. only then reclaim old structures.

A change is considered complete only after the publish step is safely persisted.

This model enables:

* crash safety,
* deterministic recovery,
* rollback to the last published good state.

---

## Recovery: boring by design

On startup (or mount), storage recovery must:

* find the last published consistent state,
* validate it,
* roll back incomplete changes,
* optionally reclaim leaked space,
* fail safely if corruption is detected.

Recovery should be:

* deterministic,
* repeatable,
* and explainable.

If recovery feels mysterious, it is a bug.

---

## Integrity and validation

Storage must detect corruption.

Common strategies include:

* checksums for metadata and/or data,
* version tags and magic values,
* redundancy for critical records.

The exact mechanisms are specified elsewhere.
The requirement is simple:

* corruption must be detectable,
* and the system must fail safely.

---

## Backup and recovery strategy

Backup is part of storage design, not an afterthought.

AS 4_26 should support backups that are:

* verifiable,
* restorable,
* and independent of the runtime implementation.

Possible backup targets may include unusual media (for example optical discs) as long as:

* integrity can be verified,
* restoration is deterministic,
* and the process is documented.

Backup and restore flows will be defined as a separate part of the documentation.

---

## Relationship to other documents

* AfFS architecture: `Docs/AfFS/Architecture/en.md`
* AfFS disk layout (conceptual): `Docs/AfFS/DiskLayout/en.md`
* Principles: `Docs/Principles/en.md`

Formal on-disk definitions will live in:

* `Specs/AfFS.DiskLayout.md`

---

## Summary

Storage in AS 4_26 is built to protect data first.

It combines:

* backend transports,
* AfFS as a userspace persistence engine,
* hardware-aware efficiency,
* deterministic recovery,
* and mandatory rollback.

Performance is welcome.
But safety is law.
