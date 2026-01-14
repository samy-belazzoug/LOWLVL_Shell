#!/bin/bash
# Build script for LOWLVL Shell

CC="gcc"
CFLAGS="-Wall -Wextra -g -std=c99 -D_POSIX_C_SOURCE=200809L"
TARGET="bashr"

case "${1:-}" in
    clean)
        echo "Cleaning build artifacts..."
        rm -f src/*.o "${TARGET}"
        echo "Done"
        ;;
    test)
        if [ ! -f "${TARGET}" ]; then
            echo "Building first..."
            bash build.sh
        fi
        echo ""
        echo "Running tests..."
        echo "cd /tmp" | ./"${TARGET}" 2>&1 | head -3 || true
        echo "Test executed"
        ;;
    help)
        echo "LOWLVL Shell Build targets:"
        echo "  ./build.sh       - Build the shell"
        echo "  ./build.sh clean - Remove artifacts"
        echo "  ./build.sh test  - Run tests"
        echo "  ./build.sh help  - Show help"
        ;;
    *)
        echo "Building LOWLVL Shell..."
        ${CC} ${CFLAGS} -c src/bashr.c -o src/bashr.o
        ${CC} ${CFLAGS} -c src/executor.c -o src/executor.o
        ${CC} ${CFLAGS} -c src/builtins.c -o src/builtins.o
        ${CC} ${CFLAGS} -c src/utils.c -o src/utils.o
        ${CC} ${CFLAGS} -o "${TARGET}" src/*.o
        echo "Built ${TARGET}"
        ;;
esac
