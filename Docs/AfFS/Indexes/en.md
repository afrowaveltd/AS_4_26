# AfFS Indexes

This document defines the **indexing model of AfFS** within the AS426 architecture.
Indexes are treated as **first‑class system structures**, not as database optimizations.
They provide deterministic navigation from logical keys to physical object locations and are fully rebuildable from canonical data.

---

## 1. Purpose and Philosophy

In AfFS, an **index is a deterministic map from a key to a location pointer**.

Indexes exist to:

* enable reachability and navigation
* decouple logical identity from physical layout
* support crash recovery and rebuild
* provide stable lookup semantics across time

Indexes **do not define truth**. They only describe how to reach truth.
The canonical truth is always the **object log + checkpoints**.

---

## 2. Location Pointer

All indexes ultimately resolve to a **location pointer**, which describes where an object payload resides.

### 2.1 Pointer Structure

A location pointer is a fixed, self‑contained structure:

* `SegmentId` – logical segment identifier
* `Offset` – byte offset within the segment
* `Length` – payload length in bytes
* `Flags` – reserved (compression, encryption, future use)
* `ObjectId` (optional redundancy) – used for validation during rebuild

The pointer must be sufficient to locate the object **without consulting any other structure**.

---

## 3. Primary Index

### 3.1 Definition

The **primary index** maps:

```
ObjectId → LocationPointer
```

### 3.2 Properties

* Uniqueness: exactly one active location per ObjectId
* Append‑only updates: newer mappings override older ones
* No in‑place mutation

### 3.3 Semantics

* The last valid entry wins
* Stale entries are ignored during rebuild
* Validation is performed using object header checksum

The primary index is the **only authoritative source of object location**.

---

## 4. Secondary Indexes

Secondary indexes provide alternative lookup paths but never store physical locations directly.

### 4.1 Mapping Model

```
SecondaryKey → ObjectId(s)
```

### 4.2 Common Secondary Keys

* Tags
* Logical path
* Content hash
* Timestamp
* Size class (future)

### 4.3 Characteristics

* Non‑unique
* Multi‑value
* Fully discardable and rebuildable

Secondary indexes may become inconsistent and are always considered **derivable state**.

---

## 5. Index Storage Model

### 5.1 Index Log

All index mutations are written to an **append‑only index log**:

* primary index updates
* secondary index updates
* checkpoint markers

The index log is durable and replayable.

### 5.2 In‑Memory State

At runtime:

* index entries are loaded into memory tables
* newest entries shadow older ones

### 5.3 Sorted Runs (Optional)

For persistence and faster lookup:

* in‑memory tables may be flushed into sorted runs
* compaction is optional and deferred

This design follows an **LSM‑style philosophy** optimized for simplicity and rebuildability.

---

## 6. Rebuild Strategies

AfFS supports multiple rebuild modes depending on severity and intent.

### 6.1 Fast Rebuild

* Start from latest pinned checkpoint
* Replay index log forward
* Validate pointers lazily

### 6.2 Full Rebuild

* Scan all object segments sequentially
* Reconstruct primary index from object headers
* Rebuild secondary indexes from metadata

### 6.3 Verification Mode

* Random sampling of pointers
* Header and checksum validation
* Detection of silent corruption

Secondary indexes may be dropped and rebuilt without data loss.

---

## 7. Crash Safety and Atomicity

### 7.1 Atomic Units

* Object append (header + payload + checksum)
* Index log append

Partial writes are ignored during rebuild.

### 7.2 Checkpoints

A checkpoint pins:

* last valid segment offsets
* last durable index log position
* optional summary metadata

Checkpoints provide the **root of reachability**.

---

## 8. Hot / Cold Data Policy (Future Extension)

Segments may be classified by storage tier:

* hot (SSD)
* cold (HDD)

Location pointers remain unchanged; tier routing is external policy.

---

## 9. Minimal Index API

Indexes are required to support the following logical operations:

* `Resolve(ObjectId) → LocationPointer`
* `FindBySecondaryKey(key) → ObjectId[]`
* `RangeByTime(from, to) → ObjectId[]`

Query planning and optimization are explicitly out of scope.

---

## 10. Design Invariants

* Indexes are never canonical
* Primary index defines location, secondary indexes define reachability
* All indexes must be rebuildable from object data
* Simplicity is preferred over maximal performance

---

End of document.
