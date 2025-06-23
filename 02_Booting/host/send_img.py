import serial
import os
import time
from dotenv import load_dotenv
import tqdm

CRC32_POLY = 0xEDB88320
ACK = b'\x06'
NACK = b'\x15'

def crc32_update(crc: int, data_byte: int) -> int:
    crc ^= data_byte
    for _ in range(8):
        if crc & 1:
            crc = (crc >> 1) ^ CRC32_POLY
        else:
            crc >>= 1
    return crc

def crc32_calculate(data: bytes) -> int:
    crc = 0xFFFFFFFF
    for b in data:
        crc = crc32_update(crc, b)
    return (~crc) & 0xFFFFFFFF

def main():
    with open("../kernel8_pure.img", "rb") as f:
        payload = f.read()

    crc32 = crc32_calculate(payload)
    print(f"CRC32: 0x{crc32:08X}")

    # b'IMGX' is magic header
    packet = b'IMGX' + len(payload).to_bytes(4, 'little') + payload + crc32.to_bytes(4, 'little')
    print(f"kernel8_pure.img length: {len(payload)}B")

    load_dotenv('../../.env')
    port = os.environ.get('SERIAL_PORT_PATH')
    new_port = input('Input serial port path or press ENTER to use default path: ')
    if (new_port != ''):
        port = new_port
    else:
        print(f'default path: {port}')

    with serial.Serial(port=port, baudrate=115200) as tty:
        for byte in tqdm.tqdm(packet, desc="Sending", unit="B", ncols=80):
            tty.write(byte.to_bytes(1, 'little'))
            time.sleep(0.01)

        print("Waiting for response...")
        response = tty.read(1)
        if response == ACK:
            print("CRC verified by client: OK")
        elif response == NACK:
            print("CRC mismatch on client side")
        else:
            print(f"No response or unknown response {response}")

    print('Send img done')

if __name__ == "__main__":
    main()