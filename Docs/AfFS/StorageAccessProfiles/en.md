# AfFS Storage Access Profiles

This document defines **storage access profiles** used by AfFS.
Access profiles describe *how data is accessed*, not *how queries are expressed*.
They exist to guide physical layout, segment design, caching, and I/O strategy.

AfFS is a database-oriented operating system storage.
Therefore, access patterns are known and intentional.

---

## 1. Purpose

Storage Access Profiles:

* formalize expected access patterns
* drive physical data layout decisions
* reduce fragmentation by design
* improve cache and prefetch efficiency
* prevent accidental performance regressions

They are architectural contracts, not optimizations.

---

## 2. Core Access Profiles

AfFS defines a small, closed set of core access profiles.
New profiles may be introduced only when a distinct physical behavior is required.

---

### 2.1 Point Lookup

**Intent:** Retrieve a single record by identifier.

Typical usage:

* primary key lookup
* object ID resolution
* metadata access

Characteristics:

* random access
* small read size
* latency-sensitive

Physical implications:

* dense packing of fixed or small records
* alignment for minimal I/O
* caching prioritized

---

### 2.2 Range Scan

**Intent:** Read a sequence of records ordered by a key.

Typical usage:

* time-ordered data
* index traversal
* ordered rows

Characteristics:

* sequential or near-sequential access
* predictable stride
* throughput-sensitive

Physical implications:

* sequential segment layout
* contiguous extents
* read-ahead friendly

---

### 2.3 Sequential Append

**Intent:** Continuously append new records.

Typical usage:

* logs
* event streams
* audit trails

Characteristics:

* write-heavy
* append-only
* minimal read locality

Physical implications:

* append-only segments
* minimal metadata mutation
* relaxed read optimization

---

### 2.4 Bulk Scan

**Intent:** Scan a large portion of data without filtering.

Typical usage:

* analytics
* verification
* migration

Characteristics:

* large sequential reads
* cache-thrashing risk

Physical implications:

* dedicated segments
* bypass or low-priority cache
* sequential I/O optimization

---

### 2.5 Streamed Large Object Access

**Intent:** Read or write large binary payloads.

Typical usage:

* BLOB access
* media streaming

Characteristics:

* large I/O sizes
* streaming behavior
* low random access

Physical implications:

* extent-based allocation
* minimal relocation
* streaming-friendly buffering

---

## 3. Relationship to Data Kinds and Size Classes

Access Profiles interact with Data Kinds and Size Classes:

* Scalar Records + Point Lookup → dense fixed segments
* Row Records + Range Scan → ordered row segments
* Log Data + Sequential Append → append-only segments
* BLOB + Streamed Access → large extent segments

This mapping is intentional and explicit.

---

## 4. Cache Strategy Implications

Access Profiles influence cache behavior:

* Point Lookup → high cache priority
* Range Scan → read-ahead buffers
* Sequential Append → write buffers
* Bulk Scan → cache bypass or eviction-resistant policy

Cache behavior must align with profile intent.

---

## 5. Fragmentation Avoidance

Fragmentation is avoided by:

* separating data by access profile
* allocating segments with uniform intent
* avoiding mixed workloads in a single segment

Compaction, when needed, is localized per profile.

---

## 6. Profile Stability

Access Profiles are stable over time.

Schema or data evolution does not change the access profile unless:

* the fundamental access pattern changes
* a new physical behavior is required

This prevents accidental layout churn.

---

## 7. Design Invariants

* Every segment declares its access profile
* Access profile determines physical behavior
* Profiles guide layout, not query semantics
* No implicit or guessed profiles are allowed

---

End of document.
