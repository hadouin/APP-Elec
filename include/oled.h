#include <Arduino.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_i2c.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/i2c.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
//#include "isep.h"
#include "fontData.h"

#define PORTI2C   2    // Selection du port I2C 0, 1 ou 2



//initialize I2C module 0
//Slightly modified version of TI's example code
void InitI2C0(void)
{
    //enable I2C module 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    //enable GPIO peripheral that contains I2C 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PB2_I2C0SCL);
    GPIOPinConfigure(GPIO_PB3_I2C0SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
    GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), false);

    //clear I2C FIFOs
    HWREG(I2C0_BASE + I2C_O_FIFOCTL) = 80008000;
}

void InitI2C1(void)
{
    //enable I2C module 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C1);

    //enable GPIO peripheral that contains I2C 1
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    // Configure the pin muxing for I2C1 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PA6_I2C1SCL);
    GPIOPinConfigure(GPIO_PA7_I2C1SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
    GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);

    //clear I2C FIFOs
    HWREG(I2C1_BASE + I2C_O_FIFOCTL) = 80008000;
}


void InitI2C2(void)
{
    //enable I2C module 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C2);

    //reset module
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C2);

    //enable GPIO peripheral that contains I2C 2
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    // Configure the pin muxing for I2C0 functions on port B2 and B3.
    GPIOPinConfigure(GPIO_PE4_I2C2SCL);
    GPIOPinConfigure(GPIO_PE5_I2C2SDA);

    // Select the I2C function for these pins.
    GPIOPinTypeI2CSCL(GPIO_PORTE_BASE, GPIO_PIN_4);
    GPIOPinTypeI2C(GPIO_PORTE_BASE, GPIO_PIN_5);

    // Enable and initialize the I2C0 master module.  Use the system clock for
    // the I2C0 module.  The last parameter sets the I2C data transfer rate.
    // If false the data rate is set to 100kbps and if true the data rate will
    // be set to 400kbps.
    I2CMasterInitExpClk(I2C2_BASE, SysCtlClockGet(), false);

    //clear I2C FIFOs
    HWREG(I2C2_BASE + I2C_O_FIFOCTL) = 80008000;
}

void InitI2C(void)
{
    if (PORTI2C == 0)
        InitI2C0();
    else
        if (PORTI2C == 1)
            InitI2C1();
        else
            InitI2C2();
}
//sends an I2C command to the specified slave
void I2CSend(uint8_t slave_addr, uint8_t num_of_args, ...)
{

    uint8_t i;
    uint32_t PortI2c;

    if (PORTI2C == 0)
        PortI2c = I2C0_BASE;
    else
        if (PORTI2C == 1)
            PortI2c = I2C1_BASE;
        else
            PortI2c = I2C2_BASE;
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(PortI2c, slave_addr, false);

    //stores list of variable number of arguments
    va_list vargs;

    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);

    //put data to be sent into FIFO
    I2CMasterDataPut(PortI2c, va_arg(vargs, uint32_t));

    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(PortI2c, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(PortI2c));

        //"close" variable argument list
        va_end(vargs);
    }

    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(PortI2c, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(PortI2c));

        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(PortI2c, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(PortI2c, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while(I2CMasterBusy(PortI2c));
        }

        //put last piece of data into I2C FIFO
        I2CMasterDataPut(PortI2c, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(PortI2c, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(PortI2c));

        //"close" variable args list
        va_end(vargs);
    }
}

void I2C2Send(uint8_t slave_addr, uint8_t num_of_args, ...)
{

    uint8_t i;
    // Tell the master module what address it will place on the bus when
    // communicating with the slave.
    I2CMasterSlaveAddrSet(I2C2_BASE, slave_addr, false);

    //stores list of variable number of arguments
    va_list vargs;

    //specifies the va_list to "open" and the last fixed argument
    //so vargs knows where to start looking
    va_start(vargs, num_of_args);

    //put data to be sent into FIFO
    I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));

    //if there is only one argument, we only need to use the
    //single send I2C function
    if(num_of_args == 1)
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_SINGLE_SEND);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));

        //"close" variable argument list
        va_end(vargs);
    }

    //otherwise, we start transmission of multiple bytes on the
    //I2C bus
    else
    {
        //Initiate send of data from the MCU
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_START);

        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));

        //send num_of_args-2 pieces of data, using the
        //BURST_SEND_CONT command of the I2C module
        for(i = 1; i < (num_of_args - 1); i++)
        {
            //put next piece of data into I2C FIFO
            I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
            //send next data that was just placed into FIFO
            I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);

            // Wait until MCU is done transferring.
            while(I2CMasterBusy(I2C2_BASE));
        }

        //put last piece of data into I2C FIFO
        I2CMasterDataPut(I2C2_BASE, va_arg(vargs, uint32_t));
        //send next data that was just placed into FIFO
        I2CMasterControl(I2C2_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
        // Wait until MCU is done transferring.
        while(I2CMasterBusy(I2C2_BASE));

        //"close" variable args list
        va_end(vargs);
    }
}



