#include <LPC214x.h>

#define RBR   0x00
#define THR   0x00
#define IER   0x04
#define IIR   0x08
#define FCR   0x08
#define LCR   0x0C
#define LSR   0x14

#define EEPROM_ADDR_START  0x00000000
#define CRC_POLYNOMIAL 0x31

unsigned char calculateCRC8(const char *data,int length) 
{
   char crc = 0x00;
   char extract;
   char sum;
   for(int i=0;i<length;i++)
   {
      extract = *data;
      for (char tempI = 8; tempI; tempI--) 
      {
         sum = (crc ^ extract) & 0x01;
         crc >>= 1;
         if (sum)
            crc ^= 0x8C;
         extract >>= 1;
      }
      data++;
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

void receiveDataFromPC() {
    unsigned char buffer[1000];
    unsigned int index = 0;
    
    while (1) {
        
        while (!(U1LSR & 0x01));
        buffer[index] = U1RBR;
        
        while (!(U1LSR & 0x01));
        unsigned char receivedCRC = U1RBR;
        
        unsigned char calculatedCRC = calculateCRC8(&buffer[index], 1);
        
        if (receivedCRC == calculatedCRC) {
            IAP_ENTRY(buffer, (void *)(EEPROM_ADDR_START + index), 100);
            index++;
        }
        
        if (buffer[index - 1] == '\0') {
            break;
        }
    }
}

int main() {
    PINSEL0 = 0x00050000;  // UART1
    U1LCR = 0x83;      
    U1DLL = 78;        
    U1LCR = 0x03;        
    
    IAP_ENTRY(0, 0, 0);  
    
    receiveDataFromPC();
    
    unsigned char buffer[1000];
    for (unsigned int i = 0; i < 1000; ++i) {
        buffer[i] = IAP_ENTRY(0, (void *)(EEPROM_ADDR_START + i), 1);
    }
    sendDataToPC(buffer, 1000);
    
    return 0;
}
