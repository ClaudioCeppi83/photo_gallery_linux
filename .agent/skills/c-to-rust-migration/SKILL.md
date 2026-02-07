---
name: c-to-rust-migration
description: Expert guide for systematically migrating C code to safe Rust, covering FFI, safe wrappers, and iterative replacement strategies. Use this skill when porting legacy C projects or functions to Rust.
license: MIT
---

# C to Rust Migration Guide

This skill provides a structured approach for rewriting or interfacing with legacy C code from Rust.

## Migration Strategy: The "Strangler Fig" Pattern

Don't rewrite everything at once. Replace C modules one by one, keeping the C interface intact during the transition.

1.  **Identify a module:** Pick a small, self-contained C file (e.g., `utils.c`).
2.  **Write Rust equivalent:** Reimplement the logic in Rust.
3.  **Create C-compatible API:** Expose `extern "C"` functions from Rust that match the original C header.
4.  **Update Build System:** Link the new Rust object/library instead of the old C object.
5.  **Verify:** Run existing C tests against the new Rust implementation.

## 1. Interfacing with C (FFI)

Use `bindgen` to automatically generate Rust bindings for C headers. Avoid manual `extern "C"` blocks for complex headers.

```bash
bindgen wrapper.h -o bindings.rs
```

### Safety First
- **Pointers are `unsafe`:** Wrap raw pointers in safe abstractions as soon as possible.
- **Strings:** Convert `*const c_char` to `CStr` (borrowed) or `CString` (owned).
- **Structs:** Ensure `#[repr(C)]` layout compatibility for structs shared across the boundary.

## 2. Wrapping Unsafe C in Safe Rust

**Pattern: The Handle Wrapper**
If C uses opaque pointers (e.g., `Context*`), wrap them in a Rust struct that manages lifetime.

```rust
struct SafeContext {
    raw: *mut sys::Context,
}

impl Drop for SafeContext {
    fn drop(&mut self) {
        unsafe { sys::context_free(self.raw) };
    }
}
```

## 3. Common Translations

| C Concept | Rust Equivalent | Notes |
| :--- | :--- | :--- |
| `void*` context | `Box<T>` / `*mut c_void` | Use `Box::into_raw` and `Box::from_raw` |
| callbacks | `extern "C" fn` + user_data | Pass a closure/trait object as `user_data` |
| `malloc`/`free` | `Vec<T>`, `Box<T>`, `String` | Rust manages memory automatically |
| `#define` constant | `const` or `enum` | Use `bitflags!` for bitmasks |
| `goto error;` | `Result<T, E>` + `?` operator | Idiomatic error handling |

## Tools
- **c2rust**: Automatic translation tool (produces unsafe Rust). Good for initial porting of algorithms, but requires heavy refactoring.
- **bindgen**: Generates FFI bindings.
- **cc**: Build dependency for compiling C code within `build.rs`.
