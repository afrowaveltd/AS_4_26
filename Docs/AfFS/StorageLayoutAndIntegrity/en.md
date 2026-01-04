# AfFS Storage Layout and Integrity Signals

This document complements AfFS redundancy and migration by defining:

* physical separation strategies for mirrored copies
* integrity signals (checksums) used for disagreement resolution
* handling of bad regions when underlying media cannot fully mask defects

This is intentionally an addendum-style document to keep other AfFS documents stable.

---

## 1. Physical Separation of Mirrored Copies

AfFS redundancy is strengthened when Copy A and Copy B are separated to reduce correlated failure.

### 1.1 Block Device or Dedicated Partition

When AfFS owns a block device (or a dedicated partition), the preferred mirrored layout is:

* **Copy A** grows from the beginning forward
* **Copy B** grows from the end backward
* a configurable **gap** is maintained between them

The gap acts as a safety buffer and can be reserved for emergency writes or kept unused.

### 1.2 File-Backed Stores

When AfFS is stored inside a file on a host filesystem, physical sector distance cannot be guaranteed.
Preferred approach:

* **two separate files** (A and B), ideally on different mount points

Optional constrained approach:

* **single-file split layout** (A from start, B from end) with a logical gap

Single-file separation primarily reduces software-correlated damage and does not guarantee resistance to localized media failure.

---

## 2. Integrity Signals (Checksums)

AfFS expects integrity validation for all binary structures it writes.
At minimum:

* object headers are self-validating (magic/version/length)
* object payloads carry checksums
* index-log records carry checksums
* checkpoint records carry checksums

Checksums are used to:

* detect corruption early
* choose a healthy source when Copy A and Copy B disagree
* enable safe automatic repair when one good copy exists

---

## 3. Disagreement Resolution Between Copies

When A and B disagree, AfFS follows a predictable rule set:

1. Prefer the copy that **passes checksum validation** for the accessed structure
2. If both pass, prefer the copy with a **newer valid pinned checkpoint**
3. If still ambiguous, prefer the copy that produces **consistent reconstruction results**

Disagreements are never silent:

* they are recorded
* they are reportable
* they can trigger degraded mode and/or migration recommendations

---

## 4. Media Defects and Bad Region Map

When the underlying device or filesystem cannot fully mask defects, AfFS may maintain a **Bad Region Map**.

### 4.1 Purpose

The bad region map exists to:

* avoid repeated access to unstable regions
* support best-effort reconstruction from failing media
* keep operator-visible accounting of unrecoverable areas

### 4.2 Representation

Bad regions may be recorded as:

* block ranges (for block devices)
* segment/offset/length ranges (for file-based or segmented stores)

The bad region map itself is:

* checksum-verified
* mirrored to both copies

### 4.3 Symmetric Sacrifice

To reduce divergence and preserve predictable mirroring behavior, AfFS may apply **symmetric sacrifice**:

* if a region is declared unusable on one copy,
* the corresponding logical region is also avoided on the other copy

This makes later verification and migration easier to reason about.

---

## 5. Deterministic Image Equivalence Concept

In a fully consistent state, reading mirrored stores should yield equivalent canonical content.
Conceptually:

* reading Copy A forward (start → end) and Copy B backward (end → start)
* should reconstruct the same canonical object stream, modulo layout differences

This document does not require bit-identical raw images across copies.
Instead, it targets **canonical equivalence**:

* the same reachable objects
* the same valid checkpoints
* the same integrity-verified records

---

End of document.
