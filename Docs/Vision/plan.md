# AfFS v1 Implementation Roadmap

> **Note:** This document is part of the *Vision / Plan* layer of the AS426 project.
> It is intended to live under `Docs/Vision/plan.md` (or equivalent) as the authoritative implementation plan.
> This copy exists to keep the roadmap versioned alongside AfFS technical documentation.

This document is the **development map** for implementing AfFS v1 within the AS426 architecture.
It complements the design documentation by defining:

* milestone order (bottom → top)
* concrete deliverables per milestone
* acceptance criteria (what “done” means)
* the rule for updating docs as implementation progresses

The goal is to make implementation predictable, reviewable, and easy to resume across threads.

---

## 0. Guiding Rules

### 0.1 Bottom → Top

Implementation proceeds from the smallest stable primitives to larger systems.
No higher layer may be implemented until the layer below:

* has tests
* has clear contracts
* produces observable outputs

### 0.2 “Truth First” Tools

Early in the roadmap we build tools that can:

* scan
* validate
* report

These tools prevent guessing and speed up debugging.

### 0.3 Docs Stay Live

Whenever a milestone is completed:

* update the corresponding design document(s)
* add a short “Implementation Status” note (what exists, what is pending)
* record any deviations discovered during coding

---

## 1. Milestones Overview

Milestones are designed to produce value early and reduce risk.

* M0: Repo skeleton and test harness
* M1: Backend I/O Layer (File backend first)
* M2: Binary primitives (CRC32C, LE codecs, alignment)
* M3: Record framing (AFR1) + scan/report
* M4: Segment header (AFSG) + segment writer
* M5: Superblock (AFSB) + root pointer record
* M6: Minimal checkpoint record + pin selection
* M7: Verification report tool (CLI)

Later milestones (indexes, allocators, runtime integration) come after AfFS v1 is bootable and verifiable.

---

## 2. Milestone Details

### M0: Repo Skeleton and Test Harness

**Goal:** Enable test-first development from day one.

Deliverables:

* solution structure
* core projects (names follow repository conventions)
* test project(s) with consistent folder structure
* CI-ready test runner setup (local first)

Acceptance criteria:

* tests run in one command
* empty smoke test passes

Docs to touch:

* add this roadmap document

---

### M1: Backend I/O Layer (Platform Boundary)

**Goal:** Deterministic byte access with structured results.

Deliverables:

* backend interface contract
* structured result model (`Ok`, `Partial`, `TransientError`, `PermanentError`, `Timeout`, `AccessDenied`, `OutOfSpace`)
* `FileBackend` implementation
* flush/durability call-out
* basic metrics hooks (optional)

Acceptance criteria:

* read/write at explicit offsets works
* partial and failure outcomes are representable
* flush call is exposed and errors propagate
* unit tests cover:

  * roundtrip
  * bounds
  * partial simulation (test double)
  * error mapping (where possible)

Docs to touch:

* `Docs/AfFS/Backend-IOLayer/en.md` (status + any contract refinements)

---

### M2: Binary Primitives

**Goal:** One consistent binary language.

Deliverables:

* CRC32C implementation (or vetted dependency)
* little-endian encode/decode helpers
* alignment/padding helpers

Acceptance criteria:

* checksum passes known vectors
* encode/decode roundtrips
* padding is deterministic and zero-filled

Docs to touch:

* `Docs/AfFS/Binary-Layout-v1/en.md` (checksum and alignment notes if adjusted)

---

### M3: Record Framing (AFR1) + Scan/Report

**Goal:** “AfFS Hello World”: write records, read records, scan forward.

Deliverables:

* `RecordHeader` encode/decode
* record writer (append at offset)
* scanner that:

  * detects valid headers
  * validates checksum
  * skips invalid/torn records
  * continues scanning
* scan report model (counts, invalid reasons)

Acceptance criteria:

* valid record read-back works
* torn record is skipped and scanning continues
* checksum mismatch is detected
* scan report is deterministic

Docs to touch:

* `Docs/AfFS/Binary-Layout-v1/en.md` (framing clarifications if needed)

---

### M4: Segment Header (AFSG) + Segment Writer

**Goal:** Enforce intent and reduce fragmentation by design.

Deliverables:

* `SegmentHeader` encode/decode
* segment creation
* segment append enforcing:

  * uniform access profile
  * compatible size class
* segment scan support

Acceptance criteria:

* segment header validates
* mixed profile writes are rejected
* segment scan enumerates records consistently

Docs to touch:

* `Docs/AfFS/Binary-Layout-v1/en.md` (segment rules)

---

### M5: Superblock (AFSB) + Root Pointer Record

**Goal:** Bootable store that knows where “current world” is.

Deliverables:

* superblock encode/decode
* store UUID
* root pointer record payload encode/decode
* store init/open functions

Acceptance criteria:

* init → reopen loads superblock
* root pointer record is readable and validates
* corruption of superblock is detectable

Docs to touch:

* `Docs/AfFS/Binary-Layout-v1/en.md` (superblock/root pointer)
* `Docs/AfFS/DiskLayout/en.md` (status notes)

---

### M6: Minimal Checkpoint Record + Pin Selection

**Goal:** Trust anchor exists in code.

Deliverables:

* minimal checkpoint record format (v1)
* pin metadata
* algorithm: select newest valid pinned checkpoint

Acceptance criteria:

* multiple checkpoints with one corrupted → selects previous valid
* lineage rules are testable

Docs to touch:

* `Docs/AfFS/Checkpoints/en.md` (status + field adjustments)

---

### M7: Verification Report Tool (CLI)

**Goal:** Visible truth for operators and developers.

Deliverables:

* `verify` command:

  * scan store
  * validate records
  * validate superblock and root pointer
  * report counts and failures
* output:

  * human-readable summary
  * machine-readable JSON

Acceptance criteria:

* tool can verify a fresh store
* tool flags injected corruption
* output is stable and parseable

Docs to touch:

* `Docs/AfFS/Verification-and-Scrubbing/en.md` (verification part status)

---

## 3. Later Milestones (Post v1 Bootability)

After M7, AfFS is bootable and verifiable.
Then we proceed to:

* allocator and A/B separation policy implementation
* commit/checkpoint binding refinements
* index-log framing and rebuild
* query-oriented runtime integration
* scrubbing (repair) implementation
* scheduled snap-to-disk

These are intentionally deferred until the base truth pipeline is stable.

---

## 4. Status Tracking

Each milestone should be tracked with:

* Status: Planned / In Progress / Done
* Date completed
* Links to tests or tool commands used
* Any deviations from docs

---

End of document.
