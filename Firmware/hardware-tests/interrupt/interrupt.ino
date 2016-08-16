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
 * Testing the "interrupt" feature.
 * Confision ensues.
 * 
 * The interrupt featuere of the LP55231 isn't entirely simple - it's not 
 * clearly documented, and has side effects.
 * 
 * First thing to note: page 1 of the datasheet indicates that 
 * int is used to inform the host the program execution is complete.  It might not serve as a 
 * very good waypoint in the middle of a program, due to the sode effects.
 * 
 * Second thing to note: the datasheet appears to have transposed the row labels between
 * the "INT" and "END" in table 6.  End is apparently 0xC400, Int is 0xC000
 * with optional flags of 0x1000 (int) and 0x0800 (reset)
 * 
 * If the int flag in the instruction isn't set, I'm not sure what it does.
 * 
 * If the reset flag is set, it resets the engine PC, ad well as the engine.
 * It also looks like the mapping values get cleared, and maybe the engine's variable....making
 * it hard to presist a map across a reset.
 * 
 * If you're going to stat over from scratch after an interrupt, the reset bit is probably useful.
 * If yuo're looking to retrigger the same program, there are some loose ends that need handling.
 * 
 * ........
 * 
 * As such, this example is showing the INT instruction flag with the INT bit set, but not reset.
 * 
 * When interrupt is detected, the handler sets PC to 2, the first instruction after the table.  This prevents
 * it from executing the map start instruction, which resets the map pointer.
 * 
 * 
 */

static const uint16_t program[] = 
{
  0x9c09, // 0 map start addr
  0x9c8c, // 1 map end addr
  0xf000, // 2 wait for trigger
  0x0eff, // 3 ramp up over 255
  0x3000, // 4 wait 
  0x0fff, // 5 ramp down over 255
  0x9d80, // 6 mux map next
  0xD000, // 7 interrupt
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

  ledChip.setEnginePC(0, 0);
  ledChip.setEngineRunning(0);


  Serial.println("### Setup complete");
  
}



void loop()
{
  int32_t result;
  int8_t  val;
  static uint32_t count = 1;

  // Poll the interrupt line.
  if( !digitalRead(interrupt_pin))
  {
    Serial.println("Interrupt asserted");

    Serial.println(ledChip.getEngineMap(0), HEX);
    
    ledChip.clearInterrupt();
    ledChip.setEnginePC(0, 2);
    ledChip.setEngineRunning(0);
  }
  
  if(millis() >= next)
  {
    Serial.print("#");
    Serial.print(count);

    Serial.print(" PC:");
    Serial.println(ledChip.getEnginePC(0));

    

    if(count == 1)
    {
      //ledChip.fastProgram(program);
    }    

    if(count % 4 == 0)
    {
      Serial.println("send trigger");
      digitalWrite(trigger_pin, LOW);
      delay(20);
      digitalWrite(trigger_pin, HIGH);

      Serial.println(ledChip.getEngineMap(0), HEX);

//      bool bit;
//
//      do
//      {
//          if( !digitalRead(interrupt_pin))
//          {
//            Serial.println("Interrupt asserted");
//            ledChip.clearInterrupt();
//            ledChip.setEnginePC(0, 0);
//            ledChip.setEngineRunning(0);
//            break;
//          }
//      }while(1);
      
      
    }

    count++;
    next += 1000;

  }
}


