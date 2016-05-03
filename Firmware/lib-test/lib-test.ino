#include <Wire.h>

#include "lp55231.h"

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused


static uint32_t next;
//static bool     ledon;

static lp55231 ledChip(0x32);

void setup()
{
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  digitalWrite(enable_pin, HIGH);

  // Trigger is active hi, so unused means low
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);

  ledChip.init();
  ledChip.reset();
  ledChip.enable();

  ledChip.setLogBrightness(6);
  ledChip.setLogBrightness(7);
  ledChip.setLogBrightness(8);

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

    ledChip.setBrightness(6, 0xff);
    ledChip.setBrightness(7, 0x7f);
    ledChip.setBrightness(8, 0x3f);

    count++;
    next += 25;
    

  }
}

#if 0
    if(count == 0)
    {  
      // enable chip
//      writeReg(0x32, REG_CNTRL1, 0x40 );
//      val = readReg(0x32, REG_CNTRL1);
//      Serial.print("REG_CNTRL1 ");
//      Serial.println(val);

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

#if 1 
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
#endif


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
      writeReg(0x32, REG_PROG_MEM_BASE     , 0x9c);// 0 output map start at addr _
      writeReg(0x32, REG_PROG_MEM_BASE +  1, 0x06);
      writeReg(0x32, REG_PROG_MEM_BASE +  2, 0x9c );// 1 output map - sel at addr 4
      writeReg(0x32, REG_PROG_MEM_BASE +  3, 0x8e);
      
      writeReg(0x32, REG_PROG_MEM_BASE +  4, 0x0e);// 2 ramp up
      writeReg(0x32, REG_PROG_MEM_BASE +  5, 0xff);
      writeReg(0x32, REG_PROG_MEM_BASE +  6, 0x0f);// 3 ramp dn
      writeReg(0x32, REG_PROG_MEM_BASE +  7, 0xff);
      writeReg(0x32, REG_PROG_MEM_BASE +  8, 0x9d);// 4 next map entry
      writeReg(0x32, REG_PROG_MEM_BASE +  9, 0x80);
      writeReg(0x32, REG_PROG_MEM_BASE +  10, 0xa0);// 5 loop
      writeReg(0x32, REG_PROG_MEM_BASE + 11, 0x02);

      writeReg(0x32, REG_PROG_MEM_BASE + 12, 0x00);// 6 mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 13, 0x01);
      writeReg(0x32, REG_PROG_MEM_BASE + 14, 0x00);// 7 mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 15, 0x02);
      writeReg(0x32, REG_PROG_MEM_BASE + 16, 0x00);// 8 mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 17, 0x04);
      writeReg(0x32, REG_PROG_MEM_BASE + 18, 0x00);// 9 mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 19, 0x08);
      writeReg(0x32, REG_PROG_MEM_BASE + 20, 0x00);// 10mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 21, 0x10);
      writeReg(0x32, REG_PROG_MEM_BASE + 22, 0x00);// 11mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 23, 0x20);
      writeReg(0x32, REG_PROG_MEM_BASE + 24, 0x00);// 12mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 25, 0x40);
      writeReg(0x32, REG_PROG_MEM_BASE + 26, 0x00);// 13mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 27, 0x80);
      writeReg(0x32, REG_PROG_MEM_BASE + 28, 0x01);// 14mux data
      writeReg(0x32, REG_PROG_MEM_BASE + 29, 0x00);

      // and that's amlost a full page...can we put more data in next page??

#if 0
      // check too see if it's there?
      Serial.print("prog peek ");
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+1) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+2) & 0xff), HEX);
      Serial.println((readReg(0x32, REG_PROG_MEM_BASE+3) & 0xff), HEX);
#endif

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

    val = readReg(0x32, REG_PC1);
    Serial.print("PC1 val ");
    Serial.println(val);
    val = readReg(0x32, REG_ENG1_MAP_LSB);
    Serial.print("map lsbs");
    Serial.println(val);


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

#endif    

