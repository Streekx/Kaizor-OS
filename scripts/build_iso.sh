#!/bin/sh

echo "====================================="
echo " KAIZOR ISO BUILDER"
echo "====================================="

mkdir -p release

makefs \
    -t cd9660 \
    -o rockridge \
    release/kaizor.iso \
    build/rootfs

echo "[DONE] ISO generated:"
echo "release/kaizor.iso"
