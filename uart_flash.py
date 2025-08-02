import sys

import serial

if len(sys.argv) != 3:
    print(
        "Use it like this: python3 uart_flash.py /dev/ttyACMX s32k148/build/dc33_fw_s32k148.srec"
    )
    sys.exit(1)

with open(sys.argv[2], "r") as f:
    lines = list(f)

with serial.Serial(sys.argv[1], baudrate=38400) as f:
    print("Waiting for bootloader...")
    f.reset_input_buffer()
    while f.read() != b"?":
        pass
    print("Bootloader connected, starting download:")
    for line in lines:
        f.write(line.encode() + b"\n")
        f.flush()
        c = f.read()
        print(c.decode(), end="", flush=True)
    c = f.read()
    if c == b"=":
        print("\nDownload complete")
