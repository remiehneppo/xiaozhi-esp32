# Domain Docs

How engineering skills should consume this repo's domain documentation when exploring the codebase.

## Layout

This repo uses a multi-context layout:

- `CONTEXT-MAP.md` at the repo root points to the active context files
- Each context has its own `CONTEXT.md`
- `docs/adr/` holds architectural decision records, either shared or scoped by context as documented in `CONTEXT-MAP.md`

## Before exploring, read these

- `CONTEXT-MAP.md` at the repo root
- The relevant `CONTEXT.md` for the area you are about to work in
- `docs/adr/` entries that touch that area

If any of these files do not exist, proceed silently. Do not flag their absence or suggest creating them upfront. The producer skill (`/grill-with-docs`) creates them lazily when terms or decisions get resolved.

## Use the glossary's vocabulary

When your output names a domain concept in an issue title, refactor proposal, hypothesis, or test name, use the term as defined in the relevant `CONTEXT.md`. Do not drift to synonyms the glossary explicitly avoids.

If the concept you need is not in the glossary yet, that is a signal that the project language is incomplete or that you may be inventing terminology the project does not use.

## Flag ADR conflicts

If your output contradicts an existing ADR, surface it explicitly rather than silently overriding it:

> _Contradicts ADR-0007 (event-sourced orders) - but worth reopening because..._
