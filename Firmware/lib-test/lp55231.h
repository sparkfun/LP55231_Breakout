#pragma once

#ifndef _LP55231_H_
#define _LP55231_H_

#include <stdint.h>

class lp55231
{
  public:

    lp55231(uint8_t address = 0x32);
    void init();

    // basic operations
    void enable();
    void disable();
    void reset();

    bool setBrightness(uint8_t channel, uint8_t value);
    void setLogBrightness(uint8_t channel);
  
  private:

    // private methods
    uint8_t readReg(uint8_t reg);
    void   writeReg(uint8_t reg, uint8_t val);
    
    // private data
    uint8_t _address;

    
    
};

#endif

