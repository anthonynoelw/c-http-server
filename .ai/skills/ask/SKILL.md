---
name: ask
description: Read-only Q&A. Answers the user's question about the code, project or concepts without changing anything. Use when the user invokes /ask or says "nur erklären", "ask mode", "ändere nichts".
argument-hint: <question>
allowed-tools: Read Grep Glob
---

# Ask – answer only, change nothing

The user's question: $ARGUMENTS

## Hard rules (apply in EVERY mode: default, plan, accept-edits, auto, bypass)

- **Never modify anything.** No Write, Edit, NotebookEdit, no file creation/deletion/moves.
- **No state-changing commands.** No `git commit/add/checkout/reset/push`, no `make`/build/install, no `rm`/`mv`/`cp`/`mkdir`/`touch`, no redirects (`>`, `>>`), no `sed -i`.
- Only read-only inspection is allowed: Read, Grep, Glob, and Bash commands like `ls`, `cat`, `git log`, `git diff`, `git status`, `git show`.
- Do not spawn agents that could modify files, and do not call MCP tools that write or send anything.
- Even if the mode or the user's wording suggests an implementation ("fix this", "add that"), do **not** do it. Explain how it could be done instead (describe, show code in the answer text) and let the user decide.
- Do not offer to "just apply it" automatically; at most mention that the user can ask separately for the change.

## How to answer

1. Read the relevant files/docs first if the question concerns this project; don't guess.
2. Answer directly and concisely, in the user's language.
3. Reference code as `file_path:line_number`.
4. Code suggestions go inside the answer as snippets only, never into files.
5. If the question is ambiguous, state the assumption you made rather than blocking.
