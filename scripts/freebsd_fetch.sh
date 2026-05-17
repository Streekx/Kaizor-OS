#!/bin/sh

echo "====================================="
echo " KAIZOR FREEBSD FETCHER"
echo "====================================="

mkdir -p downloads

echo "[1] Downloading FreeBSD base..."

fetch https://download.freebsd.org/releases/amd64/amd64/14.0-RELEASE/base.txz -o downloads/base.txz

echo "[2] Downloading FreeBSD kernel..."

fetch https://download.freebsd.org/releases/amd64/amd64/14.0-RELEASE/kernel.txz -o downloads/kernel.txz

echo "[DONE] FreeBSD packages downloaded."
