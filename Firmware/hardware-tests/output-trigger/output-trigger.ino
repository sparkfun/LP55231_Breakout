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
 * More trickery at play here (not as tangled as the interrupt example).
 * 
 * The LP55231 trigger line can be used as both an input and an output.
 * This example does exactly that...bit at first,v we couldn't explain why it worked.
 * 
 * The Trigger instruction on the LP allows for it listen for a trigger, or to drive 
 * the trigger line.  If both options are used in the same instruction, it sends a 
 * short (roughly 100 uSec) trigger pulse, then waits for a response.
 * 
 * This sketch waits until it sees that pulse, observes it going high again, then 
 * drives the line.
 * 
 * But it wasn't entirely apparent why & how it was working.  The AVR port pin 
 * architecture held the explanation.
 * 
 * The trigger pin on the pro micro is configured as INPUT_PULLUP in
 * setup().  Loop polls that pin, looking for a low.  When it finds a low, it waits for 
 * it to go high again, waits a touch longer, the drives it low itself.
 * 
 * Yes, it's an input...but it's allowed to drive it low, but somewhat indirectly.
 * 
 * When configured as an output, the data bits enable the pullup resistors.
 * 
 * So by writing a low to the output pin, we essentially defeat the pullup.
 * The Trigger line is pulled low on the LP breakout, which takes over when the 
 * PUP is defeated.
 * 
 * This will probably work with other AVR based microcontrollers, but 
 * might not be portable beynd that.
 */

static const uint16_t program[] =
{
  0x9c09, // 0 map start addr
  0x9c8c, // 1 map end addr
  0x3000, // 2 wait
  0x0eff, // 3 ramp up over 255
  0x3000, // 4 wait
  0x0fff, // 5 ramp down over 255
  0x9d80, // 6 mux map next
  0xf040, // 7 trigger - send to micro, and wait for response.
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

  ledChip.clearInterrupt();
  
  ledChip.setEngineEntryPoint(0, 0);
  ledChip.setEnginePC(0, 0);
  
  ledChip.setEngineModeFree(0);
  ledChip.showControls();

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


