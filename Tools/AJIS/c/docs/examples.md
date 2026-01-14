# AJIS - Examples

Practical examples of using AJIS.

## 1. Configuration with Comments

```ajis
{
  // App metadata
  "name": "myapp",
  "version": "1.0.0",
  
  /* 
     Network configuration 
     Default port is 8080
  */
  "network": {
    "port": 8080,
    "host": "localhost"
  }
}
```

## 2. Advanced Numeric Formats

```ajis
{
  "limits": {
    "max_connections": 1_000_000,
    "buffer_size": 0xFF_AA,
    "flags": 0b1010_1100
  },
  "constants": {
    "pi": 3.14159,
    "e": 2.71828
  }
}
```

## 3. Binary Data

```ajis
{
  "checksum": hex"DEADBEEF",
  "payload": b64"SGVsbG8gV29ybGQ="
}
```

## 4. Heterogeneous Array

```ajis
[
  "text",
  42,
  0x2A,
  true,
  null,
  { "key": "value" },
  [1, 2, 3]
]
```

## 5. From AJIS to JSON (Equivalence)

**AJIS:**
```ajis
{
  // Port number
  "port": 0x50 // 80 in hex
}
```

**Equivalent JSON:**
```json
{
  "port": 80
}
```
