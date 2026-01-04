# AfFS Migration and Drive Preservation

This document defines **migration, evacuation, and preservation mechanisms** of AfFS within the AS426 architecture.
Migration is treated as a **normal, first-class system capability**, not an emergency workaround.

The intent is to ensure that AfFS behaves as a **predictable, trustworthy system** when storage degrades, changes, or must be replaced.

---

## 1. Design Intent

AfFS migration exists to:

* preserve canonical data under degrading conditions
* allow relocation without loss of logical state
* provide deterministic and observable outcomes
* express system intent clearly to the operator

Migration is a manifestation of **system self-preservation**, not failure.

---

## 2. Canonical Scope of Migration

Migration concerns **everything that belongs to AfFS**, regardless of current usefulness.

This includes:

* object segments (canonical log)
* index log
* pinned checkpoints
* unreachable or stale objects

Migration does **not** attempt to interpret or optimize meaning.
Its purpose is faithful preservation.

---

## 3. Migration Modes

AfFS defines two explicit migration modes.
The mode is selected by the operator and always recorded.

---

### 3.1 Reconstructive Migration (Logical Rebuild)

#### Purpose

Designed for **maximum survivability** when the source medium is unreliable.

#### Characteristics

* sequential read-only scan of source storage
* tolerant to read errors and missing regions
* rebuilds logical state on target
* does not preserve physical layout

#### Procedure

1. Open source storage in best-effort read mode
2. Locate latest valid pinned checkpoint
3. Scan object segments sequentially
4. Copy only objects with valid headers and checksums
5. Write objects into new segments on target
6. Rebuild primary index from copied objects
7. Reconstruct secondary indexes if required
8. Create new pinned checkpoint on target

#### Properties

* unreachable or corrupt objects are skipped
* stale data may be omitted naturally
* target layout is compact and clean

This mode prioritizes **data meaning over physical fidelity**.

---

### 3.2 Bitwise Migration (Physical Clone)

#### Purpose

Designed for **maximum fidelity** when the source medium is readable.

#### Characteristics

* block-level or sector-level copy
* preserves physical layout exactly
* includes unreachable and stale data
* sensitive to read failures

#### Procedure

1. Copy source medium block-by-block
2. Detect and record unreadable regions
3. Optionally retry or substitute zero-filled blocks
4. Produce an identical target image

#### Properties

* exact physical replica
* supports forensic analysis
* preserves historical artifacts

This mode prioritizes **physical truth over usability**.

---

## 4. Hybrid Migration Strategy

AfFS may support a hybrid strategy:

1. Perform reconstructive migration to preserve live logical state
2. Optionally perform bitwise copy of remaining readable regions
3. Attach residual data as archival artifact

This strategy maximizes recovery while retaining optional fidelity.

---

## 5. Trigger Conditions

Migration may be initiated:

* manually by an operator
* automatically due to health degradation
* as part of planned hardware replacement

Health signals may include:

* increasing read error rates
* repeated retry patterns
* excessive latency
* explicit media failure indicators

---

## 6. Observability and Trust

Migration must be:

* explicit (no silent relocation)
* observable (progress and outcomes visible)
* verifiable (checksums, object counts, reports)

Operators must be able to distinguish:

* data preserved
* data skipped
* data unrecoverable

Trust arises from **clear intent and transparent results**.

---

## 7. Post-Migration Verification

After migration, AfFS may perform:

* full or sampled object verification
* index rebuild and consistency checks
* checkpoint validation

Verification results are recorded as part of migration metadata.

---

## 8. Design Invariants

* Migration never mutates source data
* Canonical logs are always preserved first
* Reconstruction is preferred over abandonment
* Physical fidelity is optional, not assumed

---

End of document.
