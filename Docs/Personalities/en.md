# Personalities

A personality is how AS 4_26 becomes *hostable as a character*.

The core runtime remains the same, but personalities define how the system behaves and presents itself to users.

Personalities are not forks.
They are packages.

---

## Why personalities exist

Different environments want different behaviors.

A home lab, a school lab, a production server, and a developer workstation may all run the same runtime, but they need:

* different defaults,
* different command sets,
* different policies,
* different UI layouts,
* different safety restrictions.

Personalities make these differences explicit and repeatable.

---

## What a personality can define

A personality may define:

* default configuration values,
* enabled and disabled command groups,
* UI layout and navigation structure,
* safety policies (for example, confirmation levels),
* roles and access expectations,
* presentation (names, labels, themes),
* startup workflows,
* bundled tools.

A personality can feel like:

* a product,
* a distro,
* a “mode”,

without changing the runtime truth.

---

## What a personality must NOT change

A personality must not change:

* AUV data contracts,
* AfFS storage semantics,
* core execution rules,
* the meaning of commands and results.

Personalities configure behavior.
They do not rewrite reality.

---

## Personality as a package

A personality should be distributable as a package.

Conceptually, a personality includes:

* a manifest (identity, version, compatibility),
* configuration defaults,
* UI definitions (web navigation, panels),
* optional assets (icons, themes),
* optional help text and tutorials.

Personalities must be:

* installable,
* removable,
* upgradable.

---

## Compatibility

Personalities must declare compatibility with:

* runtime versions,
* required capabilities,
* minimum feature sets.

A personality should fail gracefully when:

* required features are missing,
* the runtime is too old.

---

## Security and safety

Personalities may tighten safety.

For example:

* require extra confirmations,
* restrict destructive commands,
* disable certain tools,
* enforce read-only modes.

Safety belongs to the runtime.
Personalities can configure safety policies, but cannot bypass them.

---

## Personalities and UI

Personalities can shape the UI experience:

* what appears on the dashboard,
* which workflows are highlighted,
* what commands are shown by default,
* whether the web UI starts in “safe mode”.

A key benefit of AS 4_26 is that:

* the Web UI can embed a terminal,
* and that terminal can run the same CLI/TUI tools.

This creates a strong sense of continuity:

* GUI and terminal are not separate worlds.

---

## Examples (conceptual)

Possible personalities include:

* **Minimal**: small, safe defaults, essential commands only.
* **Developer**: verbose diagnostics, tooling, test helpers.
* **Demo**: friendly UI, guided workflows, educational hints.
* **School Lab**: restricted destructive actions, shared-user safety.
* **Server**: service-first, monitoring-focused, minimal UI.

---

## Relationship to documentation

Personalities must be documented.

A personality package should include:

* a clear description,
* intended users,
* enabled features,
* safety expectations,
* and example workflows.

---

## Summary

Personalities make AS 4_26 hostable as “a character”.

They:

* configure behavior and presentation,
* package defaults and policies,
* remain compatible with the same core runtime.

The runtime stays true.
The personality shapes the experience.
