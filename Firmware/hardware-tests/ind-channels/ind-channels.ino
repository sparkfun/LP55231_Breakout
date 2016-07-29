#include <lp55231.h>

#include <Wire.h>

#include "lp55231.h"

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused
static const int32_t interrupt_pin = 16;


static uint32_t next;
//static bool     ledon;

static lp55231 ledChip(0x32);

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
//  pinMode(trigger_pin, OUTPUT);
//  digitalWrite(trigger_pin, HIGH);

//  pinMode(interrupt_pin, INPUT);

  ledChip.init();
  ledChip.reset();
  ledChip.enable();

  delay(1000);

  for(uint8_t i = 0; i < 9; i++)
  {
    ledChip.setDriveCurrent(i, 111);
  }

  next = millis() + 3000;

  Serial.println("### Setup complete");
  
}



void loop()
{
  int32_t result;
  int8_t  val;
  static uint32_t count = 0;
  static uint32_t prev = 0;

  if(millis() >= next)
  {
    Serial.print("#");
    Serial.println(count);

    ledChip.setBrightness(prev, 0);
    ledChip.setBrightness(count, 0xff);

    prev = count % 9;
    count++;
    count %= 9;
    next += 1000;

  }
}

