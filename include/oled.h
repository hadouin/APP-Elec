#include "Arduino.h"

#include "fontData.h"
#include <i2c.h>

#define LCD_SLAVE_ADDR 0x3C // L'adresse semble être 0x3C
#define LCD_ON 0x4f
#define LCD_OFF 0x4e
#define PUMP 0x14 // 0x8d

void mydelay(int duration)
{
  int i;
  while (duration--)
    for (i = 0; i < 1000; i++)
      ;
}

void WriteCmd(int cmd)
{
  I2CSend(LCD_SLAVE_ADDR, 2, 0x0, (char)cmd); // 00 indique le mode commande
}

void WriteData(int data)
{
  I2CSend(LCD_SLAVE_ADDR, 2, 0x40, (char)data); // 0x40 indique le mode data
}

void SetPageAddress(uint8_t add)
{
  add &= 0x7;
  add = 0xb0 | add;
  WriteCmd(add);
  return;
}

void SetColumnAddress(uint8_t add)
{
  add += 2;
  WriteCmd((0x10 | (add >> 4)));
  WriteCmd((0x0f & add));
  return;
}

void Fill(int val)
{

  int i, j;
  for (i = 0; i < 8; i++)
  {
    SetPageAddress(i);
    SetColumnAddress(0);
    for (j = 0; j < 128; j++)
    {
      WriteData(val);
    }
  }
}
#define NORMALDISPLAY 0xA6
#define INVERTDISPLAY 0xA7
#define ACTIVATESCROLL 0x2F
#define DEACTIVATESCROLL 0x2E
#define SETVERTICALSCROLLAREA 0xA3
#define RIGHTHORIZONTALSCROLL 0x26
#define LEFT_HORIZONTALSCROLL 0x27
#define VERTICALRIGHTHORIZONTALSCROLL 0x29
#define VERTICALLEFTHORIZONTALSCROLL 0x2A

void InvertDisplay(int inv)
{
  if (inv)
    WriteCmd(INVERTDISPLAY);
  else
    WriteCmd(NORMALDISPLAY);
}

void Display(unsigned char *p)
{
  int i, j;

  for (i = 0; i < 8; i++)
  {
    SetPageAddress(i);
    SetColumnAddress(0);
    for (j = 127; j >= 0; j--)
    {
      WriteData(p[i * 128 + j]);
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
#define SX 16
#define EX 96

#define SY 3
#define EY 5
void DrawTest()
{
  int j;
  SetPageAddress(SY);
  SetColumnAddress(SX);
  for (j = SX; j < EX; j++)
    WriteData(0x80);
  SetPageAddress(4);
  SetColumnAddress(SX);
  WriteData(0x255);
  SetColumnAddress(EX);
  WriteData(0x255);
  SetPageAddress(EY);
  SetColumnAddress(SX);
  for (j = SX; j < EX; j++)
    WriteData(0x1);
}

void DisplayCarac(int x, int y, char c)
{
  int i, j;
  x = 128 - x;
  SetPageAddress(y);
  SetColumnAddress(x - 5);
  for (i = 4; i >= 0; i--)
  {
    j = fontData[5 * (c - 32) + i];
    WriteData(j);
  }
}

void DisplayString(int x, int y, char *s)
{

  while (*s)
  {
    DisplayCarac(x, y, *s);
    x += 6;
    s++;
  }
}
