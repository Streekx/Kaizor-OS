
#!/bin/sh

echo "====================================="
echo " KAIZOR OS ISO BUILD SCRIPT"
echo "====================================="

OUTDIR="out"
ROOTFS="rootfs"
BOOTLOADER="bootloader"

mkdir -p $OUTDIR

echo "[KAIZOR] Packing rootfs..."
tar -czf $OUTDIR/rootfs.tar.gz $ROOTFS

echo "[KAIZOR] Copying bootloader..."
cp -r $BOOTLOADER $OUTDIR/

echo "[KAIZOR] Done."
echo "NOTE: Real ISO build requires FreeBSD tools: makefs + mkimg"
echo "Output stored in iso/out/"
