# test script for handling ES100 scooter
# the script will connect to the ESC via COM port and send some test messages
import serial
import time
import sys
import struct
import binascii
import threading

# CRC-8/MAXIM-DOW
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
# Define messages to send to the ESC
# Each message is a list of bytes

messageOff = [0xA6, 0x12, 0x02, 0x10, 0x14, 0xCF]  # If the scooter is on turn it off.

messageA = [0xA6, 0x12, 0x02, 0x10, 0x14, 0xCF]  # MAKES HEADLIGHT BLINK ONCE

messageB = [0xA6, 0x12, 0x02, 0xE1, 0x14, 0x92]  # LIGHT OFF & ESC ON & MPH & SLOW
messageC = [0xA6, 0x12, 0x02, 0xE3, 0x14, 0x03]  # LIGHT Flash & ESC ON & MPH & SLOW
messageD = [0xA6, 0x12, 0x02, 0xE5, 0x14, 0xA9]  # LIGHT ON & ESC ON & MPH & SLOW

messageE = [0xA6, 0x12, 0x02, 0xE1, 0xE4, 0xE6]  # LIGHT OFF & ESC ON & MPH & FAST
messageF = [0xA6, 0x12, 0x02, 0xE3, 0xE4, 0x77]  # LIGHT Flash & ESC ON & MPH & Fast
messageG = [0xA6, 0x12, 0x02, 0xE5, 0xE4, 0xDD]  # LIGHT ON & ESC ON & MPH & FAST

messageH = [0xA6, 0x12, 0x02, 0xF1, 0x14, 0x7E]  # LIGHT OFF & ESC ON & KPH & SLOW
messageI = [0xA6, 0x12, 0x02, 0xF3, 0x14, 0xEF]  # LIGHT Flash & ESC ON & KPH & SLOW
messageJ = [0xA6, 0x12, 0x02, 0xF5, 0x14, 0x45]  # LIGHT ON & ESC ON & KPH & SLOW

messageK = [0xA6, 0x12, 0x02, 0xF1, 0xE4, 0x0A]  # LIGHT OFF & ESC ON & KPH & FAST
messageL = [0xA6, 0x12, 0x02, 0xF3, 0xE4, 0x9B]  # LIGHT Flash & ESC ON & KPH & Fast
messageM = [0xA6, 0x12, 0x02, 0xF5, 0xE4, 0x31]  # LIGHT ON & ESC ON & KPH & FAST

messageWORKING = [0xA6, 0x12, 0x02, 0x15, 0x14, 0x30]  # LIGHT ON & ESC ON & KPH & SLOW

# messageWORKING = [0xA6, 0x12, 0x02, 0x15, 0x14]  # LIGHT ON & ESC ON & KPH & SLOW


# Function to send a message to the ESC
def send_message(ser, message):
    # Convert message to bytearray
    message_bytes = bytearray(message)
    ser.write(message_bytes)
    # Print sent message in hex format  with | separators
    print(f"Sent: {'|'.join(f'{byte:02X}' for byte in message_bytes)}")  # Print sent message in hex format
    time.sleep(0.1)  # Short delay to ensure message is sent




if __name__ == "__main__":
    print("Test script for handling ES100 scooter")
    ser=serial.Serial('COM9',baudrate=9600,timeout=1)  # Adjust 'COM3' to your ESC's COM port
    while True:
        try:
            send_message(ser,messageWORKING)
            time.sleep(2)  # Wait before sending the next message
        except KeyboardInterrupt:
            print("Exiting...")
            send_message(ser, messageOff)  # Turn off the scooter before exiting
            ser.close()
            sys.exit(0)












