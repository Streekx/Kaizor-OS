#!/bin/sh

echo "====================================="
echo " KAIZOR ROOTFS PREPARATION"
echo "====================================="

mkdir -p build/rootfs

echo "[1] Extracting FreeBSD base..."

tar -xvf downloads/base.txz -C build/rootfs

echo "[2] Extracting FreeBSD kernel..."

tar -xvf downloads/kernel.txz -C build/rootfs

echo "[3] Copying Kaizor rootfs..."

cp -R iso/rootfs/* build/rootfs/

echo "[DONE] RootFS ready."
