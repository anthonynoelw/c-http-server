---
name: review
description: Read-only code review of C code. Reviews the current diff or given files for bugs, memory safety, security and style, without changing anything. Use when the user invokes /review or says "code review", "schau dir den Code an", "review mal".
argument-hint: [file or path, default: current git diff]
allowed-tools: Read Grep Glob
---

# Review – inspect code, change nothing

Review target: $ARGUMENTS (if empty: the uncommitted changes from `git diff` and `git diff --staged`)

## Hard rules (apply in EVERY mode)

- **Never modify anything.** No Write, Edit, NotebookEdit, no file creation/deletion/moves.
- **No state-changing commands.** No `git commit/add/checkout/reset/push`, no `make`/build/install, no `rm`/`mv`/`cp`/`mkdir`/`touch`, no redirects, no `sed -i`.
- Only read-only inspection: Read, Grep, Glob, and Bash like `ls`, `cat`, `git diff`, `git log`, `git show`, `git status`.
- Findings are reported in the answer text only. Fixes are shown as snippets in the answer, never applied.

## What to check (this is a C HTTP server)

1. **Memory safety:** buffer overflows, off-by-one, missing NUL terminators, use-after-free, double free, leaks, unchecked `malloc`/`realloc`, uninitialized variables.
2. **Parsing robustness:** malformed or oversized requests, missing `\r\n`, partial reads from `recv`, header/URI length limits, integer overflow in length handling.
3. **Sockets and syscalls:** unchecked return values (`socket`, `bind`, `listen`, `accept`, `recv`, `send`), `EINTR`, partial `send`, leaked file descriptors, missing `close`.
4. **Security:** path traversal, unbounded input, format-string issues, unsafe functions (`strcpy`, `sprintf`, `gets`, `strcat`).
5. **Correctness vs. HTTP spec:** status codes, header handling, behavior described in `docs/`.
6. **Style and maintainability:** naming, header guards, dead code, duplication, unclear ownership of memory, missing comments where logic is non-obvious.

## How to answer

1. Read the changed code and the surrounding context (headers, callers) before judging; don't guess.
2. Answer in the user's language, concisely.
3. Group findings by severity: **Critical** (crash/security), **Important** (bug/robustness), **Hint** (style/nice-to-have).
4. Reference each finding as `file_path:line_number`, say what is wrong and why, and give a short fix suggestion as a snippet.
5. Mention what looks good, briefly. If nothing is wrong, say so instead of inventing findings.
