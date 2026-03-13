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

Current example flow:

1. Build the C++ example library first (Qt).
2. Then link it from the MoonBit native target.

In `xmake.lua`, the `example` target copies build artifacts into the repository-level `build/` directory. On the MoonBit side, linking uses `-L../../build -lexample` plus Qt framework flags.

Interested users can try implementing a build script to pass linker flags following the method described in the [MoonBit documentation](https://docs.moonbitlang.com/zh-cn/latest/toolchain/moon/module.html#experimental-pre-build-config-script).

For platforms other than macOS, please figure out how to link Qt dynamic libraries on your target platform and adjust the linker flags accordingly.

To run the example on macOS (with Homebrew installed):

```sh
brew install qt
xmake
cd example/mbt
moon run --target native cmd/main
```

## Example

The current demo is a Qt-based counter app. You can use it as a reference for how to use moonbridge.

## Credits & Inspiration

This project is inspired by [moonbit-community/qpainter.mbt](https://github.com/moonbit-community/qpainter.mbt), created by illusory0x0 (猗露).  
That project is no longer being actively updated. Many thanks to the original author.

## License

Apache-2.0.
