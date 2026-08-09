#!/bin/sh 

# Exit codes:
# 0  Success
# 1  General error
# 2  Required file not found
# 3  Failed to install executable
# 4  Failed to install man page
# 5  Failed to install license

: "${PREFIX:=/usr/local}"
: "${BINDIR:=${PREFIX}/bin}"
: "${MANDIR:=${PREFIX}/share/man}"
: "${LICENSEDIR:=${PREFIX}/share/licenses/randbit}"
: "${EXEC_NAME:=randbit}"

ANSI_RED="\033[31m"
ANSI_BOLD="\033[1m"
ANSI_ERR="${ANSI_RED}${ANSI_BOLD}"
ANSI_RESET="\033[0m"

print_styled() {
    printf "%b%s%b" "$1" "$2" "$ANSI_RESET"
}

println_styled() {
    printf "%b%s%b\n" "$1" "$2" "$ANSI_RESET"
}

println_err() {
    println_styled "$ANSI_ERR" "$1" >&2
}

if [ -n "$OUTDIR" ]; then
    : "${TARGET:=${OUTDIR}/${EXEC_NAME}}"
else 
    : "${TARGET:=${EXEC_NAME}}"
fi


failed=0

#For missing, required files.
if [ ! -f "$TARGET" ]; then
    println_err "Error: Executable file ($TARGET) not found."
    failed=1
fi

if [ ! -f "LICENSE" ]; then
    println_err "Error: LICENSE file (LICENSE) not found."
    failed=1
fi

if [ ! -f "randbit.1" ] ;then
    println_err "Error: Man page (randbit.1) not found."
    failed=1
fi

if [ $failed -ne 0 ]; then 
    exit 2
fi

# Installation

install -Dm755 "$TARGET" "${DESTDIR}${BINDIR}/${EXEC_NAME}"
exit_code=$?
if [ $exit_code -ne 0 ]; then
    println_err "Couldn't install executable."
    println_err "install returned with exit code $exit_code"
    exit 3
fi

install -Dm644 'randbit.1' "${DESTDIR}${MANDIR}/man1/randbit.1"
exit_code=$?
if [ $exit_code -ne 0 ]; then
    println_err "Couldn't install man page."
    println_err "install exited with exit code $exit_code"
    exit 4
fi

install -Dm644 'LICENSE' "${DESTDIR}${LICENSEDIR}/LICENSE"
exit_code=$?
if [ $exit_code -ne 0 ]; then
    println_err "Couldn't install license."
    println_err "install exited with exit code $exit_code"
    exit 5
fi

