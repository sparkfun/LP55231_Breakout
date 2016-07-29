#include <Wire.h>

#include "lp55231.h"

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused
static const int32_t interrupt_pin = 16;


static uint32_t next;
//static bool     ledon;

static lp55231 ledChip(0x32);

static const uint16_t program[] = 
{
  0x9D02,
  0x38ff, // 0 map start
  0x39ff, // 1 map end
  0xa000 // 2 ramp up over 255
};

void setup()
{
  Serial.begin(9600);
  
  delay(2000);
  Serial.println("### Setup entry");
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  digitalWrite(enable_pin, HIGH);

  // Trigger is active hi, so unused means low
  // but External trigger signal is active low
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, HIGH);

  pinMode(interrupt_pin, INPUT);

  ledChip.init();
  ledChip.reset();
  ledChip.enable();

  delay(1000);


  for(uint8_t i = 0; i < 9; i++)
  {
    ledChip.setLogBrightness(i);
    ledChip.setDriveCurrent(i, 111);
  }

#if 1
  if(ledChip.loadProgram(program, 4))
  {
    Serial.println("Program loaded?");
  }
  else
  {
    Serial.println("Program dodn't load?");
  }

#endif

  next = millis() + 3000;

  ledChip.setEnginePC(0, 0);
  ledChip.setEngineRunning(0);

  Serial.println("### Setup complete");
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

    Serial.print("PC: ");
    Serial.print(ledChip.getEnginePC(0));
    Serial.print(" map: ");
    Serial.println(ledChip.getEngineMap(0));

    count++;
    next += 1000;

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

