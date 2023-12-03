import serial
import time

def measure_uart_speed(start_time, end_time, data_size):
    elapsed_time = end_time - start_time
    bits_transmitted = data_size * 10  
    speed = bits_transmitted / elapsed_time
    return speed

# Function to receive data from MCU via UART
def receive_data_from_mcu(port, baud_rate):
    ser = serial.Serial(port, baud_rate)
    ser.timeout = 1
    
    start_time = time.time()
    received_data = ser.read(1000)
    end_time = time.time()
    
    speed = measure_uart_speed(start_time, end_time, len(received_data))
    print(f"Received Data: {received_data}")
    print(f"UART Speed: {speed} bits/second")
    
    ser.close()

def send_data_to_mcu(port, baud_rate, data):
    ser = serial.Serial(port, baud_rate)
    
    start_time = time.time()
    ser.write(data)
    end_time = time.time()
    
    speed = measure_uart_speed(start_time, end_time, len(data))
    print(f"UART Speed: {speed} bits/second")
    
    ser.close()

mcu_port = 'COMx' 
baud_rate = 2400
data_to_send = """Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan for predicting that the next banking crisis would be triggered by MSME lending, saying postmortem is easier than taking action when it was required. Rajan, who had as the chief economist at IMF warned of impending financial crisis of 2008, in a note to a parliamentary committee warned against ambitious credit targets and loan waivers, saying that they could be the sources of next banking crisis. Government should focus on sources of the next crisis, not just the last one.
In particular, government should refrain from setting ambitious credit targets or waiving loans. Credit targets are sometimes achieved by abandoning appropriate due diligence, creating the environment for future NPAs, Rajan said in the note. Both MUDRA loans as well as the Kisan Credit Card, while popular, have to be examined more closely for potential credit risk. Rajan, who was RBI governor for three years till September 2016, is currently."""

send_data_to_mcu(mcu_port, baud_rate, data_to_send)
receive_data_from_mcu(mcu_port, baud_rate)
