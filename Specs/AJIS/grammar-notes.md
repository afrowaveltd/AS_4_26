
# AJIS Grammar Notes

This document provides informal grammar notes for implementers.

These notes are non-normative.

AJIS grammar is JSON-like and suitable for recursive descent parsing.

Key notes:

- Comments behave like whitespace and may appear between tokens.
- Comments must not appear inside tokens.
- Arrays may contain mixed value types.
- Object keys are strings and are case-sensitive.
- Duplicate object keys are an error.

Detailed grammar and token rules will be expanded after AJIS v1 stabilization.
