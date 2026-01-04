# AfFS Backend I/O Layer

This document defines the **platform-dependent I/O boundary** for AfFS within the AS426 architecture.

AfFS must be implementable on:

* host operating systems (Windows/Linux/FreeBSD)
* future custom kernels (e.g., AfNAS-OS)

To achieve this, all low-level access to bytes is isolated behind a **Backend I/O Layer**.
Everything above this layer is platform-independent.

---

## 1. Role and Non-Goals

### 1.1 Role

The Backend I/O Layer provides:

* deterministic reads/writes at explicit offsets
* durability boundaries (flush/sync)
* alignment and size information
* structured error reporting (not just exceptions)
* optional best-effort reading for failing media

### 1.2 Non-Goals

The Backend I/O Layer does **not**:

* interpret AfFS structures (no superblock parsing, no object headers)
* manage allocation policy
* implement indexes, checkpoints, or recovery
* perform compression/encryption (future layers may)

---

## 2. Backend Types

AfFS supports two backend forms.
Both implement the same core capabilities.

### 2.1 Block Backend

A backend that reads and writes fixed-size blocks.
Examples:

* raw disk
* dedicated partition
* virtual block device

### 2.2 File Backend

A backend that reads and writes arbitrary byte ranges in a file.
Examples:

* AfFS store file on a host filesystem
* container file in a disk image

Note: physical sector separation cannot be guaranteed on file backends.

---

## 3. Core Operations

All operations are explicit in position and length.
No implicit seeking.

### 3.1 Read

Read bytes from `offset` into a provided buffer.

Required behavior:

* returns the number of bytes read
* may return partial reads
* never fabricates data

### 3.2 Write

Write bytes at `offset` from a provided buffer.

Required behavior:

* returns the number of bytes written
* may return partial writes
* never claims success without writing

### 3.3 Flush / Durability Barrier

A flush indicates a durability boundary.

Required behavior:

* requests that all prior successful writes become durable
* if durability cannot be guaranteed, the backend must report it

Implementation examples:

* host OS: fsync / fdatasync / FlushFileBuffers
* custom kernel: explicit device barrier

---

## 4. Capability and Geometry Queries

Backends must provide enough geometry to keep AfFS predictable.

### 4.1 Size

* total addressable size in bytes

### 4.2 Alignment

Backends report:

* preferred read alignment (bytes)
* preferred write alignment (bytes)
* minimal atomic write size (if known)

For block devices, alignment is usually the logical sector size.

### 4.3 Identity

Backends may provide a stable identity:

* device identifier / serial / path hash (best effort)

Identity helps detect accidental store moves.

---

## 5. Error Model (Structured Outcomes)

AfFS requires errors to be classified so higher layers can make deterministic decisions.
Backends must return structured outcomes with:

* category
* bytes processed
* errno / OS code (if available)
* retry hint
* timing metadata (optional)

### 5.1 Error Categories

At minimum:

* `Ok`
* `Partial` (some bytes processed, then failure)
* `TransientError` (retry may succeed)
* `PermanentError` (retry unlikely, e.g., bad sector)
* `Timeout`
* `AccessDenied`
* `OutOfSpace`

The backend must not downgrade errors into generic failure.

### 5.2 Retry Semantics

If an operation fails with `TransientError` or `Timeout`, the backend may:

* perform internal retries (bounded)
* or return immediately and let higher layers decide

All retries must be observable in the result (attempt count).

---

## 6. Best-Effort Reading (Failing Media)

AfFS supports migration and recovery from degrading devices.
The backend may implement a best-effort read mode:

* attempts bounded retries
* tolerates slow regions
* returns partial buffers when possible
* clearly reports unreadable ranges

Best-effort mode must never silently replace unreadable bytes.

---

## 7. Observability Hooks

Backends should expose lightweight metrics to support AfFS protective mode:

* last error timestamp
* rolling read/write error counts
* rolling retry counts
* rolling latency summaries

These are hints, not canonical truth.

---

## 8. Determinism Requirements

To maintain trust, the Backend I/O Layer must be deterministic:

* the same request on the same medium yields the same outcome (modulo progressive failure)
* results must always indicate partial success explicitly
* flush semantics must be consistent within a backend implementation

---

## 9. Integration Contract

Everything above the backend layer assumes:

* explicit offsets
* explicit lengths
* explicit durability boundaries
* explicit error categories

No higher AfFS logic may call platform APIs directly.
All platform dependencies are confined to the backend implementation.

---

End of document.
