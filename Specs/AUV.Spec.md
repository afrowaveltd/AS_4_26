# AUV Specification

**Document:** AUV.Spec.md
**Status:** Draft (v1)
**Scope:** Universal value model and canonical rules for AS 4_26

This specification defines AUV (Universal Values): the value model, canonical semantics, and a wire format for serialization and transport.

The key requirement is cross-implementation consistency.

---

## 0. Conventions

### 0.1 Normative language

The keywords **MUST**, **MUST NOT**, **SHOULD**, **SHOULD NOT**, and **MAY** are to be interpreted as described in RFC 2119.

### 0.2 Goals

AUV MUST be:

* small and universal,
* deterministic across implementations,
* stable over time,
* usable for storage, transport, APIs, and command/query interaction.

### 0.3 Non-goals

AUV does not define:

* query languages,
* indexing strategies,
* filesystem semantics,
* UI rendering.

---

## 1. Type system

AUV defines a closed set of value types:

1. Null
2. Bool
3. Int64
4. Float64
5. Char
6. String
7. Binary
8. Array
9. Object

No other base types exist at the AUV level.

---

## 2. Logical semantics

### 2.1 Null

* Represents explicit absence of a value.
* Null is a value.

### 2.2 Bool

* Two values: `false` and `true`.

### 2.3 Int64

* Signed 64-bit integer.
* Range: [-9,223,372,036,854,775,808 … 9,223,372,036,854,775,807].

### 2.4 Float64

* IEEE 754 binary64.
* Special values (±Infinity, NaN) are allowed.

**Canonical NaN:**

* All NaN values MUST be canonicalized to a single quiet-NaN bit pattern in canonical form.

**Signed zero:**

* `+0.0` and `-0.0` MUST be treated as semantically equal.

### 2.5 Char

* A single Unicode scalar value.
* Valid range: U+0000 to U+10FFFF excluding surrogate range U+D800 to U+DFFF.

### 2.6 String

* A sequence of Unicode scalar values.
* Serialization MUST use UTF-8.
* Strings MUST be valid UTF-8 and MUST NOT contain invalid byte sequences.

### 2.7 Binary

* An arbitrary byte sequence.
* No implicit encoding.

### 2.8 Array

* An ordered sequence of AUV values.
* Order is significant.

### 2.9 Object

* A mapping from string keys to AUV values.
* Keys MUST be Strings.
* Keys MUST be unique.

**Canonical key ordering:**

* Canonical objects MUST store keys in ascending lexicographic order by UTF-8 byte sequence.

---

## 3. Equality and ordering

### 3.1 Semantic equality

Two AUV values are semantically equal if:

* They have the same type and equal contents according to the rules below.

Rules:

* Null: equal to Null.
* Bool: equal if both are the same boolean.
* Int64: equal if numeric values match.
* Float64:

  * +0.0 equals -0.0.
  * NaN equals NaN (because canonical NaN is required for canonical form).
  * Otherwise numeric equality.
* Char: equal if code points match.
* String: equal if Unicode scalar sequences match (equivalently: identical UTF-8 byte sequences).
* Binary: equal if identical bytes.
* Array: equal if same length and each element is semantically equal in order.
* Object: equal if same set of keys and each corresponding value is semantically equal.

### 3.2 Total ordering (for deterministic sorting)

AUV defines a total ordering to allow deterministic sorting.

Type precedence order (lowest to highest):

1. Null
2. Bool
3. Int64
4. Float64
5. Char
6. String
7. Binary
8. Array
9. Object

Within a type:

* Bool: `false < true`.
* Int64: numeric ordering.
* Float64:

  * Order by numeric value.
  * `-Infinity < finite numbers < +Infinity < NaN`.
  * `+0.0` and `-0.0` compare equal.
* Char: by code point.
* String: lexicographic by UTF-8 byte sequence.
* Binary: lexicographic by bytes.
* Array: lexicographic by element ordering; shorter prefix is smaller.
* Object:

  * Compare by ordered key list (UTF-8 lexicographic).
  * If keys equal, compare values by key order.

---

## 4. Limits (safety boundaries)

Implementations MUST enforce limits to prevent resource exhaustion.

Default limits (v1):

* Maximum nesting depth: 256
* Maximum String length (bytes): 64 MiB
* Maximum Binary length (bytes): 1 GiB
* Maximum Array length: 10,000,000 elements
* Maximum Object key count: 10,000,000 keys
* Maximum Object key length (bytes): 4 KiB

