# randbit

A small cross-platform command-line utility for generating cryptographically secure random bits and integers.

## Features

- C23
- Linux, Windows, BSD, macOS
- Uses the operating system CSPRNG
- Minimal dependencies

## Installation

```bash
make
sudo make install
```

## Uninstallation

```bash
sudo make uninstall
```

## Usage

randbit
randbit --bool
randbit --coin
randbit --int
...

## Examples

$ randbit
1

$ randbit --coin
heads

...

## Building

```bash
make
make BUILD=debug
make TARGET\_IS\_WINDOWS=y
```

## Also visit
on unix-like systems:

```bash
man randbit #if installed
```
or
```bash
cat randbit.1
```

on windows:
open randbit.txt #(or randbit.1 if you cloned the repo)

on github:
open randbit.1


## License

MIT
