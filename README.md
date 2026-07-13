# randbit

A small cross-platform command-line utility for generating cryptographically secure random bits and integers.

## Features

- C23
- Linux, Windows, BSD, macOS
- Uses the operating system CSPRNG
- Minimal dependencies

## Installation

make
sudo make install

## Uninstallation

sudo make uninstall

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

make
make BUILD=debug
make TARGET\_IS\_WINDOWS=y

## License

MIT