Implementations MAY allow configurable limits, but MUST reject inputs exceeding active limits.

---

## 5. Canonical form

Canonical AUV is the preferred stable representation.

A value is canonical if:

* it uses the canonical wire format (AUV Wire v1),
* objects have unique keys and are key-sorted,
* Float64 NaN values are canonicalized,
* no invalid UTF-8 exists,
* all limits are respected.

---

## 6. Wire format: AUV Wire v1

### 6.1 Overview

AUV Wire v1 is a typed, length-delimited binary encoding.

Properties:

* streamable
* supports skipping values without decoding full subtrees
* deterministic when canonical rules are followed

### 6.2 Endianness

* Int64 and Float64 payloads MUST be little-endian.

### 6.3 VarUInt (unsigned variable-length integer)

Lengths use VarUInt (LEB128-style):

* 7 bits per byte for data
* MSB indicates continuation
* VarUInt MUST use the shortest encoding (no leading zero groups).

### 6.4 Record structure

Each value is encoded as:

* `TypeTag` (1 byte)
* `Length` (VarUInt)
* `Payload` (Length bytes)

`Length` is the number of bytes in `Payload`.

### 6.5 Type tags

Type tags (v1):

* 0x00: Null
* 0x01: Bool
* 0x02: Int64
* 0x03: Float64
* 0x04: Char
* 0x05: String
* 0x06: Binary
* 0x07: Array
* 0x08: Object

Unknown tags:

* Decoders MUST reject unknown tags in v1.

### 6.6 Payload definitions

#### Null (0x00)

* Length MUST be 0.

#### Bool (0x01)

* Length MUST be 1.
* Payload: 0x00 = false, 0x01 = true.
* Other values MUST be rejected.

#### Int64 (0x02)

* Length MUST be 8.
* Payload: signed 64-bit little-endian.

#### Float64 (0x03)

* Length MUST be 8.
* Payload: IEEE 754 binary64 little-endian.
* Canonical form MUST use canonical NaN bit pattern for NaN.

#### Char (0x04)

* Length MUST be 4.
* Payload: unsigned 32-bit code point (little-endian).
* MUST be a valid Unicode scalar value.

#### String (0x05)

* Length MAY be 0..limit.
* Payload: UTF-8 bytes.
* MUST be valid UTF-8.

#### Binary (0x06)

* Length MAY be 0..limit.
* Payload: raw bytes.

#### Array (0x07)

* Payload is a sequence of embedded AUV values (each as full record).
* The array payload is parsed until payload length is consumed exactly.
* Empty array: Length = 0.

#### Object (0x08)

* Payload is a sequence of key/value pairs.
* Each key is encoded as a full AUV value and MUST be a String.
* Each value is encoded as a full AUV value.
* Parsed until payload length is consumed exactly.

Pairing rules:

* The payload MUST contain an even number of embedded AUV records.
* Decoders MUST parse `Key` then `Value` repeatedly.
* If the payload ends after a Key without a Value, the decoder MUST reject the object.

Key rules:

* Keys MUST be unique.
* Canonical objects MUST have keys in ascending UTF-8 lexicographic order.

Note:

* A future wire revision MAY add an optional upfront pair-count for faster validation/allocation, but AUV Wire v1 does not require it.

### 6.7 Validation rules

Decoders MUST reject:

* incorrect fixed lengths
* invalid UTF-8
* invalid Char code points
* duplicate object keys
* payload overrun/underrun
* VarUInt non-minimal encodings
* values exceeding active limits

---

## 7. Versioning and compatibility

* This document defines AUV Wire v1.
* Any future wire version MUST define:

  * how to detect the version,
  * how v1 decoders should behave (reject or partial support),
  * reserved tag space rules.

v1 decoders MUST reject unknown tags to avoid silent misinterpretation.

---

## 8. Test vectors (to be added)

This section will contain canonical encoded examples:

* Null, Bool
* Int64 boundary values
* Float64 special values (+0/-0, infinities, NaN)
* UTF-8 strings including non-ASCII
* nested arrays/objects

---

## 9. Notes

* Canonical object key ordering by UTF-8 bytes avoids locale-dependent behavior.
* Canonical NaN ensures deterministic equality and hashing.

---

## 10. Changelog

* v1 draft: initial type system, canonical rules, and AUV Wire v1 encoding.
