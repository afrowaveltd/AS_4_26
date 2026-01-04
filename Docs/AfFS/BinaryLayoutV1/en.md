# AfFS Binary Layout v1

This document defines the **binary on-disk layout** of AfFS version 1.
It is a normative, byte-level specification.

AfFS is a database-oriented operating system storage.
The binary layout reflects **typed data, known access patterns, and deterministic recovery**.

---

## 1. Scope and Goals

Binary Layout v1 defines:

* record framing
* segment structure
* superblock format
* root pointer record
* checksum and alignment rules

It does **not** define:

* query semantics
* index algorithms
* higher-level runtime behavior

---

## 2. Endianness and Encoding

* All multi-byte integer fields are **little-endian**
* All offsets and lengths are in **bytes**
* Character encoding (when applicable): UTF-8

---

## 3. Common Concepts

### 3.1 Alignment

* Records are aligned to the backend-reported write alignment
* Padding bytes are zero-filled
* Alignment is part of the record length

### 3.2 Checksums

* Default checksum: CRC32C
* Checksum is calculated over the record header (excluding checksum field) + payload
* Checksum mismatch invalidates the record

---

## 4. Record Framing

All AfFS records use a common framing structure.

### 4.1 Record Header (Fixed Size)

| Field         | Size (bytes) | Description                       |
| ------------- | ------------ | --------------------------------- |
| Magic         | 4            | ASCII 'AFR1'                      |
| Version       | 1            | Layout version (1)                |
| RecordKind    | 1            | Data Kind identifier              |
| SizeClass     | 1            | Size Class identifier             |
| AccessProfile | 1            | Storage Access Profile identifier |
| Flags         | 2            | Record flags                      |
| PayloadLength | 8            | Length of payload                 |
| Checksum      | 4            | CRC32C                            |

Header size: 22 bytes (aligned to backend alignment).

### 4.2 Payload

* Opaque to the binary layer
* Interpreted by higher layers based on RecordKind

---

## 5. Segment Structure

Segments group records with the same intent.

### 5.1 Segment Header

| Field         | Size (bytes) | Description                |
| ------------- | ------------ | -------------------------- |
| Magic         | 4            | ASCII 'AFSG'               |
| Version       | 1            | Segment format version (1) |
| SegmentKind   | 1            | Size Class / Data grouping |
| AccessProfile | 1            | Declared access profile    |
| Flags         | 1            | Segment flags              |
| SegmentLength | 8            | Total segment size         |
| Checksum      | 4            | Header checksum            |

Segment header is written once when segment is created.

### 5.2 Segment Rules

* A segment contains only records compatible with its declared intent
* Mixed access profiles are not allowed
* Segments grow append-only

---

## 6. Superblock

The superblock anchors the AfFS store.

### 6.1 Location

* Stored at a fixed offset defined by backend policy
* Mirrored if redundancy is enabled

### 6.2 Superblock Format

| Field             | Size (bytes) | Description                           |
| ----------------- | ------------ | ------------------------------------- |
| Magic             | 4            | ASCII 'AFSB'                          |
| Version           | 1            | Superblock version (1)                |
| Flags             | 1            | Feature flags                         |
| Reserved          | 2            | Reserved                              |
| RootPointerOffset | 8            | Offset of current root pointer record |
| StoreUUID         | 16           | Unique store identifier               |
| Checksum          | 4            | CRC32C                                |

---

## 7. Root Pointer Record

The root pointer identifies the current trusted world state.

### 7.1 Root Pointer Record Payload

| Field            | Size (bytes) | Description                             |
| ---------------- | ------------ | --------------------------------------- |
| CheckpointId     | 16           | Identifier of pinned checkpoint         |
| IndexLogOffset   | 8            | Offset to primary index log position    |
| SegmentPositions | Variable     | Highest known valid offsets per segment |

The root pointer record itself uses standard record framing.

---

## 8. Allocation and Growth

* Allocation is append-only
* No in-place overwrite of records
* New segments are allocated when size class boundaries require

Allocation policy is defined by the layout allocator layer.

---

## 9. Invalid and Torn Records

A record is considered invalid if:

* magic does not match
* version is unsupported
* payload length is inconsistent
* checksum validation fails

Invalid records are skipped during scan.

---

## 10. Forward Compatibility

* Unknown RecordKind values must be skipped safely
* Version mismatches require explicit compatibility handling

---

## 11. Design Invariants

* All records are self-describing
* All records are checksum-verified
* Segments enforce uniform intent
* Root pointer defines trusted state
* Reconstruction is always possible from record scans

---

End of document.
