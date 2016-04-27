#include <Wire.h>

static const int32_t enable_pin = 15; // Apparently active high?
static const int32_t trigger_pin = 14; // low if unused

static uint32_t next;
//static bool     ledon;

int8_t readReg(int8_t dev, int8_t reg)
{
  // Wire is awkward because it doesn't really have a register address concept.
  // http://www.arduino.cc/en/Tutorial/SFRRangerReader for reference
  
  Wire.beginTransmission(dev);
  Wire.write(reg);
  Wire.endTransmission();

  if(Wire.requestFrom(dev, 1))
  {
    return(Wire.read());
  }
  else
  {
    Serial.println("readReg failed?");
  }

}

void setup()
{
  
  pinMode(enable_pin, OUTPUT);
  digitalWrite(enable_pin, HIGH);
  //digitalWrite(enable_pin, LOw);
  
  pinMode(trigger_pin, OUTPUT);
  digitalWrite(trigger_pin, LOW);

  Wire.begin();

  next = millis() + 3000;
}



void loop()
{
  int32_t result;
  int8_t  val;

  
  if(millis() >= next)
  {
    Serial.println("#");

    val = readReg(0x32, 0);
    Serial.print("Value0 ");
    Serial.println(val);
    
    val = readReg(0x32, 1);
    Serial.print("Value1 ");
    Serial.println(val);
    
    val = readReg(0x32, 2);
    Serial.print("Value2 ");
    Serial.println(val);
    
    val = readReg(0x32, 3);
    Serial.print("Value3 ");
    Serial.println(val);
    
    val = readReg(0x32, 4);
    Serial.print("Value4 ");
    Serial.println(val);
    
    val = readReg(0x32, 5);
    Serial.print("Value5 ");
    Serial.println(val);

  
#if 0  
    result = Wire.requestFrom(0x32, 5, true);
  
    Serial.print("Request Result: ");
    Serial.println(result, HEX);
  
    if(result > 0)
    {
      for(int8_t i; i < result; i++)
      {
        val = Wire.read();
      }
    }
  
#endif
    
    next += 3000;
    
  }
}

