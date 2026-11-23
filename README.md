# HighPriest decompilation

C++ source for the Ragnarok Online HighPriest client (kRO, built 2008-11-05), reconstructed from the original executable and its program database. Compiled with the compilers of the time, it links to an executable that equals the original `HighPriest.exe` byte for byte:

```
SHA-256  9abc36c88376387d85c878c0c879d9af8ef66eb1f5152638085dc6995e3f4742
size     3,338,345 bytes (PE32, i386)
```

The source covers the game and the libraries the original linked statically: the Visual C++ runtime, Lua 5.0, the GameGuard client library, and the objects of the DirectX and Miles libraries. The DLLs the game imports are outside the scope.

## This branch: English translation

The `translation` branch puts the game into English: the text compiled into the executable, and the pictures and tables of the game data that the minigame shows. Its build no longer equals the original and `make check` fails by design; use `make`. `main` is the byte-identical source.

### Playing it

The [english-2026-11-23 release](https://github.com/malvarezcastillo/highpriest-decomp/releases/tag/english-2026-11-23) has the game ready to play on Windows (`HighPriest-English.zip`: unpack, run `HighPriest.exe`) and, for a game already installed from the original installer, the files that change (`HighPriest-English-patch.zip`). To make them from source:

```bash
make                                   # build/HighPriest.exe, in English
python3 translation/data.py GAME/data_hp.grf GAME/data_en.grf GAME/skin/default
cp build/HighPriest.exe GAME/
```

`GAME` is the folder the [original installer](https://github.com/malvarezcastillo/highpriest-decomp/releases/tag/installer-2008-11-05) creates. `data.py` needs Python 3 with Pillow. It writes `data_en.grf`, a small archive the translated client loads on top of `data_hp.grf` when it is there, so the game's own archive stays untouched. The repository holds only the translations and the scripts: every English file is made from its original in `data_hp.grf`.

### What is translated

- In the executable: the minigame's dialogue, menus, hints, shop, endings and credits; the client's built-in message table (`src/MsgStrings.cpp`); the interface, error and debug strings scattered through the other sources; and the crash dialog in `src/res/SimpleAssert.rc`.
- In `data_en.grf`: 155 interface pictures with text drawn into them (every button in each of its states, the bars of the Esc menu, the titles of the message and character windows, the tabs of the inventory, status and option windows and the labels of the option window); the names of all 565 skills; the descriptions of the 48 skills and the names and descriptions of the 56 items the minigame uses.

Names follow the international (iRO) client: Guillotine Fist, Occult Impaction, Mastersmith, Swordman, Hatii, Zeny, Blank Eyes. The emotion commands take their iRO forms (`/swt`, `/gg`, `/no1`). Where the minigame renamed a skill or an item the new name is translated instead: Aco Smash, Aco Kaboom, Aco Tornado, Grrr, Toy Chain.

### What stays Korean

- 2,727 literals in the source that are not text for the player. Most name files inside the game data (sprites, textures, sounds, palettes); the rest are the Korean chat commands, comparison keys, font names and GameGuard's encrypted strings. The author's credit line keeps his name in Hangul.
- Game data the minigame does not reach: the descriptions of the other items and skills of the full game, which its tables still carry; the monster and pet speech, map name and quest tables; and the pictures of screens it never opens (login, character select, mail, auction, the chat-room and shortcut windows, loading screens).
- The save slots that come with the installer, which carry Korean names.
- In `skin/default`, three pictures (`btn_edit*.bmp`) that differ from the archive's own.

The client still asks Windows for a Korean font (Gulim). Without any font that covers the Korean character set every string draws as boxes, the English ones too; a Windows with Korean language support, or a Wine prefix after `winetricks fakekorean`, has one.

### How it is made

`translation/strings/` lists every Korean string literal of `main` by source file and position, with its English text or the reason it is kept. `translation/tr.py apply` rewrites the literals from those tables, `translation/tr.py status` checks that every entry is accounted for and that printf conversions match, and `translation/tr.py extract` rebuilds the list from `main`.

`translation/images.py` redraws the pictures listed in `images.json`: it wipes the Korean label of the original and draws the English one in the same place and style, with the glyphs in `font.json`. `translation/tables.py` puts the text of `tables.json` into the game's name and description tables; the item descriptions are stored with letters mirrored, as the client expects. `translation/grf.py` reads the game's archive (version 0x102) and writes the new one (0x200), and `translation/data.py` runs them all.

Changes made by hand, because they are not string literals or needed room: the daughter's default name in `src/UIWindowMgr.cpp` (a list of character constants); the local text buffers of the `HP_*.cpp` sources (100 to 256 bytes); the save-slot difficulty buffer in `src/HP_SaveLoad.cpp`; the position of the difficulty value in the character window (`src/window/UIFrameWnd.cpp`); and the loading of `data_en.grf` in `src/WinMain.cpp`.

The English build has been played under Wine 9.0 from the title screen through character creation, the day menu, the inventory, equipment, skill and option windows, a battle and the shop. It has not been played through to an ending, and not yet run on Windows.

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
- `translation/`: the English text and the scripts that apply it (this branch only).
- `docs/`: the toolchain's provenance and the compiler notes.
- `build/`: outputs, untracked.

## Licence

The build files, `tools/stamp.py` and the documentation are dedicated to the public domain under [CC0 1.0](CC0-1.0.txt). Everything else belongs to its owners and is not covered: the reconstructed game source, the bundled compilers, SDK files and runtime sources, and the third-party libraries. [THIRD-PARTY.md](THIRD-PARTY.md) lists them with the notices their licences require.
