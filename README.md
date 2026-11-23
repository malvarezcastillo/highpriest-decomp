# HighPriest decompilation

C++ source for the Ragnarok Online HighPriest client (kRO, built 2008-11-05), reconstructed from the original executable and its program database. Compiled with the compilers of the time, it links to an executable that equals the original `HighPriest.exe` byte for byte:

```
SHA-256  9abc36c88376387d85c878c0c879d9af8ef66eb1f5152638085dc6995e3f4742
size     3,338,345 bytes (PE32, i386)
```

The source covers the game and the libraries the original linked statically: the Visual C++ runtime, Lua 5.0, the GameGuard client library, and the objects of the DirectX and Miles libraries. The DLLs the game imports are outside the scope.

## This branch: English translation

The `translation` branch replaces the Korean text compiled into the executable with English, so its build no longer equals the original and `make check` fails by design; use `make`. `main` is the byte-identical source.

`translation/strings/` lists every Korean string literal of `main` by source file and position, with its English text or the reason it stays Korean (a file name inside the game data, a lookup key). `translation/tr.py apply` rewrites the literals from those tables, and `translation/tr.py status` counts what is still open. Names follow the international (iRO) client. Work in progress: see the status count.

## Building

```bash
make          # builds build/HighPriest.exe
make check    # builds it and compares its SHA-256 with the original's
make clean
```

A full build takes a few seconds on a many-core machine. It needs:

- Linux on x86-64 with glibc 2.38 and the libstdc++ of GCC 13, or newer (Ubuntu 24.04, Debian 13): the bundled `wibo` is linked against them. GNU Make, Bash and Python 3.
- Wine on the `PATH` (tested with 9.0), for `LINK.EXE` and `RC.EXE`. It uses the default prefix, `~/.wine`, and ignores `WINEPREFIX`; a first run creates the prefix (about 1.3 GB).
- A checkout whose absolute path has at most 154 characters and no spaces. The compilers order some symbols and sections by the length of the directory they run in, so every compile runs in a directory padded to a fixed length.

The compilers, the assembler and the librarian are in `tools/` and run under the bundled `tools/bin/wibo`. Nothing is downloaded.

## How the build works

`Makefile` holds the rules and `sources.mk` the lists: which compiler and flags each source gets, the members of each library and the order of the link command line.

1. Each source compiles in a directory of its own under `build/stage/`, with the clock fixed and a minimal environment. The original project compiled through an automatic precompiled header of `common.h`; four units build the way their objects show (three make the header, one reads it).
2. The statically linked libraries are archived from `src/` with their members in the stock libraries' order. The import libraries of `binkw32`, `mss32`, `granny2`, `ijl15` and `cps` are built from `src/imports/`.
3. `LINK.EXE` 6.00 links the objects in the original's order.
4. `tools/stamp.py` writes three values no build can repeat: the link time, and the signature, age and path of the program database that the image's debug record names. The `Makefile` gives them.

Three things in the image do not come from `src/`:

- `wlocale.obj` of `libcpmt.lib` is the stock library's member. No compile of its source gives the stock member's section order.
- The import records of the system DLLs and of DirectX come from the stock import libraries.
- The three stamped values above.

## Steering markers

Most functions match from ordinary source. In some, Visual C++ 6.0 makes a decision (which calls to inline, the order of commutative operands, the order of sections) from something the surviving evidence does not show, such as the number of declarations a lost header held. Those places carry a marker: a macro from one of the `src/rag_*.h` headers that changes that count and emits no code.

| Marker | Uses | Stands in for |
|---|---:|---|
| `RAG_BUDGET_FILL` | 41 | source that enlarged a function's inline budget |
| `RAG_BUDGET_SITES` | 15 | inline candidate call sites |
| `RAG_ENTROPY_SHIFT`, `RAG_ENTROPY_FILL_256` | 20 | declarations ahead of a function |
| `RAG_SYMBOL_SHIFT`, `RAG_SYMBOL_CHAIN` | 5 | symbols inside a function |

They are in 22 files; `grep -rlE 'RAG_(BUDGET|ENTROPY|SYMBOL)' src --exclude='rag_*.h'` lists them. Of the image's 7,546 procedures, 231 pieces of code (305,026 bytes) match only with a marker in effect. Each header explains its marker, and [docs/compiler-notes.md](docs/compiler-notes.md) records the compiler behaviour behind them.

## Layout

- `src/`: the reconstruction. `src/library/` holds the sources of statically linked libraries, `src/imports/` the definitions of the import libraries, `src/res/` the resources.
- `vendor/`: Visual C++ runtime sources that `src/library/` includes.
- `tools/`: the compilers and SDK files the build reads ([docs/toolchain.md](docs/toolchain.md)), and `tools/stamp.py`.
- `docs/`: the toolchain's provenance and the compiler notes.
- `build/`: outputs, untracked.

## Licence

The build files, `tools/stamp.py` and the documentation are dedicated to the public domain under [CC0 1.0](CC0-1.0.txt). Everything else belongs to its owners and is not covered: the reconstructed game source, the bundled compilers, SDK files and runtime sources, and the third-party libraries. [THIRD-PARTY.md](THIRD-PARTY.md) lists them with the notices their licences require.
