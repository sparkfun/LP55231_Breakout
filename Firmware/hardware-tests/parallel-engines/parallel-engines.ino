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
  0xa000, // 3 loop to 0
  //0x1d00,
  0x9D03, // 4 map direct 
  0x18ff, // 5 ramp up
  0x19ff, // 6 ramp dn
  //0xa000, // 7 loop to 4
  0x1d00, // 7 wait...
  0x9D04, // 8 map direct 
  0x18ff, // 9 ramp up
  0x19ff, // a ramp dn
  0xa000, // b loop to 4
  0x9d09, // c map direct 
  0x04ff,
  0x05ff,
  0xa001 // f loop to d (skip map instr...)
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

  // Multi-engine programs have a few particular details that need te be understood.
  //
  // After loading the program, the load/verify routines leave the execution engines
  // in hold state.  Apparently necessary to allow PC/entry point setting, and to set them to
  // running.
  //
  // Entry points have a funny side effect: the engine operates relative to the entry 
  // point, NOT the absolute program memory address.
  // IE: the program counter for the engine is relative to the entry point.  In the program
  // we're using here, engine 1 takes a start point of 4.  It will initialize, and report a 
  // PC of 0, which is actually the offset into memory from the entrry point.  If you 
  // observe the PC as it rungs, it will be in the range 0..7.
  //
  // The branch also
  // branches to 0, which is relative ot the entry point, meaning instruction address 4.
  // 
  // The sequence of operations is a rigid/fragile.  Getting things out of order means they 
  // probably won't work.  In general: load the program (optionally, verify), set the relevant entry points and PCs,
  // set execution mode (usually free-running), then set them running.
  // 
 
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
  
  ledChip.setEngineEntryPoint(1, 4);
  ledChip.setEnginePC(1, 4);

  ledChip.setEngineEntryPoint(2, 0x0c);
  //ledChip.setEnginePC(2, 0x0);

  ledChip.setEngineModeFree(0);
  ledChip.showControls();
  ledChip.setEngineModeFree(1);
  ledChip.showControls();
  ledChip.setEngineModeStep(2);
  ledChip.showControls();
  // Tried "once" mode, but it's not very useful...

  ledChip.setEngineRunning(0);
  ledChip.setEngineRunning(1);
  ledChip.setEngineRunning(2);

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
    Serial.print(ledChip.getEngineMode(0));
    Serial.print(" ");
    Serial.print(ledChip.getEnginePC(1));
    Serial.print(" ");
    Serial.print(ledChip.getEngineMode(1));
    Serial.print(" ");
    Serial.print(ledChip.getEnginePC(2));
    Serial.print(" ");
    Serial.println(ledChip.getEngineMode(2));

    // Single step sets engine mode to hold at end.
    // So we need to keep setting it.
    ledChip.setEngineModeStep(2);
    ledChip.setEngineRunning(2);
      
    
  }
}


