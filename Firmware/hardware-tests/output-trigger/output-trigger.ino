#include <lp55231.h>

#include <Wire.h>

//#include "lp55231.h"

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused
static const int32_t interrupt_pin = 16;


static uint32_t next;
//static bool     ledon;

static lp55231 ledChip(0x32);

/*
 *
 */

static const uint16_t program[] =
{
  0x9c09, // 0 map start addr
  0x9c8c, // 1 map end addr
  0x3000, // 4 wait
  0x0eff, // 3 ramp up over 255
  0x3000, // 4 wait
  0x0fff, // 5 ramp down over 255
  0x9d80, // 6 mux map next
  0xf040, // 7 send ext trigger
  0xa002, // 8 loop to 2
  0x01ff, // 9 channel map - all
  0x01c0, // a channel map - reds
  0x0015, // b channel map - greens
  0x002a, // c channel map - blues
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
  pinMode(trigger_pin, INPUT_PULLUP);
  //digitalWrite(trigger_pin, HIGH);

  pinMode(interrupt_pin, INPUT);

  ledChip.init();
  ledChip.reset();
  ledChip.enable();

  delay(1000);

  for (uint8_t i = 0; i < 9; i++)
  {
    ledChip.setLogBrightness(i);
    ledChip.setDriveCurrent(i, 111);
  }

#if 1
  if (ledChip.loadProgram(program, 16))
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
  static uint32_t count = 1;

  if ( !digitalRead(trigger_pin))
  {
    Serial.println("trigger");

    while (!digitalRead(trigger_pin))
      ;

    delay(5);
    digitalWrite(trigger_pin, LOW);
    delay(5);
    digitalWrite(trigger_pin, HIGH);

  }

  if (millis() >= next)
  {
    Serial.print("#");
    Serial.print(count);

    Serial.print(" PC:");
    Serial.println(ledChip.getEnginePC(0));

    if (count == 1)
    {
      //ledChip.fastProgram(program);
    }

    //    if (count % 4 == 0)
    //    {
    //      Serial.println("send trigger");
    //      digitalWrite(trigger_pin, LOW);
    //      delay(20);
    //      digitalWrite(trigger_pin, HIGH);
    //
    //      Serial.println(ledChip.getEngineMap(0), HEX);
    //
    //    }

    count++;
    next += 1000;

  }
}


