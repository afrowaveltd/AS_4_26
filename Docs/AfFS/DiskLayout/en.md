# AfFS Disk Layout (Conceptual)

This document explains the AfFS disk layout in human terms.

It does **not** define byte offsets, exact headers, or binary structures.
Those belong in the formal specification.

The goal here is to explain how AfFS can remain consistent even when things go wrong:

* power loss,
* crashes,
* partial writes,
* unexpected restarts.

---

## The core promise

AfFS must never leave the storage volume in an ambiguous state.

After any interruption, the volume must be recoverable into a consistent state.

This is achieved by treating every structural change as a **transaction-like commit**:

1. write what is needed,
2. validate it,
3. publish it,
4. only then consider the change complete.

---

## Blocks

AfFS treats the underlying storage as a sequence of fixed-size blocks.

* A block is the smallest unit AfFS reads and writes.
* Blocks are addressed by a numeric index.

Using blocks makes AfFS portable across backends:

* a file backend can emulate blocks,
* a raw partition backend can map blocks directly.

---

## What lives on disk

Conceptually, AfFS stores two kinds of information:

* **Data blocks**: the content we want to keep (payload).
* **Metadata blocks**: the map that describes where everything is.

Data without metadata is a pile of bytes.
Metadata without data is an empty map.
AfFS keeps both, and keeps them consistent.

---

## The Superblock

The superblock is the entry point of the volume.

It answers questions like:

* “Is this an AfFS volume?”
* “Which version is it?”
* “Where is the current root of the metadata?”
* “What was the last known good state?”

To support recovery, AfFS may keep more than one superblock copy.

---

## The Root Pointer

AfFS needs a single, stable pointer that represents "the current world".

This pointer is the boundary between:

* the old consistent state,
* and the new consistent state.

When a change is committed, AfFS updates the root pointer.

If a crash happens before the root pointer is updated, the old state remains valid.
If a crash happens after the root pointer is updated, the new state is valid.

This is the foundation of safe commits.

---

## Allocation map

AfFS must know which blocks are:

* free,
* used,
* reserved.

This is managed by an allocation structure, such as:

* a bitmap,
* an extent table,
* or another explicit free-space map.

The exact structure is a specification detail.
The conceptual requirement is:

* allocation must be correct after recovery,
* leaked blocks must be detectable,
* double-allocation must be impossible.

---

## Objects, files, and directories

AfFS is flexible storage, but it still needs a navigable structure.

Conceptually:

* a **directory** is a container of named entries,
* a **file** is a named entity that points to stored content.

AfFS may store many kinds of payloads:

* Binary data,
* AUV-encoded structured data,
* or a combination.

User-facing naming rules (like hidden prefix `.` or warning prefix `!`) are policy.
The disk layout must store these names and attributes explicitly.

---

## Metadata is committed, not edited

To remain safe under interruption, AfFS avoids "editing important metadata in place".

Instead, it follows a simple rule:

* write new metadata blocks,
* verify them,
* then switch the root pointer to point to the new metadata.

This pattern supports:

* rollback,
* crash recovery,
* and deterministic reconstruction.

---

## Commit model (crash safety)

AfFS uses a commit model that ensures the volume is always recoverable.

A commit typically looks like this:

1. **Prepare**

   * allocate new blocks
   * write new data blocks
   * write new metadata blocks

2. **Validate**

   * verify checksums
   * verify structure consistency

3. **Publish**

   * update the root pointer (or commit record)

4. **Finalize**

   * mark old blocks as reclaimable
   * update free-space structures

If the system stops at any step, recovery can determine:

* what was fully published,
* what was incomplete,
* what must be rolled back or completed.

---

## Recovery

On startup (or mount), AfFS performs recovery.

Conceptually, recovery:

* finds the last published root pointer,
* validates the referenced metadata,
* rolls back any incomplete changes,
* optionally reclaims leaked blocks.

Recovery must be safe, repeatable, and deterministic.

---

## Checksums and integrity

AfFS stores integrity information to detect corruption.

Common mechanisms include:

* block checksums,
* metadata checksums,
* version tags,
* redundancy for critical records.

The exact algorithms are a specification choice.
The requirement is:

* corruption is detectable,
* and the system fails safely.

---

## Why this matters

A storage engine is judged by how it behaves when things go wrong.

AfFS is designed around the assumption that:

* crashes happen,
* power loss happens,
* partial writes happen,
* external failures happen.

Therefore:

* write quickly,
* publish only when safe,
* keep the volume consistent,
* make recovery boring.

---

## Relationship to the specification

This document explains the layout conceptually.

Exact structures live in:

* `Specs/AfFS.DiskLayout.md`

The specification will define:

* block sizes,
* headers,
* magic values,
* on-disk records,
* commit record format,
* and compatibility rules.

---

## Summary

AfFS disk layout is built around one idea:

**the storage must always be recoverable to a consistent state.**

It achieves this with:

* blocks,
* explicit metadata,
* a publish step (root pointer),
* and deterministic recovery.
