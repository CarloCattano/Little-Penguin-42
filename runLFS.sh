#!/bin/bash

# qemu-system-x86_64 -m 16G -enable-kvm \
  # -drive file=lfsbase.img,cache=none \
  # -netdev user,id=net0,hostfwd=tcp::2222-:22 \
  # -device e1000,netdev=net0 \
  # -smp $(nproc),threads=1,sockets=1 \
  # -cpu host

QEMU_CMD="qemu-system-x86_64"
QEMU_OPTS=(
  -m 16G
  -drive file=lfsbase.img,cache=none
  -netdev user,id=net0,hostfwd=tcp::2222-:22
  -device e1000,netdev=net0
  -smp $(nproc),threads=1,sockets=1
  -enable-kvm
  -cpu host
)

if [ "$1" == "vnc" ]; then
  echo "Starting QEMU with VNC support..."
else
  echo "Starting QEMU in headless mode..."
  QEMU_OPTS+=(-nographic -vga none)
fi

$QEMU_CMD "${QEMU_OPTS[@]}"

