# AfFS Data Kinds and Size Classes

This document defines **what AfFS stores** and **how stored data is physically classified**.

AfFS is designed as a **database-oriented operating system storage**, inspired by AS/400 principles and adapted for 2026 realities.
It does not store arbitrary files first and interpret them later; it stores **known data kinds with predictable behavior**.

This classification is foundational for:

* physical layout
* fragmentation avoidance
* cache efficiency
* query performance
* long-term maintainability

---

## 1. Core Principle

> AfFS stores *typed data*, not generic bytes.

Every stored record declares:

* what kind of data it is
* how large it is expected to be
* how it is likely to be accessed

Unknown or opaque binary data is supported, but treated explicitly as such.

---

## 2. Data Kinds

A **Data Kind** describes the semantic role of a record.
It is orthogonal to size.

The initial set is intentionally small and extensible.

### 2.1 Structural Data

Purpose: system and database structure.

Examples:

* schema definitions
* table and index metadata
* configuration records
* internal catalogs

Characteristics:

* small
* fixed or near-fixed size
* read frequently
* updated infrequently

---

### 2.2 Scalar Records

Purpose: atomic values and fixed-width records.

Examples:

* numeric values
* timestamps
* GUIDs / IDs
* fixed-width rows

Characteristics:

* fixed size
* predictable alignment
* ideal for dense packing

---

### 2.3 Row Records

Purpose: logical database rows or documents.

Examples:

* table rows
* structured objects
* JSON-like records (typed)

Characteristics:

* bounded but variable size
* frequently accessed
* often subject to range scans

---

### 2.4 Textual Data

Purpose: human-readable content.

Examples:

* text fields
* logs
* messages

Characteristics:

* variable size
* moderate locality
* usually smaller than large binaries

---

### 2.5 Binary Large Objects (BLOB)

Purpose: opaque binary payloads.

Examples:

* images
* media files
* archives
* encrypted payloads

Characteristics:

* large
* opaque
* streamed access
* low locality with structured data

---

### 2.6 Log and Event Data

Purpose: append-only event streams.

Examples:

* event sourcing logs
* audit trails
* time-series events

Characteristics:

* append-only
* sequential access
* high write throughput

---

## 3. Size Classes

A **Size Class** determines physical storage strategy.
Size classes are chosen to minimize fragmentation and maximize locality.

### 3.1 Fixed

* exact, constant size
* packed densely
* no internal fragmentation

Typical uses:

* scalar records
* small structural entries

---

### 3.2 Small

* variable size up to a small upper bound
* slab or chunk allocator
* high density

Typical uses:

* small rows
* short text

---

### 3.3 Medium

* variable size
* extent-based allocation
* moderate locality

Typical uses:

* typical rows
* medium text

---

### 3.4 Large

* large payloads
* extent or streaming allocation
* minimal movement

Typical uses:

* BLOBs
* media
  n

---

## 4. Relationship Between Kind and Size

Data Kind and Size Class are independent but correlated.

Examples:

* Scalar Records → Fixed
* Structural Data → Fixed or Small
* Row Records → Small or Medium
* Textual Data → Small, Medium, or Large
* BLOB → Large

This separation allows:

* schema evolution
* storage optimization without semantic change

---

## 5. Implications for Physical Layout

Each Size Class is stored in **dedicated segments**.

Consequences:

* small records never fragment large data
* large data does not evict hot small records from cache
* compaction is localized per size class

Segments may additionally be grouped by Data Kind when beneficial.

---

## 6. Query-Oriented Optimization

Because Data Kind and Size Class are known:

* point lookups can target dense fixed segments
* range scans benefit from sequential row segments
* analytical scans can avoid BLOB segments entirely

Physical layout serves query patterns naturally.

---

## 7. Extensibility

New Data Kinds may be introduced by:

* assigning a new Kind identifier
* mapping it to an existing or new Size Class

Backward compatibility is preserved at the record level.

---

## 8. Design Invariants

* Every stored record declares a Data Kind
* Every record belongs to exactly one Size Class
* Size Classes define allocation strategy
* Data Kind defines semantic meaning
* Layout decisions are explicit and predictable

---

End of document.
