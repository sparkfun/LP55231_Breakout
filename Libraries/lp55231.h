#pragma once

#ifndef _LP55231_H_
#define _LP55231_H_

#include <stdint.h>

class lp55231
{
  public:

    lp55231(uint8_t address = 0x32);
    void init();

    // fundamental operations
    void enable();
    void disable();
    void reset();
    uint8_t clearInterrupt();

    // basic LED control functions
    bool setBrightness(uint8_t channel, uint8_t value);
    bool setLogBrightness(uint8_t channel);
    bool setDriveCurrent(uint8_t channel, uint8_t value);

    // More advanced features
    int8_t readDegC();
    float  readLEDADC(uint8_t channel);
    float  readVoutADC();
    float  readVddADC();
    float  readIntADC();


    // Execution engine related items.
    bool loadProgram(const uint16_t* prog, uint8_t len);
    bool verifyProgram(const uint16_t* prog, uint8_t len);
    bool setEngineEntryPoint(uint8_t engine, uint8_t addr);
    bool setEnginePC(uint8_t engine, uint8_t addr);
    uint8_t getEnginePC(uint8_t engine);
    uint8_t getEngineMap(uint8_t engine);

    void showControls();
    bool setMasterFader(uint8_t engine, uint8_t value);


    // engine modes
    bool setEngineModeHold(uint8_t engine);
    bool setEngineModeStep(uint8_t engine);
    bool setEngineModeOnce(uint8_t engine);
    bool setEngineModeFree(uint8_t engine);
    uint8_t getEngineMode(uint8_t engine);

    // start an engine.
    bool setEngineRunning(uint8_t engine);


  private:
    void waitForBusy();
    uint8_t readADCInternal(uint8_t channel);

    // private methods
    uint8_t readReg(uint8_t reg);
    void   writeReg(uint8_t reg, uint8_t val);

    // private data
    uint8_t _address;



};

#endif
