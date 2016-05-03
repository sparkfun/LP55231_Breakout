#include <Arduino.h>
#include <Wire.h>


#include "lp55231.h"

// register stuff
static const uint8_t REG_CNTRL1 = 0x00;
static const uint8_t REG_CNTRL2 = 0x01;
static const uint8_t REG_RATIO_MSB = 0x02;
static const uint8_t REG_RATIO_LSB = 0x03;
static const uint8_t REG_OUTPUT_ONOFF_MSB = 0x04;
static const uint8_t REG_OUTPUT_ONOFF_LSB = 0x05;

// Per LED control channels - fader channel assig, log dimming enable, temperature compensation
static const uint8_t REG_D1_CTRL = 0x06;
static const uint8_t REG_D2_CTRL = 0x07;
static const uint8_t REG_D3_CTRL = 0x08;
static const uint8_t REG_D4_CTRL = 0x09;
static const uint8_t REG_D5_CTRL = 0x0a;
static const uint8_t REG_D6_CTRL = 0x0b;
static const uint8_t REG_D7_CTRL = 0x0c;
static const uint8_t REG_D8_CTRL = 0x0d;
static const uint8_t REG_D9_CTRL = 0x0e;

// Direct PWM control registers
static const uint8_t REG_D1_PWM  = 0x16;
static const uint8_t REG_D2_PWM  = 0x17;
static const uint8_t REG_D3_PWM  = 0x18;
static const uint8_t REG_D4_PWM  = 0x19;
static const uint8_t REG_D5_PWM  = 0x1a;
static const uint8_t REG_D6_PWM  = 0x1b;
static const uint8_t REG_D7_PWM  = 0x1c;
static const uint8_t REG_D8_PWM  = 0x1d;
static const uint8_t REG_D9_PWM  = 0x1e;

// Drive cuttent registers
static const uint8_t REG_D1_I_CTL = 0x26;
static const uint8_t REG_D2_I_CTL  = 0x27;
static const uint8_t REG_D3_I_CTL  = 0x28;
static const uint8_t REG_D4_I_CTL  = 0x29;
static const uint8_t REG_D5_I_CTL  = 0x2a;
static const uint8_t REG_D6_I_CTL  = 0x2b;
static const uint8_t REG_D7_I_CTL  = 0x2c;
static const uint8_t REG_D8_I_CTL  = 0x2d;
static const uint8_t REG_D9_I_CTL  = 0x2e;

static const uint8_t REG_MISC     = 0x36;
static const uint8_t REG_PC1 = 0x37;
static const uint8_t REG_PC2 = 0x38;
static const uint8_t REG_PC3 = 0x39;
static const uint8_t REG_STATUS_IRQ = 0x3A;
static const uint8_t REG_RESET      = 0x3D;

static const uint8_t REG_PROG1_START = 0x4C;
static const uint8_t REG_PROG2_START = 0x4D;
static const uint8_t REG_PROG3_START = 0x4E;
static const uint8_t REG_PROG_PAGE_SEL = 0x4f;

// Memory is more confusing - there are 4 pages, sel by addr 4f
static const uint8_t REG_PROG_MEM_BASE = 0x50;
//static const uint8_t REG_PROG_MEM_SIZE = 0x;//
static const uint8_t REG_PROG_MEM_END  = 0x6f;

static const uint8_t REG_ENG1_MAP_MSB = 0x70;
static const uint8_t REG_ENG1_MAP_LSB = 0x71;
static const uint8_t REG_ENG2_MAP_MSB = 0x72;
static const uint8_t REG_ENG2_MAP_LSB = 0x73;
static const uint8_t REG_ENG3_MAP_MSB = 0x74;
static const uint8_t REG_ENG3_MAP_LSB = 0x75;

lp55231::lp55231(uint8_t address)
{
  _address = address;
}

void lp55231::init()
{
  Wire.begin();

  // TBD: so we want more operations in here, ro should they be called 
  // explicitly?
}

void lp55231::enable()
{
  // set  enable bit
  writeReg(REG_CNTRL1, 0x40 );

  // enable internal clock & charge pump
  writeReg(REG_MISC, 0x1b);
}

void lp55231::disable()
{
  uint8_t val;

  val = readReg(REG_CNTRL1);
  val &= ~0x40;
  writeReg(REG_CNTRL1, val);
}

void lp55231::reset()
{
  // force reset
  writeReg(REG_RESET, 0xff);
}

bool lp55231::setBrightness(uint8_t channel, uint8_t value)
{
  if(channel > 9)
  {
    Serial.println("setBrightness: invalid channel");
    return false;
  }

  writeReg(REG_D1_PWM + channel, value);
}

void lp55231::setLogBrightness(uint8_t channel)
{
  if(channel > 9)
  {
    Serial.println("setLogBrightness: invalid channel");
    return ;
  }

  writeReg(REG_D1_CTRL + channel, 20);
}


uint8_t lp55231::readReg(uint8_t reg)
{
  // Wire is awkward because it doesn't really have a register address concept.
  // http://www.arduino.cc/en/Tutorial/SFRRangerReader for reference
  
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.endTransmission();

  if(Wire.requestFrom(_address, 1))
  {
    return(Wire.read());
  }
  else
  {
    Serial.println("readReg failed?");
  }
}

void lp55231::writeReg(uint8_t reg, uint8_t val)
{
  Wire.beginTransmission(_address);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}




