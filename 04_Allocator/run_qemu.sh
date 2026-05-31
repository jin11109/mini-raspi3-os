#!/bin/bash
set -e

DEBUG=false
if [ $# == 1 ]; then
    if [ "$1" = "-debug" ]; then
        DEBUG=true
    fi
fi

# Run socat to create two pts devices
LOG_FILE=$(mktemp)
socat -d -d PTY,raw,echo=0 PTY,raw,echo=0 2> "$LOG_FILE" &
SOCAT_PID=$!

# Wait for socat
sleep 1

PORTS=($(grep -o "/dev/pts/[0-9]\+" "$LOG_FILE"))
PORT1="${PORTS[0]}"
PORT2="${PORTS[1]}"

if [[ -z "$PORT1" || -z "$PORT2" ]]; then
    echo "Can't build two pts devices"
    kill "$SOCAT_PID"
    exit 1
fi

echo "Build successfully: $PORT1 <--> $PORT2"
echo "QEMU will use $PORT1"
echo "You can first use below command to interact with QEMU"
echo "    $ screen $PORT2 115200"
echo "Press enter and continue QUMU"
read

if $DEBUG; then
    QEMU_DEBUG_OPTS="-S -gdb tcp::1234"
else
    QEMU_DEBUG_OPTS=""
fi

# Run QEMU
qemu-system-aarch64 \
    -M raspi3b \
    -kernel kernel8.img \
    -cpu cortex-a53 \
    -nographic \
    -initrd initramfs.cpio \
    -dtb bcm2710-rpi-3-b-plus.dtb \
    -serial null \
    -serial "$PORT1" \
    $QEMU_DEBUG_OPTS

kill $SOCAT_PID
rm -f "$LOG_FILE"
