#include <lpc214x.h>
#include <string.h>

#define DATA_SIZE 1000

unsigned short calculate_crc(char *data, int length) {
    unsigned short crc = 0;
    for (int i = 0; i < length; i++) {
        crc ^= (unsigned short)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000)
                crc = (crc << 1) ^ 0x1021;
            else
                crc <<= 1;
        }
    }
    return crc;
}

void init_uart() {
    PINSEL0 |= (1 << 4) | (1 << 6); 
    U0LCR = 0x83; 
    U0DLL = 78;   
    U0LCR = 0x03; 
}

void send_char(char ch) {
    while (!(U0LSR & 0x20)); 
    U0THR = ch; 
}

char read_char() {
    while (!(U0LSR & 0x01)); 
    return U0RBR; 
}

void send_crc(unsigned short crc) {
    send_char((char)(crc >> 8));
    send_char((char)crc);
}

unsigned short read_crc() {
    unsigned short crc = (read_char() << 8);
    crc |= read_char();
    return crc;
}

void receive_data_from_pc() {
    char received_text[DATA_SIZE];
    unsigned short received_crc;
    int data_index = 0;

    init_uart();

    while (data_index < DATA_SIZE) {
        char received_char = read_char();
        unsigned short received_char_crc = read_crc();
        unsigned short calculated_crc = calculate_crc(&received_char, 1);

        if (calculated_crc == received_char_crc) {
            received_text[data_index++] = received_char;
        } else {
        }
    }

    for (int i = 0; i < DATA_SIZE; i++) {
        EEPROM_write(i, received_text[i]);
    }
}

void send_data_to_pc() {
    char stored_data[DATA_SIZE];

    for (int i = 0; i < DATA_SIZE; i++) {
        stored_data[i] = EEPROM_read(i);
    }

    // Send the stored data to PC via UART
    for (int i = 0; i < DATA_SIZE; i++) {
        send_char(stored_data[i]);
        send_crc(calculate_crc(&stored_data[i], 1));
    }
}

int main() {
    receive_data_from_pc();
    send_data_to_pc();

    return 0;
}
