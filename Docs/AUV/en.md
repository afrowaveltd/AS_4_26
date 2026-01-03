# AUV – Universal Values

AUV (Universal Values) is the data language of AS 4_26.

It defines how all data is represented, stored, transported, and understood inside the system.
There are no alternative data models hidden behind APIs or interfaces.

If data exists in AS 4_26, it exists as AUV.

---

## Why Universal Values?

Most software systems use many different data representations at once:

* database types,
* API types,
* transport formats,
* scripting values,
* UI-specific models.

This creates friction:

* constant conversion,
* edge cases,
* inconsistent behavior,
* hidden complexity.

AUV removes this fragmentation by defining **one shared value model** used everywhere.

---

## A simple mental model

You can think of AUV as a very small, very strict “alphabet” for data.

Everything the system knows is written using this alphabet.
More complex meanings are built by combining simple values.

AUV is closer to:

* “how values exist”

than to:

* “how values are displayed”.

---

## The AUV value types

AUV intentionally defines a **small, closed set of types**.

### Null

Represents the explicit absence of a value.

Null is a value.
It is not “missing”, and it is not the same as zero, false, or an empty string.

---

### Bool

A boolean value: `true` or `false`.

---

### Int64

A signed 64-bit integer.

Used for:

* counters,
* identifiers,
* sizes,
* timestamps,
* exact numeric values.

---

### Float64

A 64-bit floating point number.

Used for:

* measurements,
* approximations,
* values where fractional parts matter.

---

### Char

A single character, represented as a Unicode code point.

Char represents a *character*, not a byte and not a UTF-16 code unit.

---

### String

A sequence of characters encoded as UTF-8.

Strings represent human-readable text.
They are not raw binary data.

---

### Array

An ordered list of AUV values.

Arrays preserve order.
The position of values matters.

Example:

```
[ 1, 2, 3 ]
```

---

### Object

A mapping from string keys to AUV values.

Objects represent structured data.
Keys are unique and meaningful.

Example:

```
{
  "name": "Alice",
  "age": 30,
  "active": true
}
```

---

### Binary

A sequence of raw bytes.

Binary is used for:

* files,
* blobs,
* encoded data,
* anything that is not text.

Binary data has no implicit meaning.

---

## Composition: building meaning

AUV values become powerful through composition.

For example:

* a “record” is an Object,
* a “list of records” is an Array of Objects,
* a “file” may be an Object containing Binary data and metadata.

AUV does not define meaning.
It defines **structure**.

Meaning is provided by higher layers of the system.

---

## One model everywhere

The same AUV rules apply to:

* storage (AfFS),
* runtime APIs,
* command-line interaction,
* scripting,
* network transport.

This guarantees that:

* data behaves the same way everywhere,
* tools and interfaces remain consistent,
* behavior is predictable.

---

## Why AUV is intentionally limited

AUV does not try to model everything.

There are:

* no dates,
* no decimals,
* no sets,
* no special numeric types.

This is deliberate.

Complex concepts must be expressed using simple building blocks.
This keeps the core stable and understandable.

---

## Stability over convenience

Once defined, AUV rules change very slowly.

Convenience features belong in higher layers.
AUV itself must remain boring, predictable, and reliable.

---

## Relationship to the specification

This document explains AUV conceptually.

The formal definition lives in:

* `Specs/AUV.Spec.md`

The specification defines:

* canonical forms,
* wire format,
* limits,
* equality rules,
* compatibility guarantees.

Implementations must follow the specification, not this explanation.

---

## Summary

AUV is the common language of AS 4_26.

* small,
* explicit,
* universal,
* predictable.

Everything else in the system is built on top of it.
