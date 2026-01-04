# AfFS Failure Scenarios and Decision Rules

This document defines how AfFS behaves under failure conditions within the AS426 architecture.
The primary goals are:

* predictable behavior
* transparent degradation
* maximal preservation of canonical data
* operator-observable intent and outcomes

AfFS does not attempt to appear perfect. It attempts to remain trustworthy.

---

## 1. Principles

AfFS follows these principles under failure:

1. **Never silently discard canonical data**
2. **Prefer reconstruction over abandonment**
3. **Prefer validated structures over unvalidated**
4. **Degrade visibly, not silently**
5. **Repair only when safe and provable**

---

## 2. Failure Domains

AfFS distinguishes failure domains to avoid incorrect assumptions:

* media read failures (I/O errors)
* media write failures
* partial writes (torn records)
* corruption (checksum mismatch)
* divergence between mirrored copies (A/B disagreement)
* missing or invalid checkpoints
* index-log truncation

---

## 3. Startup Decision Tree (High Level)

On startup AfFS attempts:

1. Select the best copy (A or B) as primary read source
2. Locate the newest valid pinned checkpoint
3. Rebuild primary index up to the checkpoint’s referenced positions
4. Enter normal mode, degraded mode, or recovery mode

All decisions are logged and reportable.

---

## 4. Mirror Disagreement (Copy A vs Copy B)

### 4.1 Disagreement Types

* checkpoint mismatch
* index-log mismatch
* object segment corruption
* bad region map mismatch

### 4.2 Preferred Resolution Order

When A and B disagree, AfFS prefers the copy that:

1. passes checksum validation for the relevant structures
2. contains the newest valid pinned checkpoint
3. produces consistent reconstruction results

If neither copy validates, AfFS enters recovery mode.

### 4.3 Safe Repair Rules

If one copy is validated and the other is not:

* AfFS may repair the invalid copy by copying canonical records from the valid copy
* repair is performed append-only
* repair operations are recorded

Repair is never performed if the system cannot prove which copy is correct.

---

## 5. Torn Records and Partial Writes

### 5.1 Detection

Torn records are detected by:

* invalid magic/version
* length out of bounds
* checksum mismatch

### 5.2 Handling

* torn records are ignored
* scanning continues to the next valid boundary
* the presence of torn records triggers a warning report

---

## 6. Missing or Invalid Checkpoints

### 6.1 If Latest Pinned Checkpoint Is Invalid

* skip it
* attempt previous pinned checkpoints
* record the failure

### 6.2 If No Pinned Checkpoint Is Valid

* perform full object scan
* reconstruct best-effort primary index
* mark resulting state as **unpinned**
* require verification before pinning

---

## 7. Index-Log Truncation or Corruption

If index-log is truncated:

* replay up to last valid record
* rely on object scan to recover newer objects when necessary
* record divergence between index-log and object segments

Secondary indexes may be dropped and rebuilt at any time.

---

## 8. Progressive Media Failure

When media failure is progressive (increasing read errors, latency, retries):

* AfFS enters **protective mode**
* reduces write frequency where possible
* increases checkpoint pinning frequency
* recommends migration

Protective mode is explicit and observable.

---

## 9. Emergency Evacuation (Best-Effort Reconstruction)

If the source is actively failing, AfFS prioritizes:

1. copying reachable objects from the newest valid checkpoint
2. copying canonical logs
3. producing a migration report listing unrecoverable items

---

## 10. Operator Reports

AfFS produces reports that include:

* selected primary copy (A/B) and rationale
* checkpoint lineage and validation results
* counts of valid and invalid records
* bad region map summary
* performed repairs (if any)
* recommended next actions

---

## 11. Design Invariants

* AfFS never hides failure
* Repair requires a provably good source
* Reconstruction is always possible from object data
* Operators can always obtain a full decision report

---

End of document.
