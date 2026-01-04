# AfFS Verification and Scrubbing

This document defines **verification and scrubbing mechanisms** in AfFS within the AS426 architecture.
These mechanisms provide long-term confidence that stored data remains readable, consistent, and recoverable.

Verification and scrubbing are not emergency tools; they are **routine maintenance** for trust.

---

## 1. Design Intent

Verification and scrubbing exist to:

* continuously validate data integrity
* detect silent corruption early
* repair damage when safe and provable
* guide operators before failure becomes critical

AfFS prefers **early visibility** over delayed surprise.

---

## 2. Verification vs Scrubbing

### 2.1 Verification

Verification answers the question:

> "Can the system still read and validate what it believes exists?"

Verification:

* reads data
* validates headers and checksums
* does not modify storage
* produces a report

### 2.2 Scrubbing

Scrubbing answers the question:

> "Can damaged data be safely repaired using redundancy?"

Scrubbing:

* reads and validates data
* may write repaired records
* is strictly controlled
* never guesses

---

## 3. Verification Scope

Verification may operate on different scopes:

* entire AfFS store
* selected segments
* reachable objects from a pinned checkpoint
* mirrored copy comparison (A vs B)

Scope is explicit and operator-selectable.

---

## 4. Verification Process

A typical verification pass:

1. Select source copy (A, B, or both)
2. Locate pinned checkpoint(s)
3. Traverse reachable objects
4. Validate object headers
5. Validate payload checksums
6. Record results

Verification tolerates unreadable regions and records them.

---

## 5. Scrubbing Eligibility Rules

Scrubbing is permitted only when:

* one copy validates and the other does not
* the validating copy is trusted (pinned checkpoint lineage)
* the repair operation is append-only

Scrubbing is never performed when:

* both copies are corrupted
* correctness cannot be proven

---

## 6. Scrubbing Process

When scrubbing is eligible:

1. Identify corrupted or unreadable records on one copy
2. Read the corresponding canonical records from the healthy copy
3. Append repaired records to the damaged copy
4. Update relevant metadata (bad region map if applicable)
5. Record the repair action

Scrubbing does not erase original corrupted data; it supersedes it.

---

## 7. Frequency and Scheduling

Verification and scrubbing may be:

* manual
* scheduled (e.g. daily, weekly)
* adaptive (triggered by error trends)

Adaptive scheduling prioritizes areas showing degradation.

---

## 8. Reporting and Observability

Every verification or scrub run produces a report containing:

* verified objects and segments
* detected corruption
* repaired records (if any)
* unrecoverable regions
* recommendations (e.g. migration, snap-to-disk)

Reports are preserved as part of system history.

---

## 9. Interaction with Other Mechanisms

Verification and scrubbing interact with:

* checkpoints (defining verification roots)
* redundancy (repair source selection)
* migration (trigger recommendation)
* snap-to-disk (post-snap verification)

These mechanisms reinforce each other.

---

## 10. Design Invariants

* Verification never mutates data
* Scrubbing never guesses
* All repairs are append-only
* All actions are observable and reportable

---

End of document.
