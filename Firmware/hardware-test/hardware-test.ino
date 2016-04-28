#include <Wire.h>

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused

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

static uint32_t next;
//static bool     ledon;

int8_t readReg(int8_t dev, int8_t reg)
{
  // Wire is awkward because it doesn't really have a register address concept.
  // http://www.arduino.cc/en/Tutorial/SFRRangerReader for reference
  
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.endTransmission();

  if(Wire.requestFrom(dev, 1))
  {
    return(Wire.read());
  }
  else
  {
    Serial.println("readReg failed?");
  }
}

void writeReg(int8_t dev, int8_t reg, int8_t val)
{
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.write(val);
  Wire.endTransmission();
}

void setup()
{
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  digitalWrite(enable_pin, HIGH);

  // Trigger is active hi, so unused means low
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);

  Wire.begin();

  next = millis() + 3000;
}



void loop()
{
  int32_t result;
  int8_t  val;
  static uint32_t count = 0;
  
  if(millis() >= next)
  {
    Serial.print("#");
    Serial.println(count);

    if(count == 0)
    {  
      // enable chip
//      writeReg(0x32, REG_CNTRL1, 0x40 );
//      val = readReg(0x32, REG_CNTRL1);
//      Serial.print("REG_CNTRL1 ");
//      Serial.println(val);

      // force reset
      writeReg(0x32, REG_RESET, 0xff);
      // reenable
      writeReg(0x32, REG_CNTRL1, 0x40 );
      
      val = readReg(0x32, REG_STATUS_IRQ);
      Serial.print("REG_STATUS_IRQ ");
      Serial.println(val);
      
      delay(100);

      val = readReg(0x32, REG_STATUS_IRQ);
      Serial.print("REG_STATUS_IRQ ");
      Serial.println(val);

      // enable internal clock & charge pump
      writeReg(0x32, REG_MISC, 0x1b);
      //writeReg(0x32, REG_MISC, 0x03);
      val = readReg(0x32, REG_MISC);
      Serial.print("REG_MISC ");
      Serial.println(val);

      // enable ratiometric color?
//      writeReg(0x32, REG_RATIO_MSB, 0x01);
//      writeReg(0x32, REG_RATIO_LSB, 0xff);
  
      // set all outputs to log
      writeReg(0x32, REG_D1_CTRL, 20);
      writeReg(0x32, REG_D2_CTRL, 20);
      writeReg(0x32, REG_D3_CTRL, 20);
      writeReg(0x32, REG_D4_CTRL, 20);
      writeReg(0x32, REG_D5_CTRL, 20);
      writeReg(0x32, REG_D6_CTRL, 20);
      writeReg(0x32, REG_D7_CTRL, 20);
      writeReg(0x32, REG_D8_CTRL, 20);
      writeReg(0x32, REG_D9_CTRL, 20);



      //Try poking in a program
      //turn on ram access?
      // CNTRL1 has 0's in engine control - IE: hold mode
      writeReg(0x32, REG_CNTRL2, 0x10);

      // the wait to change modes
      do
      {
        val = readReg(0x32, REG_STATUS_IRQ) & 0x10; // engine busy bit
        Serial.print(".");
      }
      while(val);

      //try to write program from example
      // datasheet says MSB of each instruction is in earlier address
      // TBD: could optimize with a sequence of byte writes, using auto increment
      writeReg(0x32, REG_PROG_MEM_BASE     , 0x9d);// output map - chan 7
      writeReg(0x32, REG_PROG_MEM_BASE +  1, 0x07);
      writeReg(0x32, REG_PROG_MEM_BASE +  2, 0x3e);// ramp up
      writeReg(0x32, REG_PROG_MEM_BASE +  3, 0xff);
      writeReg(0x32, REG_PROG_MEM_BASE +  4, 0x3f);// ramp dn
      writeReg(0x32, REG_PROG_MEM_BASE +  5, 0xff);
      writeReg(0x32, REG_PROG_MEM_BASE +  6, 0xa0);// loop
      writeReg(0x32, REG_PROG_MEM_BASE +  7, 0x01);

      // check too see if it's there?
      Serial.print("prog peek ");
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+1) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+2) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+3) & 0xff), HEX);


      // set the PC1
      writeReg(0x32, REG_PC1, 0);
        
      // tell engine how to execute the program...free/step, etc
      writeReg(0x32, REG_CNTRL1, 0x40 | 0x20 );

      // and start execution
      writeReg(0x32, REG_CNTRL2, 0x20);
      
    }

#if 0
    for(uint8_t i = 0; i < 32; i++)
    {
      val = readReg(0x32, i);
      
      Serial.print("Register: ");
      Serial.print( i );
      Serial.print(":");
      Serial.println((val&0x0ff), HEX);
    }

    for(uint8_t i = 0x26; i < 0x2f; i++)
    {
      val = readReg(0x32, i);
      
      Serial.print("Register: ");
      Serial.print( i );
      Serial.print(":");
      Serial.println((val&0x0ff), HEX);
    }
#endif
#if 0
    val = count & 0xff;
    writeReg(0x32, REG_D8_PWM, val );
    writeReg(0x32, REG_D3_PWM, val );
    writeReg(0x32, REG_D4_PWM, val );

    writeReg(0x32, REG_D9_PWM, 0xff-val );
    writeReg(0x32, REG_D5_PWM, 0xff-val );
    writeReg(0x32, REG_D6_PWM, 0xff-val );
//
//    writeReg(0x32, REG_D7_PWM, (count>>1) );
//    writeReg(0x32, REG_D1_PWM, count );
//    writeReg(0x32, REG_D2_PWM, count );


//    val = readReg(0x32, REG_D8_PWM);
//    Serial.print("PWM D8 val ");
//    Serial.println(val);

#endif
    next += 1000;
    count++;
  }
}

