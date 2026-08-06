#!/bin/bash
set -e

make
echo "Compiled the OS"

make emulate-qemu
echo "Emulation successfully terminated"

make clean
echo "Cleaned the files"
