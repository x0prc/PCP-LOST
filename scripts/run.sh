#!/bin/bash
set -e

echo "[*] Loading kernel module..."
sudo insmod kernel/pcp_probe.ko || true

echo "[*] Checking device..."
if [ ! -e /dev/pcp_probe ]; then
    echo "[-] Device not found!"
    exit 1
fi

echo "[*] Running experiment..."
sudo ./user/pcp_test | tee artifacts/logs.txt

echo "[+] Output saved to artifacts/logs.txt"
