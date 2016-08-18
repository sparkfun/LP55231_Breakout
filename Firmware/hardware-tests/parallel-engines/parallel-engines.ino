#include <Wire.h>

#include <lp55231.h>

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused
static const int32_t interrupt_pin = 16;


static uint32_t next;
//static bool     ledon;

static lp55231 ledChip(0x32);

static const uint16_t program[] = 
{
  0x9D02, // 0 map direct 
  0x18ff, // 1 ramp up
  0x19ff, // 2 ramp dn
  //0xa000, // 3 loop to 0
  0x1d00,
  0x9D03, // 4 map direct 
  0x18ff, // 5 ramp up
  0x19ff, // 6 ramp dn
  //0xa000, // 7 loop to 4
  0x1d00, // 7 wait...
  0x9D06, // 8 map direct 
  0x18ff, // 9 ramp up
  0x19ff, // a ramp dn
  0xa008  // b loop to 4
  
};


void setup()
{
  Serial.begin(9600);
  
  delay(5000);
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
    ledChip.setLogBrightness(i);
    ledChip.setDriveCurrent(i, 111);
  }

  ledChip.showControls();

  ledChip.clearInterrupt();
  ledChip.setEnginePC(0, 4);

  ledChip.showControls();

  Serial.println(ledChip.getEnginePC(0));
  Serial.println(ledChip.getEnginePC(1));
  Serial.println(ledChip.getEnginePC(2));


#if 1
  Serial.println(sizeof(program)/2);
  if(ledChip.loadProgram(program, (sizeof(program)/2)))
  {
    Serial.println("Program loaded?");

    if(ledChip.verifyProgram(program, (sizeof(program)/2)))
    {
      Serial.println("program verifies");
    }
    // else: lib will print message...
  }
  else
  {
    Serial.println("Program dodn't load?");
  }
#endif

  next = millis() + 3000;

  ledChip.clearInterrupt();
  ledChip.setEnginePC(0, 4);

  ledChip.showControls();

  Serial.println(ledChip.getEnginePC(0));
  Serial.println(ledChip.getEnginePC(1));
  Serial.println(ledChip.getEnginePC(2));
  
  //ledChip.setEnginePC(1, 4);
  ledChip.setEngineRunning(0);
  //ledChip.setEngineRunning(1);

  Serial.println("### Setup complete");
  
}



void loop()
{
  int32_t result;
  int8_t  val;
  static uint32_t count = 0;

  if(millis() >= next)
  {
    next += 10000;
    count++;
    
    Serial.print("# ");
    Serial.println(count);

  Serial.println(ledChip.getEnginePC(0));
//    Serial.print(" ");
//    Serial.println(ledChip.getEnginePC(1));
//    if(count % 20 == 0)
//      ledChip.setEnginePC(0, 0);
    
  }
}


