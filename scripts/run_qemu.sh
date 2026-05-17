#!/bin/sh

echo "====================================="
echo " KAIZOR QEMU TEST"
echo "====================================="

qemu-system-x86_64 \
    -m 4096 \
    -cdrom release/kaizor.iso
