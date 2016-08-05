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
  0x9c08, // 0 map start addr
  0x9c8b, // 1 map end addr
  0xf000, // 2 wait for trigger
  0x0efa, // 3 ramp up over 255
  0xf000, // 4 wait for trigger
  0x0fff, // 5 ramp down over 255
  0x9d80, // 6 mux map next
  0xa002, // 7 loop to 2
  0x01ff, // 8 channel map - all
  0x01c0, // 9 channel map - reds
  0x0015, // a channel map - greens
  0x002a, // b channel map - blues
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
    //ledChip.setLogBrightness(i);
    ledChip.setDriveCurrent(i, 111);
  }

#if 1
  if(ledChip.loadProgram(program, 16))
  {
    Serial.println("Program loaded?");
  }
  else
  {
    Serial.println("Program dodn't load?");
  }
#endif

  next = millis() + 3000;

  Serial.println("### Setup complete");
  
}



void loop()
{
  int32_t result;
  int8_t  val;
  static uint32_t count = 0;

  if( !digitalRead(interrupt_pin))
  {
    Serial.println("Interrupt asserted");
    ledChip.clearInterrupt();
    ledChip.setEnginePC(0, 0);
    ledChip.setEngineRunning(0);
  }
  
  if(millis() >= next)
  {
    Serial.print("#");
    Serial.println(count);

    if(count == 0)
    {
      //ledChip.fastProgram(program);
      ledChip.setEnginePC(0, 0);
      ledChip.setEngineRunning(0);
    }    

    if(count % 6 == 0)
    {
      Serial.println("send trigger");
      digitalWrite(trigger_pin, LOW);
      delay(20);
      digitalWrite(trigger_pin, HIGH);
    }

    count++;
    next += 1000;

  }
}


