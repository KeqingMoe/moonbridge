# moonbridge

[中文](./README.md) | [English](./README.en.md)

> Note: This English README is AI-generated.

`moonbridge` is a C++ header-only library for MoonBit native FFI, designed to make type binding, reference counting, and closure passing between MoonBit and C++ more controllable.

Its primary purpose is to help implement MoonBit bindings for C++ libraries.

This is a single-header library, with the main entry at `include/moonbridge.hpp`.

This repository currently contains:

- `include/moonbridge.hpp`: core bridge implementation
- `example/lib`: example binding implementation
- `example/mbt`: MoonBit-side declarations and demo app

## Features

- Single-header: core implementation is concentrated in `include/moonbridge.hpp`.
- MoonBit ABI-oriented: provides `moonbit_trait<T>` and common wrapper types.
- External object bridging: safely expose C++ objects to MoonBit via `box<T>`.
- Closure bridging: `fn<R(Args...)>` can carry MoonBit closures.
- Persistent ownership semantics: `own<T>` is used to keep MoonBit objects on the C++ side and maintain reference counts.

## Dependencies

- A compiler with C++23 support
- MoonBit toolchain and runtime headers (included from `~/.moon/include` by default)
- [xmake](https://xmake.io) (example only)
- Qt (example only)

## Build Notes (Current Status)

The example is currently only known to run reliably on **macOS with Qt installed**.

This is mainly constrained by the current MoonBit build system:

- It cannot automatically choose different linker flags per platform.
- Linker flags cannot live only in dependency packages (e.g. [`example/mbt/moon.pkg`](example/mbt/moon.pkg)); downstream packages still need to repeat them manually (e.g. [`example/mbt/cmd/main/moon.pkg`](example/mbt/cmd/main/moon.pkg)).

Current example flow:

1. Build the C++ example library first (Qt).
2. Then link it from the MoonBit native target.

In `xmake.lua`, the `example` target copies build artifacts into the repository-level `build/` directory. On the MoonBit side, linking uses `-L../../build -lexample` plus Qt framework flags.

In theory, xmake could fully take over the MoonBit build flow to avoid part of these issues. This project does not use that approach for now, but interested users can explore it.

For non-macOS platforms, please figure out Qt dynamic library linking on your target platform and update linker flags in [`example/mbt/cmd/main/moon.pkg`](example/mbt/cmd/main/moon.pkg).

To run the example on macOS (with Homebrew installed):

```sh
brew install qt
xmake
cd example/mbt
moon run --target native cmd/main
```

## Example

The current demo is a Qt-based counter app. You can use it as a reference for how to use moonbridge.

## License

Apache-2.0.
