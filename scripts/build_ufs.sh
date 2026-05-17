#!/bin/sh

echo "====================================="
echo " KAIZOR UFS IMAGE BUILDER"
echo "====================================="

mkdir -p out

truncate -s 4G out/kaizor.img

mdconfig -f out/kaizor.img

newfs -U /dev/md0

mkdir -p mnt

mount /dev/md0 /mnt

cp -R build/rootfs/* /mnt

umount /mnt

mdconfig -d -u 0

echo "[DONE] UFS image created."
