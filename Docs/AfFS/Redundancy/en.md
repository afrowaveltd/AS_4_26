# AfFS Redundancy and Self-Preservation

This document defines **redundancy, recoverability, and self-preservation mechanisms** of AfFS within the AS426 architecture.
The goal is not merely availability, but **maximal survivability of data and structure**, even under partial or progressive media failure.

---

## 1. Design Intent

AfFS is designed as a system that:

* tolerates partial media failure
* preserves canonical data through redundancy
* can reconstruct itself from raw storage
* supports controlled migration away from failing media

Redundancy is treated as a **first-class system concern**, not an optional deployment detail.

---

## 2. Canonical Data and What Must Be Protected

The following structures are considered **canonical and irreplaceable**:

* object segments (append-only object log)
* index log (append-only)
* pinned checkpoints (roots of reachability)

All other structures (in-memory tables, secondary indexes) are **derivable** and may be discarded.

---

## 3. Mirrored Storage Model (A/B Copies)

### 3.1 Concept

AfFS may operate with **two equivalent physical copies** of the same logical store:

* Copy A
* Copy B

Both copies contain the same logical AfFS structures but may reside on different physical devices.

### 3.2 Write Policy

All writes are issued to both copies according to a selected durability policy:

* **STRICT**: write must succeed on both A and B
* **DEGRADED_OK**: write succeeds if at least one copy accepts it

The active policy is part of system configuration.

---

## 4. Degraded Mode

If one copy becomes unavailable or unreliable:

* the system enters **degraded mode**
* all canonical data continues to be written to the remaining healthy copy
* the degraded copy is marked read-only or offline

Degraded mode is explicit and observable.

---

## 5. Integrity and Validation

Each copy maintains its own physical layout but must satisfy:

* valid object headers
* checksum-verified payloads
* consistent checkpoints

On startup or on demand, AfFS may:

* compare checkpoints between copies
* validate object headers by sampling or full scan
* select the healthiest copy as primary source

---

## 6. Self-Reconstruction Principle

AfFS must be able to reconstruct its logical state by **sequential reading of storage**, without relying on:

* index tables
* allocation maps
* external metadata

Reconstruction relies on:

* object headers
* checksums
* segment ordering
* pinned checkpoints

Stale or unreachable objects are tolerated during reconstruction.

---

## 7. Handling Stale and Unreachable Data

Due to append-only operation:

* obsolete object versions may exist
* superseded index entries may remain
* unreachable data may accumulate

This is intentional.

Reachability is defined as:

> Data reachable from the selected pinned checkpoint.

All other data is considered **garbage**, but not immediately deleted.

---

## 8. Migration as a Recovery Tool

Redundancy is complemented by **explicit migration mechanisms** (defined in a separate document).

Redundancy guarantees survival of canonical logs; migration enables:

* escape from degrading hardware
* consolidation or relocation
* controlled cleanup

---

## 9. Design Invariants

* Canonical data is always written append-only
* Redundancy protects logs, not derived state
* Any copy may be discarded if another remains valid
* The system prefers reconstructability over perfect cleanliness

---

End of document.
