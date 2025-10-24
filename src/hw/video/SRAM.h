#ifndef _SRAM_H_
#define _SRAM_H_
#include "Arduino.h"
#include "hw/Pins.h"

#define SRAM_SIZE (unsigned long)1048576 //20 bit address

#define BUFFER_SIZE 1 << 10
#define RETRY_COUNT 3
#define ERASE_BYTE (uint8_t)0 //value denoting an erased byte
enum BusyType{
    btInvalid = 0,
    btVertical = 1,     // vertical break, suitable for longer writes
    btHorizontal = 2,   // draws on horizontal break. suitable for short burst, few pixels    
    btAny = 3,          // horizontal and vertical breaks from showing screen
    btVolatile = 4      // writes ignoring vga output state. will likely result in video signal glitching while writing    
};
enum DeviceState {
	dsOff = 0,
	dsOutput = 1,
	dsWrite = 2
};



class SRAM
{
public:
	SRAM();
	~SRAM();

    void begin();

    // bool inline Busy(BusyType busyType = btAny){
    //     return busyType == btVolatile ? false :
    //         busyType == btVertical ? 
    //             !(PIOA->PIO_PDSR & PIO_PA15) : 
    //             busyType == btHorizontal ?
    //                 (PIOA->PIO_PDSR & PIO_PA14) :
    //                 !(PIOB->PIO_PDSR & PIO_PB26);
    //     //return (PIOA->PIO_PDSR & PIO_PA15); //horizontal screen clear, 3.9us
    //     //return !(PIOD->PIO_PDSR & PIO_PD5); //vertical screen clear, 1.6ms
    //     //return (PIOB->PIO_PDSR & PIO_PB26); // screen output (active low)
    // }


	
    
	//MAX ADDR is 2048 with 11 address lines

	//construct byte from data bits
	uint8_t ReadByte(uint32_t addr);
    size_t ReadBytes(uint32_t addr, uint8_t* buffer, uint32_t length, BusyType busyType = btAny);
    uint16_t ReadShort(uint32_t addr);
    size_t ReadBytes(uint32_t addr, uint16_t* buffer, uint32_t length, BusyType busyType = btAny);
    size_t ReadString(uint32_t addr, uint8_t* buffer, uint32_t length);
	
	// void WriteFirstByte(uint8_t data, uint16_t offsetAddress = 0);
	// void WriteNextByte(uint8_t data);
	bool WriteByte(uint32_t addr, uint8_t data, uint8_t retryCount = RETRY_COUNT,BusyType busyType = btAny);
    virtual uint16_t WriteBytes(uint32_t addr, uint8_t* data, uint32_t length, BusyType busyType = btAny);
    //bool Erase(uint32_t startAddress = 0x0, uint32_t length = SRAM_SIZE);

private:
	uint16_t counter = 0;
	uint16_t _retries = 0;   

    void DeviceOff();
	void DeviceOutput();
	void DeviceWrite();

protected:
   
	bool WriteShort(uint32_t addr, uint16_t data, BusyType busyType = btAny);
    
    uint16_t FillBytes(uint32_t startAddr, uint8_t data, uint32_t length, BusyType busyType = btAny);

    void inline SetAddress(uint32_t addr){
        
        PIOD->PIO_PER = 0x7FF;         // address bits 0 - 10 //enable
        PIOD->PIO_OER = 0x7FF;         //set as output

        REG_PIOD_CODR = 0x7FF;          // 11 bits
        REG_PIOD_SODR = (addr & 0x7FF);

        REG_PIOC_CODR = 0x1FF << 1;     // 9 bits
        REG_PIOC_SODR = ((addr >> 11) & 0x1FF) << 1;
    }

    // void inline SetRow(uint32_t row){
    //     //set bits 11 - 19, y pixel
    //     REG_PIOD_CODR = 0x1 << 10;
    //     REG_PIOD_SODR = (row & 0x1) << 10;

    //     REG_PIOC_CODR = 0x1FF << 1;
    //     REG_PIOC_SODR = (row & 0x1FF) << 1;

    // }
    void inline SetCol(uint32_t addr)
    {
    //lower address bits 0-9
        REG_PIOD_CODR = 0x3FF;
        REG_PIOD_SODR = (addr & 0x3FF);
    }
    void inline SetDataLines(uint8_t data){
        REG_PIOC_CODR = 0xFF << 12;
        REG_PIOC_SODR = (data & 0xFF )<< 12;
    }
	void inline SetDataLines(uint16_t data)
    {
    #ifdef USE_PORT_IO
        PORTL = data;
    #else
        REG_PIOC_CODR = 0xFF << 12;
        REG_PIOC_SODR = (data & 0xFF )<< 12;
        REG_PIOB_CODR = 0xFF << 14;
        REG_PIOB_SODR = (data >> 8) << 14;
    #endif
    }

	void BinToSerial(uint8_t var);
	
	DeviceState ramState;
    #ifdef DOUBLE_BUFFER
    bool ready_set = false;   
    bool use_double_buffer = true; 
    #endif
};

#endif
