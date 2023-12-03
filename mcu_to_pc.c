#include <LPC214x.h>

// Define UART Registers
#define RBR   0x00
#define THR   0x00
#define IER   0x04
#define IIR   0x08
#define FCR   0x08
#define LCR   0x0C
#define LSR   0x14

// Define EEPROM Registers
#define EEPROM_ADDR_START  0x00000000

// Define CRC Parameters
#define CRC_POLYNOMIAL 0x31

// Function to calculate CRC8
unsigned char calculateCRC8(const unsigned char *data, unsigned int size) {
    unsigned char crc = 0;
    for (unsigned int i = 0; i < size; ++i) {
        crc ^= data[i];
        for (unsigned int j = 0; j < 8; ++j) {
            crc = (crc & 0x80) ? (crc << 1) ^ CRC_POLYNOMIAL : (crc << 1);
        }
    }
    return crc;
}

// Function to send data to PC via UART
void sendDataToPC(const unsigned char *data, unsigned int size) {
    for (unsigned int i = 0; i < size; ++i) {
        // Transmit data
        while (!(U1LSR & 0x20));
        U1THR = data[i];
        
        // Calculate and transmit CRC8
        unsigned char crc = calculateCRC8(&data[i], 1);
        while (!(U1LSR & 0x20));
        U1THR = crc;
        
        // Delay for realistic speed measurement
        for (volatile int delay = 0; delay < 1000; ++delay);
    }
}

// Function to receive data from PC via UART
void receiveDataFromPC() {
    unsigned char buffer[1000];
    unsigned int index = 0;
    
    while (1) {
        // Receive data
        while (!(U1LSR & 0x01));
        buffer[index] = U1RBR;
        
        // Receive CRC
        while (!(U1LSR & 0x01));
        unsigned char receivedCRC = U1RBR;
        
        // Calculate CRC for received data
        unsigned char calculatedCRC = calculateCRC8(&buffer[index], 1);
        
        // Check CRC
        if (receivedCRC == calculatedCRC) {
            // Data is valid, store in EEPROM
            IAP_ENTRY(buffer, (void *)(EEPROM_ADDR_START + index), 100);
            index++;
        }
        
        // Check for end of transmission
        if (buffer[index - 1] == '\0') {
            break;
        }
    }
}

int main() {
    // Initialize UART
    PINSEL0 = 0x00050000;  // Configure TXD1 and RXD1 for UART1
    U1LCR = 0x83;         // 8 bits, no Parity, 1 Stop bit, DLAB = 1
    U1DLL = 78;           // 9600 Baud Rate @ 15MHz VPB Clock
    U1LCR = 0x03;         // DLAB = 0
    
    // Initialize EEPROM
    IAP_ENTRY(0, 0, 0);  // Initialize EEPROM
    
    // Send data to PC
    sendDataToPC("Finance Minister Arun Jaitley Tuesday hit out at former RBI governor Raghuram Rajan...", 1000);
    
    // Receive data from PC
    receiveDataFromPC();
    
    return 0;
}
