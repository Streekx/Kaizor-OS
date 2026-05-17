# Kaizor OS ISO System (FreeBSD Base)

This folder contains the KaizorOS root filesystem and bootloader configs.

## Boot Flow

UEFI/BIOS Bootloader
→ FreeBSD Kernel
→ /etc/rc
→ /sbin/kaizor-init
→ kaizor-session
→ kaizor-shell
→ kaizor-compositor + kaizor-wm

## Notes

- BOOTX64.EFI is a placeholder.
- Real ISO build will be done using FreeBSD tools (makefs, mkimg).
- rootfs is designed to be merged into FreeBSD base.
