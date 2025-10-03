import serial
import time
import sys

# ---------------- CRC8 (Maxim/Dallas) ----------------
def crc8_maxim(data: bytes) -> int:
    crc = 0x00
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 0x01:
                crc = (crc >> 1) ^ 0x8C  # polynomial x^8 + x^5 + x^4 + 1 reversed
            else:
                crc >>= 1
    return crc & 0xFF

# ---------------- Flags ----------------
firstdigit = 1
seconddigit = 0
fastAcceleration = 0
kph = 1
fifthdigit = 0
Light = 0
LightBlink = 0
ESCOn = 1
SpeedLimit = 228  # 0x14 = 20 (slow), 0xE4 = 228 (fast)

# ---------------- Calculate forth byte ----------------
def calculate_forth() -> int:
    forth = 0
    if firstdigit:       forth += 128
    if seconddigit:      forth += 64
    if fastAcceleration: forth += 32
    if kph:              forth += 16
    if fifthdigit:       forth += 8
    if Light:            forth += 4
    if LightBlink:       forth += 2
    if ESCOn:            forth += 1
    return forth

# ---------------- Build buffer ----------------
forth = calculate_forth()
buf = bytearray([0xA6, 0x12, 0x02, forth, SpeedLimit, 0x00])
buf[5] = crc8_maxim(buf[:5])

messageOff = [0xA6, 0x12, 0x02, 0x10, 0x14, 0xCF]  # If the scooter is on turn it off.

# ---------------- Serial communication ----------------

if __name__ == "__main__":
    print("Test script for handling ES100 scooter")
    ser=serial.Serial('COM9',baudrate=9600,timeout=1)  # Adjust 'COM3' to your ESC's COM port
    while True:
        try:
            print("|".join(f"{b:02X}" for b in buf))
            ser.write(buf)
            time.sleep(2)  # Wait before sending the next message
        except KeyboardInterrupt:
            print("Exiting...")
            ser.write(bytearray(messageOff))  # Turn off the scooter before exiting
            # ser.close()
            sys.exit(0)