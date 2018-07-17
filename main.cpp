#include "MapleMini.h"
#include "mbed.h"
#include "CC1101.h"
#include "RingBuffer.h"

///////////////////////////////////////////////////
Ticker timer;
CC1100 cc1101;
DigitalOut led1(PB_1);  // timer blink led

DigitalOut r1(PB_10,1); // relay 1
DigitalOut r2(PB_2,1);  // relay 2
DigitalOut r3(PB_0,1);  // relay 3
DigitalOut r4(PA_7,1);  // relay 4

Serial pc(PA_2, PA_3); 

uint8_t Tx_fifo[FIFOBUFFER], Rx_fifo[FIFOBUFFER];
uint8_t My_addr = MY_ADDR;
uint8_t tx_addr = TG_ADDR; 
uint8_t rx_addr = My_addr;
uint8_t pktlen;
uint8_t lqi;
uint8_t Rssi;
uint8_t sender = tx_addr;
int8_t rssi_dbm = 0;
volatile uint8_t cc1101_packet_available;
Timer tick;

int main()
{
    cc1101.begin();
    cc1101.set_debug_level(0);
    cc1101.sidle();
    cc1101.spi_write_register(IOCFG2,0x06);
    cc1101.show_main_settings();             //shows setting debug messages to UART    
    cc1101.show_register_settings();         //shows current CC1101 register values  
    cc1101.receive();   
    tick.start();
    while(1)
    {
        if (cc1101.packet_available())       //checks if a packed is available
        {
          led1=!led1;
          cc1101.get_payload(Rx_fifo, pktlen, rx_addr, sender, rssi_dbm, lqi); //stores the payload data to Rx_fifo
          
          switch(Rx_fifo[4]){
            case 1:
                r1 = Rx_fifo[5];
                break;
            case 2:
                r2 = Rx_fifo[5];
                break;
            case 3:
                r3 = Rx_fifo[5];
                break;
            case 4:
                r4 = Rx_fifo[5];
                break;
            case 5:
                r1 = Rx_fifo[5];
                r2 = Rx_fifo[5];
                r3 = Rx_fifo[5];
                r4 = Rx_fifo[5];
                break;
          }
          pc.printf("Received: 0x%02X, 0x%02X, 0x%02X \r\n", Rx_fifo[3],Rx_fifo[4],Rx_fifo[5]);
        }
       // if(tick.read_ms()>=1000){
//            tick.reset();
//            
//            Tx_fifo[3] = 0x01;                    //split 32-Bit timestamp to 4 byte array
//            Tx_fifo[4] = 0x10;
//            Tx_fifo[5] = 0x1A;
//            Tx_fifo[6] = 0x2B;
//        
//            pktlen = 0x07;                                  
//                      
//            //cc1101.sent_packet(My_addr, TG_ADDR, Tx_fifo, pktlen, 40); 
//        }
    }
}
///////////////////////////////////////////////////
