# Issue Tracker: Local Markdown

Issues and PRDs for this repo live as markdown files under `.scratch/`.

## Conventions

- One feature per directory: `.scratch/<feature-slug>/`
- PRD is `.scratch/<feature-slug>/PRD.md`
- Implementation issues are `.scratch/<feature-slug>/issues/<NN>-<slug>.md`, numbered from `01`
- Triage state is recorded as a `Status:` line near the top of each issue file
- Comments and conversation history append to the bottom of the file under `## Comments`

## Skill behavior

- When a skill says "publish to the issue tracker", create a new file under `.scratch/<feature-slug>/` and create parent directories if needed.
- When a skill says "fetch the relevant ticket", read the file at the referenced path. The user will usually pass a path or issue number directly.
- When a skill needs the triage state, use the label strings from `triage-labels.md`.
