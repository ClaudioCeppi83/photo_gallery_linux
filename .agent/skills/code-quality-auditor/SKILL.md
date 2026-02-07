---
name: code-quality-auditor
description: Expert guide for ensuring high code quality and security in Rust projects. Use this skill to configure linters, formatters, and security scanners.
license: MIT
---

# Code Quality Auditor

This skill helps you maintain a pristine Rust codebase by leveraging the ecosystem's powerful static analysis tools.

## The Holy Trinity of Rust Quality

### 1. Clippy (Linter)
Catch common mistakes and improve code idioms.

**Configuration (`.clippy.toml`):**
Configure lints to be stricter than default.

```toml
# Deny warnings in CI
[workspace.lints.clippy]
pedantic = "warn"
nursery = "warn"
unwrap_used = "warn" # Avoid panics in production
expect_used = "warn"
```

**Running Clippy:**
```bash
cargo clippy --all-targets --all-features -- -D warnings
```

### 2. Rustfmt (Formatter)
Enforce a consistent style guide automatically.

**Configuration (`rustfmt.toml`):**

```toml
edition = "2021"
max_width = 100
imports_granularity = "Crate"
group_imports = "StdExternalCrate"
```

**Running Rustfmt:**
```bash
cargo fmt --all -- --check
```

### 3. Cargo Audit (Security)
Detect vulnerabilities in your dependencies.

**Running Audit:**
```bash
cargo audit
```
*Note: Requires installation via `cargo install cargo-audit`.*

## Pre-commit Hook Strategy

Don't rely on memory. Use a Git hook or `pre-commit` framework to run these checks before every commit.

```bash
#!/bin/sh
# .git/hooks/pre-commit

cargo fmt --all -- --check
cargo clippy -- -D warnings
cargo test
```
