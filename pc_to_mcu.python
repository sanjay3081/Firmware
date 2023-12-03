import serial
import time

text_to_send = """
Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one.

In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs," Rajan said in the note." Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently.
"""

# Open serial port
ser = serial.Serial('COMx', 2400, timeout=1)

# Transmit data with CRC
start_time = time.time()
crc_value = 0xFFFF

for char in text_to_send:
    # Send each character
    ser.write(char.encode())
    
    # Update CRC
    crc_value ^= ord(char)
    for _ in range(8):
        if crc_value & 0x0001:
            crc_value = (crc_value >> 1) ^ CRC16_POLY
        else:
            crc_value >>= 1

# Send null terminator and CRC
ser.write('\0'.encode())
ser.write(crc_value.to_bytes(2, 'big'))

# Close serial port
ser.close()

# Measure transmission speed
bits_transmitted = (len(text_to_send) + 3) * 8  # Include null terminator and CRC bytes
transmission_speed = bits_transmitted / (time.time() - start_time)
print(f"Transmission Speed: {transmission_speed:.2f} bits/second")
