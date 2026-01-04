# AfFS Checkpoints

This document defines the **checkpoint mechanism** of AfFS within the AS426 architecture.
A checkpoint is not an optimization primitive; it is a **trust anchor** that binds data, indexes, and recovery into a single observable state.

---

## 1. Purpose and Role

A checkpoint represents a moment in time where AfFS declares:

> "Up to this point, the system state is coherent, reachable, and verifiable."

Checkpoints serve as:

* roots of reachability
* roots of trust
* recovery anchors
* reference points for redundancy decisions
* boundaries between live data and garbage

---

## 2. What a Checkpoint Is (and Is Not)

### 2.1 Is

A checkpoint is:

* an immutable record
* append-only
* explicitly created
* checksum-verified
* mirrored

It references canonical structures without duplicating them.

### 2.2 Is Not

A checkpoint is **not**:

* a snapshot of payload data
* a copy of indexes
* a transactional lock
* a performance cache

---

## 3. Checkpoint Contents

A checkpoint record minimally contains:

* `CheckpointId`
* `Timestamp`
* `PrimaryIndexLogPosition`
* `SegmentPositions` (highest known valid offsets)
* `PreviousCheckpointId`
* `Checksum`

Optional extensions may include:

* summary statistics (object count, segment count)
* health indicators
* migration markers

---

## 4. Pinned Checkpoints

### 4.1 Definition

A **pinned checkpoint** is explicitly marked as a trusted recovery root.

Pinned checkpoints:

* are never automatically discarded
* define the minimum guaranteed recoverable state
* are referenced during rebuild, verification, and migration

### 4.2 Pinning Semantics

Pinning is:

* explicit
* operator-visible
* durable

Automatic pinning may occur during:

* clean shutdown
* successful migration
* verified recovery

---

## 5. Reachability Model

Reachability is defined as:

> Any object reachable by following index references starting from a selected pinned checkpoint.

Consequences:

* objects not reachable are considered garbage
* garbage is tolerated and preserved until explicitly cleaned
* different pinned checkpoints may yield different reachable sets

---

## 6. Checkpoints and Redundancy

In mirrored configurations:

* each copy maintains its own checkpoint records
* checkpoints are compared by:

  * checksum validity
  * checkpoint lineage
  * referenced log positions

When copies disagree:

* the copy with the newest valid pinned checkpoint is preferred
* disagreement is reported

---

## 7. Checkpoints and Reconstruction

During reconstruction:

1. Locate the latest valid pinned checkpoint
2. Validate its checksum
3. Rebuild primary index up to referenced log position
4. Reconstruct reachability from this root

If no pinned checkpoint is valid:

* fallback to full object scan
* rebuild best-effort state
* mark result as unpinned until verified

---

## 8. Checkpoints and Migration

Migration relies on checkpoints to:

* define migration starting point
* verify completeness on target
* mark successful completion

A migration that completes successfully **creates a new pinned checkpoint** on the target.

---

## 9. Failure Transparency

Checkpoints never mask failure.

If a checkpoint cannot be validated:

* it is skipped
* the failure is recorded
* reconstruction proceeds from an earlier checkpoint or full scan

Trust is maintained by **visible degradation**, not silent repair.

---

## 10. Design Invariants

* Checkpoints are immutable
* Checkpoints are append-only
* Pinned checkpoints define trust boundaries
* Reconstruction always prefers pinned checkpoints
* Garbage is a tolerated state, not an error

---

End of document.
