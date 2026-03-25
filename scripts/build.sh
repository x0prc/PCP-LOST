#!/bin/bash
set -e

echo "[*] Building kernel module..."
cd kernel
make

echo "[*] Building userland..."
cd ../user
make

echo "[+] Build complete"
