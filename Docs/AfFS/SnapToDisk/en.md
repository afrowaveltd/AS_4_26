# AfFS Snap-to-Disk

This document defines **Snap-to-Disk**, an AfFS capability to produce an immediate, recoverable backup state onto external or secondary storage.
Because AfFS uses specialized append-only structures, traditional backup tools may not provide the desired guarantees.
Snap-to-Disk is therefore treated as both a necessity and a core feature.

---

## 1. Design Intent

Snap-to-Disk exists to:

* produce a recoverable AfFS state on demand
* enable periodic automated backups without external tooling assumptions
* preserve trust anchors (pinned checkpoints) together with canonical logs
* provide predictable, operator-observable results

Snap-to-Disk does not attempt to be a generic backup system.
It preserves AfFS.

---

## 2. Snapshot Unit: A Pinned Checkpoint

The fundamental unit of a snap is a **pinned checkpoint**.

A Snap-to-Disk operation:

1. ensures a new checkpoint is created
2. pins it
3. copies the canonical structures required to reconstruct from that checkpoint

This provides a clear definition of what the snapshot means.

---

## 3. Snap Targets

A snap may be written to:

* removable external media
* permanently attached secondary disk
* network-attached storage (future)

A snap target is treated as an append-only destination to reduce risk.

---

## 4. Snap Modes

### 4.1 Canonical Snap (Recommended)

Copies:

* pinned checkpoint record(s)
* index log (at least up to the checkpoint)
* object segments containing reachable objects (or segment ranges)
* bad region map (if present)

This mode preserves a recoverable AfFS state and is resilient to partial destination failures.

### 4.2 Image Snap (Physical)

Creates a physical clone (or image) of the AfFS storage.

This mode:

* preserves layout
* is slower
* may fail on read errors

It is similar to bitwise migration and primarily intended for forensic or archival purposes.

---

## 5. Consistency Guarantees

Snap-to-Disk aims for predictable consistency:

* the snap is defined by the pinned checkpoint
* copied records are checksum-verified during transfer
* the snap includes an operator-readable report

AfFS does not claim that external media is reliable.
It produces verification evidence.

---

## 6. Scheduling and Retention

Snap-to-Disk may be:

* manual (operator initiated)
* scheduled (interval-based)

Retention policies may include:

* keep last N snaps
* keep daily/weekly/monthly pins
* keep only verified snaps

Retention must never silently delete the last pinned recovery root.

---

## 7. Verification and Restore

### 7.1 Verification

A snap is verified by:

* validating checkpoint checksums
* replaying index-log up to the checkpoint
* sampling or fully validating reachable objects

Verification results are recorded.

### 7.2 Restore

Restore is performed by:

* selecting a snap checkpoint
* reconstructing AfFS state from copied canonical logs
* optionally rebuilding secondary indexes

Restore never depends on the original source media.

---

## 8. Observability

Snap-to-Disk produces:

* progress updates (objects/bytes)
* error accounting (skipped/unreadable)
* a final summary report

Trust is strengthened by transparent outcomes.

---

## 9. Design Invariants

* A snap is defined by a pinned checkpoint
* Canonical logs are preserved first
* Checksums govern validation and reporting
* Restore is always possible from the snap without the original media

---

End of document.