#define LCD_SLAVE_ADDR 0x3C // L'adresse semble être 0x3C
#define LCD_ON 0x4f
#define LCD_OFF 0x4e
#define PUMP    0x14 //0x8d


void mydelay(int duration)
{
    int i;
    while(duration--)
     for (i=0;i<1000;i++);
}

void WriteCmd(int cmd)
{
    I2CSend(LCD_SLAVE_ADDR, 2,0x0, (char) cmd);  // 00 indique le mode commande
}

void WriteData(int data)
{
    I2CSend(LCD_SLAVE_ADDR, 2,0x40, (char) data);  // 0x40 indique le mode data
}



void SetPageAddress(uint8_t add) {
    add &= 0x7;
    add=0xb0|add;
    WriteCmd(add);
    return;
}

void SetColumnAddress(uint8_t add) {
        add += 2;
        WriteCmd((0x10|(add>>4)));
        WriteCmd((0x0f&add));
        return;
}






 void Fill(int val) {

    int i,j;
    for (i=0;i<8; i++) {
        SetPageAddress(i);
        SetColumnAddress(0);
        for (j=0; j<128; j++) {
            WriteData(val);
        }
    }
}
#define NORMALDISPLAY                   0xA6
#define INVERTDISPLAY                   0xA7
#define ACTIVATESCROLL                  0x2F
#define DEACTIVATESCROLL                0x2E
#define SETVERTICALSCROLLAREA           0xA3
#define RIGHTHORIZONTALSCROLL           0x26
#define LEFT_HORIZONTALSCROLL           0x27
#define VERTICALRIGHTHORIZONTALSCROLL   0x29
#define VERTICALLEFTHORIZONTALSCROLL    0x2A

void InvertDisplay(int inv) {
    if (inv)
    WriteCmd(INVERTDISPLAY);
    else
    WriteCmd(NORMALDISPLAY);
}

void Display(unsigned char *p) {
    int i, j;

    for (i=0; i<8; i++) {
        SetPageAddress(i);
        SetColumnAddress(0);
        for (j=127;j>=0;j--) {
            WriteData(p[i*128+j]);
        }
    }
}






void InitScreen(void)
{
    WriteCmd(0xAE); // Set display OFF

    WriteCmd(0xD4); // Set Display Clock Divide Ratio / OSC Frequency
    WriteCmd(0x80); // Display Clock Divide Ratio / OSC Frequency

    WriteCmd(0xA8); // Set Multiplex Ratio
    WriteCmd(0x3F); // Multiplex Ratio for 128x64 (64-1)

    WriteCmd(0xD3); // Set Display Offset
    WriteCmd(0x00); // Display Offset

    WriteCmd(0x40); // Set Display Start Line 0

    WriteCmd(0x8D); // Set Charge Pump
    WriteCmd(0x14); // Charge Pump (0x10 External, 0x14 Internal DC/DC)

    WriteCmd(0xA0); // Set Segment Re-Map
    WriteCmd(0xC8); // Set Com Output Scan Direction

    WriteCmd(0xDA); // Set COM Hardware Configuration
    WriteCmd(0x12); // COM Hardware Configuration

    WriteCmd(0x81); // Set Contrast
    WriteCmd(0xCF); // Contrast

    WriteCmd(0xD9); // Set Pre-Charge Period
    WriteCmd(0xF1); // Set Pre-Charge Period (0x22 External, 0xF1 Internal)

    WriteCmd(0xDB); // Set VCOMH Deselect Level
    WriteCmd(0x40); // VCOMH Deselect Level


    WriteCmd(0xb0);




    WriteCmd(0xA4); // Set all pixels OFF
    WriteCmd(0xA6); // Set display not inverted
    WriteCmd(0xAF); // Set display On


}
#define SX  16
#define EX  96

#define SY  3
#define EY  5
void    DrawTest()
{
    int j;
SetPageAddress(SY);
SetColumnAddress(SX);
for (j=SX; j<EX; j++)
      WriteData(0x80);
SetPageAddress(4);
SetColumnAddress(SX);
WriteData(0x255);
SetColumnAddress(EX);
WriteData(0x255);
SetPageAddress(EY);
SetColumnAddress(SX);
for (j=SX; j<EX; j++)
      WriteData(0x1);
}

void DisplayCarac(int x,int y,char c)
{
    int i,j;
    x = 128-x;
    SetPageAddress(y);
    SetColumnAddress(x-5);
    for(i=4;i>=0;i--)
    {
        j = fontData[5*(c-32)+i];
        WriteData(j);
    }
}

void DisplayString(int x,int y,char *s)
{

       while (*s)
       {
        DisplayCarac(x,y,*s);
        x += 6;
        s++;
       }
}