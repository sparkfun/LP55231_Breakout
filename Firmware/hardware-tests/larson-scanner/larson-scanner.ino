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
  0x9c10, // 0 map start
  0x9c9f, // 1 map end
  0x02ff, // 2 ramp up
  0x0800, // 3 wait
  0x03ff, // 4 ramp down
  0x9d80, // 5 map next
  0xa002, // 6 loop to 2
  0x000a, // 7
  0x0005, // 8
  0x000a, // 9
  0x0005, // a
  0x000a, // b
  0x0005, // c
  0x000a, // d
  0x0005, // e
  0x000a, // f
  0x0001, // 10 map begin - start of 2nd page
  0x0002, // 11
  0x0040, // 12
  0x0004, // 13
  0x0008, // 14
  0x0080, // 15
  0x0010, // 16
  0x0020, // 17
  0x0100, // 18
  0x0020, // 19
  0x0010, // 1a
  0x0080, // 1b
  0x0008, // 1c
  0x0004, // 1d
  0x0040, // 1e
  0x0002, // 1f  map end
};


void setup()
{
  Serial.begin(9600);
  
  delay(5000);
  Serial.println("### Setup entry");
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, LOW);
  digitalWrite(enable_pin, HIGH);

  ledChip.init();
  ledChip.reset();
  ledChip.enable();

  // Chip needs a moment to wake up.

  delay(1000);

  ledChip.clearInterrupt();

  for(uint8_t i = 0; i < 9; i++)
  {
    ledChip.setLogBrightness(i);
    ledChip.setDriveCurrent(i, 111);
  }

  if(ledChip.loadProgram(program, (sizeof(program)/2)))
  {
    Serial.println("Program loaded?");

    if(ledChip.verifyProgram(program, (sizeof(program)/2)))
    {
      Serial.println("program verifies");
    }
  }
  else
  {
    Serial.println("Program dodn't load?");
  }

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
  static uint32_t count = 0;

  if(millis() >= next)
  {
    next += 1000;
    count++;
    
    Serial.print("# ");
    Serial.println(count);

    ledChip.showControls();

    Serial.print(ledChip.getEnginePC(0));
    Serial.print(" ");
    Serial.println(ledChip.getEngineMode(0));
   
  }
}


