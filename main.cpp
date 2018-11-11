
#include "CC1101.h"

#define RF_0db
//#define RF_10db
///////////////////////////////////////////////////////////////////////////////////////
CC1101::CC1101(PinName mosi, PinName miso, PinName clk, PinName csn, PinName RDmiso):
    _spi(mosi, miso, clk), _csn(csn),  _RDmiso(RDmiso)
{
    _csn = 1;

    // Setup the spi for 8 bit data, high steady state clock,
    // second edge capture, with a 1MHz clock rate
    _spi.format(8,0);
    _spi.frequency(1000000);

    POWER_UP_RESET_CCxxx0();
    Strobe(CCxxx0_SRX);
}
///////////////////////////////////////////////////////////////////////////////////////
RF_SETTINGS rfSettings =        // 433Mhz, 9600Bauds
{
    0x06,  // FSCTRL1       Frequency Synthesizer Control - IF:152.343Khz
    0x07,  // IOCFG0        GDO0 Output Pin Configuration - Packet received and CRC OK
    0x00,  // FSCTRL0       Frequency Synthesizer Control - Freq offset
    0x10,  // FREQ2         Frequency Control Word, High Byte - 433.999 Mhz
    0xB1,  // FREQ1         Frequency Control Word, Middle Byte
    0x3B,  // FREQ0         Frequency Control Word, Low Byte
    0xF8,  // MDMCFG4       Modem Configuration - BW: 58.035Khz
    0x83,  // MDMCFG3       Modem Configuration - 9595 Baud
    0x13,  // MDMCFG2       Modem Configuration - 30/32 sync word bits - Manchester disable - GFSK - Digital DC filter enable
    0x22,  // MDMCFG1       Modem Configuration - num of preamble bytes:4 - FEC disable
    0xF8,  // MDMCFG0       Modem Configuration - Channel spacing: 199.951Khz
    0x00,  // CHANNR        Channel Number
    0x15,  // DEVIATN       Modem Deviation Setting - 5.157Khz
    0x56,  // FREND1        Front End RX Configuration 
    0x10,  // FREND0        Front End TX Configuration
    0x18,  // MCSM0         Main Radio Control State Machine Configuration - PO timeout: 64(149-155us) - Auto calibrate from idle to rx/tx
    0x16,  // FOCCFG        Frequency Offset Compensation Configuration
    0x6C,  // BSCFG         Bit Synchronization Configuration
    0x03,  // AGCCTRL2      AGC Control - target amplitude: 33dB - Maximum possible LNA + LNA 2 gain - All gain settings can be used
    0x40,  // AGCCTRL1      AGC Control - LNA gain decreased first
    0x91,  // AGCCTRL0      AGC Control - Medium hysterisis - Filter Samples: 16 - Normal AGC operation 
    0xE9,  // FSCAL3        Frequency Synthesizer Calibration
    0x2A,  // FSCAL2        Frequency Synthesizer Calibration
    0x00,  // FSCAL1        Frequency Synthesizer Calibration
    0x1F,  // FSCAL0        Frequency Synthesizer Calibration
    0x59,  // FSTEST        Frequency Synthesizer Calibration Control
    0x88,  // TEST2         Various Test Settings
    0x31,  // TEST1         Various Test Settings
    0x09,  // TEST0         Various Test Settings
    0x07,  // FIFOTHR       RX FIFO and TX FIFO Thresholds - Bytes in TX FIFO:33 - Bytes in RX FIFO:32
    0x06,  // IOCFG2        GDO2 Output Pin Configuration - Sync word received/sent - end of packet
    0x04,  // PKTCTRL1      Packet Automation Control - No address check - Automatic flush of RX FIFO is disable - sync word is always accepted
    0x05,  // PKTCTRL0      Packet Automation Control - whitening is off - RX/TX data normal mode - CRC calculation in TX and CRC check in RX - Variable packet length
    0x00,  // ADDR          Device Address 
    0xFF,  // PKTLEN        Packet Length
    0x3F,  // MCSM1         Main Radio Control State Machine Configuration
};
#ifdef RF_0db
// PATABLE (0 dBm output power)
char paTable[] = {0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
#ifdef RF_10db
// PATABLE (10 dBm output power)
char paTable[] = {0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
#endif
///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// Macro to reset the CCxxx0 and wait for it to be ready
void CC1101::RESET_CCxxx0(void)
{
//  while (_RDmiso);
  _csn = 0;
    wait(0.000002);
  while (_RDmiso);
  _spi.write(CCxxx0_SRES);
    wait(0.000002);
  _csn = 1;
}
///////////////////////////////////////////////////////////////////////////////////////
// Macro to reset the CCxxx0 after power_on and wait for it to be ready
// IMPORTANT NOTICE:
// The file Wait.c must be included if this macro shall be used
// The file is located under: ..\Lib\Chipcon\Hal\CCxx00
//
//                 min 40 us
//             <----------------------->
// CSn      |--|  |--------------------|          |-----------
//          |  |  |                    |          |
//              --                      ----------
//
// MISO                                       |---------------
//          - - - - - - - - - - - - - - - -|  |
//                                          --
//               Unknown / don't care
//
// MOSI     - - - - - - - - - - - - - - - ---------- - - - - -
//                                         | SRES |
//          - - - - - - - - - - - - - - - ---------- - - - - -
//
void CC1101::POWER_UP_RESET_CCxxx0(void)
{
  _csn = 1;
  wait(1e-6);
  _csn = 0;
  wait(1e-6);
  _csn = 1;
  wait(41e-6);
  RESET_CCxxx0();
}
///////////////////////////////////////////////////////////////////////////////////////
//  void Strobe(unsigned char strobe)
//
//  DESCRIPTION:
//      Function for writing a strobe command to the CCxxx0
//
//  ARGUMENTS:
//      unsigned char strobe
//          Strobe command
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::Strobe(unsigned char strobe)
{
    unsigned char x;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    x = _spi.write(strobe);
    wait(0.000002);
    _csn = 1;
    return x;
}// Strobe
///////////////////////////////////////////////////////////////////////////////////////
//  unsigned char ReadStatus(unsigned char addr)
//
//  DESCRIPTION:
//      This function reads a CCxxx0 status register.
//
//  ARGUMENTS:
//      unsigned char addr
//          Address of the CCxxx0 status register to be accessed.
//
//  RETURN VALUE:
//      unsigned char
//          Value of the accessed CCxxx0 status register.
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::ReadStatus(unsigned char addr)
{
    unsigned char x;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    _spi.write(addr | READ_BURST);
    x = _spi.write(0);
    wait(0.000002);
    _csn = 1;
    return x;
}// ReadStatus
///////////////////////////////////////////////////////////////////////////////////////
//  void WriteRfSettings(RF_SETTINGS *pRfSettings)
//
//  DESCRIPTION:
//      This function is used to configure the CCxxx0 based on a given rf setting
//
//  ARGUMENTS:
//      RF_SETTINGS *pRfSettings
//          Pointer to a struct containing rf register settings
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::WriteRfSettings(RF_SETTINGS *pRfSettings)
{
    // Write register settings
    WriteReg(CCxxx0_FSCTRL1,  pRfSettings->FSCTRL1);
    WriteReg(CCxxx0_FSCTRL0,  pRfSettings->FSCTRL0);
    WriteReg(CCxxx0_FREQ2,    pRfSettings->FREQ2);
    WriteReg(CCxxx0_FREQ1,    pRfSettings->FREQ1);
    WriteReg(CCxxx0_FREQ0,    pRfSettings->FREQ0);
    WriteReg(CCxxx0_MDMCFG4,  pRfSettings->MDMCFG4);
    WriteReg(CCxxx0_MDMCFG3,  pRfSettings->MDMCFG3);
    WriteReg(CCxxx0_MDMCFG2,  pRfSettings->MDMCFG2);
    WriteReg(CCxxx0_MDMCFG1,  pRfSettings->MDMCFG1);
    WriteReg(CCxxx0_MDMCFG0,  pRfSettings->MDMCFG0);
    WriteReg(CCxxx0_CHANNR,   pRfSettings->CHANNR);
    WriteReg(CCxxx0_DEVIATN,  pRfSettings->DEVIATN);
    WriteReg(CCxxx0_FREND1,   pRfSettings->FREND1);
    WriteReg(CCxxx0_FREND0,   pRfSettings->FREND0);
    WriteReg(CCxxx0_MCSM0 ,   pRfSettings->MCSM0 );
    WriteReg(CCxxx0_FOCCFG,   pRfSettings->FOCCFG);
    WriteReg(CCxxx0_BSCFG,    pRfSettings->BSCFG);
    WriteReg(CCxxx0_AGCCTRL2, pRfSettings->AGCCTRL2);
    WriteReg(CCxxx0_AGCCTRL1, pRfSettings->AGCCTRL1);
    WriteReg(CCxxx0_AGCCTRL0, pRfSettings->AGCCTRL0);
    WriteReg(CCxxx0_FSCAL3,   pRfSettings->FSCAL3);
    WriteReg(CCxxx0_FSCAL2,   pRfSettings->FSCAL2);
    WriteReg(CCxxx0_FSCAL1,   pRfSettings->FSCAL1);
    WriteReg(CCxxx0_FSCAL0,   pRfSettings->FSCAL0);
    WriteReg(CCxxx0_FSTEST,   pRfSettings->FSTEST);
    WriteReg(CCxxx0_TEST2,    pRfSettings->TEST2);
    WriteReg(CCxxx0_TEST1,    pRfSettings->TEST1);
    WriteReg(CCxxx0_TEST0,    pRfSettings->TEST0);
    WriteReg(CCxxx0_FIFOTHR,  pRfSettings->FIFOTHR);
    WriteReg(CCxxx0_IOCFG2,   pRfSettings->IOCFG2);
    WriteReg(CCxxx0_IOCFG0,   pRfSettings->IOCFG0);
    WriteReg(CCxxx0_PKTCTRL1, pRfSettings->PKTCTRL1);
    WriteReg(CCxxx0_PKTCTRL0, pRfSettings->PKTCTRL0);
    WriteReg(CCxxx0_ADDR,     pRfSettings->ADDR);
    WriteReg(CCxxx0_PKTLEN,   pRfSettings->PKTLEN);
    WriteReg(CCxxx0_MCSM1 ,   pRfSettings->MCSM1 );
    RXMode();

}// WriteRfSettings
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::init(void)
{
    WriteRfSettings(&rfSettings);
    WriteReg(CCxxx0_PATABLE, paTable[0]);
}
///////////////////////////////////////////////////////////////////////////////////////
//  unsigned char ReadReg(unsigned char addr)
//
//  DESCRIPTION:
//      This function gets the value of a single specified CCxxx0 register.
//
//  ARGUMENTS:
//      unsigned char addr
//          Address of the CCxxx0 register to be accessed.
//
//  RETURN VALUE:
//      unsigned char
//          Value of the accessed CCxxx0 register.
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::ReadReg(unsigned char addr)
{
    unsigned char x;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    _spi.write(addr | READ_SINGLE);
    x = _spi.write(0);
    wait(0.000002);
    _csn = 1;
    return x;
}// ReadReg
///////////////////////////////////////////////////////////////////////////////////////
//  void ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
//
//  DESCRIPTION:
//      This function reads multiple CCxxx0 register, using SPI burst access.
//
//  ARGUMENTS:
//      unsigned char addr
//          Address of the first CCxxx0 register to be accessed.
//      unsigned char *buffer
//          Pointer to a byte array which stores the values read from a
//          corresponding range of CCxxx0 registers.
//      unsigned char count
//          Number of bytes to be read from the subsequent CCxxx0 registers.
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::ReadBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
{
    unsigned char i;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    _spi.write(addr | READ_BURST);
    for (i = 0; i < count; i++)
    {
        buffer[i] = _spi.write(0);
    }
    wait(0.000002);
    _csn = 1;
}// ReadBurstReg
///////////////////////////////////////////////////////////////////////////////////////
//  void WriteReg(unsigned char addr, unsigned char value)
//
//  DESCRIPTION:
//      Function for writing to a single CCxxx0 register
//
//  ARGUMENTS:
//      unsigned char addr
//          Address of a specific CCxxx0 register to accessed.
//      unsigned char value
//          Value to be written to the specified CCxxx0 register.
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::WriteReg(unsigned char addr, unsigned char value)
{
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    _spi.write(addr);
    _spi.write(value);
    wait(0.000002);
    _csn = 1;
}// WriteReg
///////////////////////////////////////////////////////////////////////////////////////
//  void WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
//
//  DESCRIPTION:
//      This function writes to multiple CCxxx0 register, using SPI burst access.
//
//  ARGUMENTS:
//      unsigned char addr
//          Address of the first CCxxx0 register to be accessed.
//      unsigned char *buffer
//          Array of bytes to be written into a corresponding range of
//          CCxx00 registers, starting by the address specified in _addr_.
//      unsigned char count
//          Number of bytes to be written to the subsequent CCxxx0 registers.
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::WriteBurstReg(unsigned char addr, unsigned char *buffer, unsigned char count)
{
    unsigned char i;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    _spi.write(addr | WRITE_BURST);
    for (i = 0; i < count; i++)
    {
        _spi.write(buffer[i]);
    }
    wait(0.000002);
    _csn = 1;
}// WriteBurstReg
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::RdRSSI(void)
{
    unsigned char crssi;
    
    
    if (rssi >= 128)
    {
        crssi = 255 - rssi;
        crssi /= 2;
        crssi += 74;
    }
    else
    {
        crssi = rssi/2;
        crssi += 74;
    }
    return crssi;
}
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::RdLQI(void)
{
    unsigned char clqi;
    clqi = 0x3F - (lqi & 0x3F);
    
    return clqi;
}
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::RxFifoEmpty(void)
{
    unsigned char RxFifoStatus;

    Strobe(CCxxx0_SRX);
    RxFifoStatus = ReadStatus(CCxxx0_RXBYTES);
    
    if (RxFifoStatus & 0x80)      // check for RXFIFO overflow 
    {
        // Make sure that the radio is in IDLE state before flushing the FIFO
        // (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point)
        Strobe(CCxxx0_SIDLE);

        // Flush RX FIFO
        Strobe(CCxxx0_SFRX);
    }
    if (RxFifoStatus & ~0x80)
    {
        return 0;
    }
    else
        return 1;
}
///////////////////////////////////////////////////////////////////////////////////////
//-------------------------------------------------------------------------------------------------------
//  BOOL ReceivePacket(unsigned char *rxBuffer, unsigned char *length)
//
//  DESCRIPTION:
//      This function can be used to receive a packet of variable packet length (first byte in the packet
//      must be the length byte). The packet length should not exceed the RX FIFO size.
//
//  ARGUMENTS:
//      unsigned char *rxBuffer
//          Pointer to the buffer where the incoming data should be stored
//      unsigned char *length
//          Pointer to a variable containing the size of the buffer where the incoming data should be
//          stored. After this function returns, that variable holds the packet length.
//
//  RETURN VALUE:
//      BOOL
//          1:   CRC OK
//          0:  CRC NOT OK (or no packet was put in the RX FIFO due to filtering)
///////////////////////////////////////////////////////////////////////////////////////

int CC1101::ReceivePacket(unsigned char *rxBuffer, unsigned char *length)
{
  unsigned char status[2];
  unsigned char packetLength;

    packetLength = ReadStatus(CCxxx0_RXBYTES);
    if (packetLength & BYTES_IN_RXFIFO)
    {
        // Read length byte
        packetLength = ReadReg(CCxxx0_RXFIFO);

        // Read data from RX FIFO and store in rxBuffer
        if (packetLength <= *length)
        {
            ReadBurstReg(CCxxx0_RXFIFO, rxBuffer, packetLength);
            *length = packetLength;
 
            // Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI)
            ReadBurstReg(CCxxx0_RXFIFO, status, 2);

            rssi = status[RSSI];
            lqi = status[LQI];
            // MSB of LQI is the CRC_OK bit
//            return (status[LQI] & CRC_OK);
            if(status[LQI] & CRC_OK)
            {
                    return 1;
            }
        }
        else
        {
            *length = packetLength;

            // Make sure that the radio is in IDLE state before flushing the FIFO
            // (Unless RXOFF_MODE has been changed, the radio should be in IDLE state at this point)
            Strobe(CCxxx0_SIDLE);

            // Flush RX FIFO
            Strobe(CCxxx0_SFRX);
            return 0;
        }
    } else
        return 0;
  return 0;
}// halRfReceivePacket
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::TxFifoEmpty(void)
{
    unsigned char TxFifoStatus;

    Strobe(CCxxx0_STX);
    TxFifoStatus = ReadStatus(CCxxx0_TXBYTES);
    
    if (TxFifoStatus & 0x80)      // check for TXFIFO underflow 
    {
        // Make sure that the radio is in IDLE state before flushing the FIFO
         Strobe(CCxxx0_SIDLE);

        // Flush TX FIFO
        Strobe(CCxxx0_SFTX);
    }
    if (TxFifoStatus & ~0x80)
    {
        return 0;
    }
    else
        return 1;
}
///////////////////////////////////////////////////////////////////////////////////////
//  void halRfSendPacket(unsigned char *txBuffer, unsigned char size)
//
//  DESCRIPTION:
//      This function can be used to transmit a packet with packet length up to 63 bytes.
//
//  ARGUMENTS:
//      unsigned char *txBuffer
//          Pointer to a buffer containing the data that are going to be transmitted
//
//      unsigned char size
//          The size of the txBuffer
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::SendPacket(unsigned char *txBuffer, unsigned char size)
{
    unsigned char i;

    for (i = size; i > 0; i--)
      txBuffer[i] = txBuffer[i-1];  
    txBuffer[0] = size;

    WriteBurstReg(CCxxx0_TXFIFO, txBuffer, size+1);
    Strobe(CCxxx0_SIDLE);
    Strobe(CCxxx0_STX);
}// halRfSendPacket
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::ReadChipStatusTX(void)
{
    unsigned char x;

    x = Strobe(CCxxx0_SNOP);
    return x;
}
///////////////////////////////////////////////////////////////////////////////////////
unsigned char CC1101::ReadChipStatusRX(void)
{
    unsigned char x;
    wait(0.000005);
    _csn = 0;
    wait(0.000002);
    while (_RDmiso);
    x = _spi.write(CCxxx0_PARTNUM | READ_BURST);
    wait(0.000002);
    _csn = 1;
    return x;
}
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::FlushRX(void)
{
    // Make sure that the radio is in IDLE state before flushing the FIFO
    Strobe(CCxxx0_SIDLE);

    // Flush RX FIFO
    Strobe(CCxxx0_SFRX);
}
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::FlushTX(void)
{
    // Make sure that the radio is in IDLE state before flushing the FIFO
    Strobe(CCxxx0_SIDLE);

    // Flush TX FIFO
    Strobe(CCxxx0_SFTX);
}
///////////////////////////////////////////////////////////////////////////////////////
void CC1101::RXMode(void)
{
    Strobe(CCxxx0_SIDLE);
    Strobe(CCxxx0_SRX);
}
///////////////////////////////////////////////////////////////////////////////////////
