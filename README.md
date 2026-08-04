# cliniti

A simple terminal initiative tracker for Dungeons & Dragons.

## Description

The thing is intended to be used in terminal for **easier and FOSSier** initiative tracking, cuz **existing** solutions are way **too bloated**.

**Usage:**
* Entries are **sorted by** their **initiative**.
* Use arrows to navigate.
* Enter - confirm
* Esc - go back

* An **entry** consists of 3 things:
    * Name
    * Initiative
    * HitPoints (not necessary to set)

* [**A**] - to add an entry.
* [**E**] - to edit an entry
* [**D**] - to delete an entry
* [**H**] - to **incr**ement/**decr**ement hits.
    * Just enter number to increment.
    * Enter number with '-' to decrement.
    * Remember, that hits can be undefined on entry creation? We introduce **negative hits!**\
    Just decrement hits and watch how damaged the thing is rn.
* [**M**] - to swap entries with **equal initiative**.
* [**Q**] - to **quit** program. **Achtung!! Session is RAM only**. All unsaved data will never be saved!

## Requirements

* C++17 compiler
* CMake ≥ 3.16
* GNU Make
* ncurses

## Build

```sh
make
```

## Run

```sh
./build/cliniti
```

Or install it system-wide:

```sh
sudo make install
```

After installation:

```sh
cliniti
```

## Debug build

```sh
make debug
```

## Clean

```sh
make clean
```

## Packaging (Arch Linux)

```sh
makepkg -si
```

## License

This project is licensed under the **GNU General Public License v2.0 (GPL-2.0)**.

See the `LICENSE` file for the full license text.

