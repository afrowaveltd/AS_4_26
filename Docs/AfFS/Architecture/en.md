# AfFS Architecture

AfFS (Afrowave Flexible Storage) is the persistence layer of AS 4_26.

It is responsible for storing data reliably and efficiently, while remaining flexible enough to operate over different underlying transports.
AfFS is not a traditional filesystem and not a kernel component.

It is a **userspace storage engine**.

---

## What AfFS is

AfFS is a storage abstraction that: 

* stores structured and unstructured data,
* preserves data integrity across restarts,
* adapts to different storage backends,
* exposes predictable behavior to the runtime.

AfFS exists to give the runtime a stable and portable persistence layer.

---

## What AfFS is not

AfFS is not:

* a kernel filesystem,
* a POSIX replacement,
* a virtual filesystem layer for the OS,
* a user-facing file manager.

It does not attempt to mirror or replace operating system filesystems.

---

## Userspace-first storage

AfFS runs entirely in userspace.

The operating system kernel is used only for:

* reading bytes,
* writing bytes,
* flushing buffers,
* signaling errors.

All decisions about:

* layout,
* metadata,
* integrity,
* structure

are made inside AfFS itself.

---

## Flexible by design

The defining property of AfFS is flexibility.

The same storage logic can operate on top of:

* a regular file,
* a raw partition,
* an entire block device,
* other transports in the future.

From the runtime’s perspective, these backends behave consistently.

---

## Storage backends

A backend defines *where* data lives.

Examples include:

* file backend (storage-in-a-file),
* partition backend (raw block device),
* disk backend (whole drive).

Backends provide block-level access.
They do not interpret data meaning.

---

## Separation of concerns

AfFS follows a strict separation of responsibilities:

* **Runtime** decides what data exists and how it is used.
* **AfFS** decides how data is persisted and recovered.
* **Backend** decides how bytes are physically accessed.

No layer leaks responsibility into another.

---

## Data and metadata

AfFS distinguishes between:

* data blocks (actual stored content),
* metadata blocks (structure, references, integrity information).

Metadata is minimal but explicit.
It exists to support:

* consistency,
* recovery,
* navigation.

---

## Atomicity and consistency

AfFS is designed to avoid partial or ambiguous states.

Operations that modify structure or metadata are:

* atomic,
* ordered,
* recoverable after interruption.

Consistency is preferred over raw performance.

---

## Integrity and validation

AfFS includes mechanisms for:

* detecting corruption,
* validating structure,
* verifying data integrity.

Integrity rules are defined at the architecture and layout levels.

---

## Relationship to AUV

AfFS does not interpret the meaning of data.

It stores AUV values as opaque structured content.

The runtime is responsible for understanding and validating AUV semantics.
AfFS is responsible for storing and retrieving bytes reliably.

---

## Relationship to the runtime

AfFS is controlled exclusively by the runtime.

The runtime:

* opens and closes storage volumes,
* requests reads and writes,
* manages lifecycle and recovery.

AfFS never initiates actions on its own.

---

## Portability and longevity

AfFS layouts are designed to be:

* portable across platforms,
* readable by different implementations,
* stable across versions.

Data stored today should remain accessible in the future.

---

## Relationship to disk layout

This document defines *architecture and intent*.

Exact on-disk structures are defined separately in:

* `Docs/AfFS/DiskLayout/en.md`
* `Specs/AfFS.DiskLayout.md`

Architecture explains *why*.
Disk layout defines *how*.

---

## Summary

AfFS is the flexible persistence backbone of AS 4_26.

It:

* lives in userspace,
* adapts to multiple storage backends,
* separates data meaning from storage mechanics,
* provides stable, predictable persistence for the runtime.