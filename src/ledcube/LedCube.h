/******************************************************************************************************************************************************
 *  COPYRIGHT
 *  ---------------------------------------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) Andreas Burnickl                                                                                                 All rights reserved.
 *
 *  \endverbatim
 *  ---------------------------------------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -------------------------------------------------------------------------------------------------------------------------------------------------*/
/**     \file       LedCube.h
 *      \brief      Main header file of LedCube library
 *
 *      \details    Arduino library to drive an 8x8x8 LED Cube
 *                  
 *
 *****************************************************************************************************************************************************/
#ifndef _LEDCUBE_H_
#define _LEDCUBE_H_

/******************************************************************************************************************************************************
 *  INCLUDES
 *****************************************************************************************************************************************************/
#include <stdint.h>
#include <string.h>

#include "StandardTypes.h"
#include "Arduino.h"
#include "ledcube/OutputPin.h"


/******************************************************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 *****************************************************************************************************************************************************/
/* LedCube Configuration Parameter */
#define LEDCUBE_NUMBER_OF_LEDS_PER_SIDE             8
#define LEDCUBE_NUMBER_OF_LAYERS                    LEDCUBE_NUMBER_OF_LEDS_PER_SIDE
#define LEDCUBE_NUMBER_OF_SHIFT_REGISTERS           9
#define LEDCUBE_SHIFT_REGISTER_BITWIDTH             8


/******************************************************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 *****************************************************************************************************************************************************/
//#define LEDCUBE_GET_VOXEL(X, Y, Z)                    bitRead(CurrentFrame[Y][Z], X)
//#define LEDCUBE_SET_VOXEL(X, Y, Z)                    bitSet(NextFrame[Y][Z], X)
//#define LEDCUBE_CLEAR_VOXEL(X, Y, Z)                  bitClear(NextFrame[Y][Z], X)
//#define LEDCUBE_WRITE_VOXEL(X, Y, Z, Value)           bitWrite(NextFrame[Y][Z], X, Value)


/******************************************************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *****************************************************************************************************************************************************/
/* Type which describes the internal state of the LedCube */
typedef enum {
    LEDCUBE_STATE_NONE,
    LEDCUBE_STATE_INIT,
    LEDCUBE_STATE_READY,
    LEDCUBE_STATE_RUNNING,
    LEDCUBE_STATE_STOPPED
} LedCubeStateType;

/* Type which defines the axes */
typedef enum {
    LEDCUBE_AXIS_X,
    LEDCUBE_AXIS_Y,
    LEDCUBE_AXIS_Z
} LedCubeAxisType;

/* Type which defines the buffer with all led states */
typedef byte BufferType[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE][LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];
/* Type which points to the buffer for the next or current frame */
typedef byte (*pFrameType)[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];


/******************************************************************************************************************************************************
 *  CLASS  LedCube
 *****************************************************************************************************************************************************/
/*!
 * \brief Drives an 8x8x8 LED cube through a chain of 74HC595 shift registers.
 *
 * The pins are known when compiling, so that on the AVR each write to them is
 * a write to the port register, not a call of the core's `digitalWrite()`.
 *
 * \code{.cpp}
 * LedCube<13, 11, 12> cube;  // data, clock, storage
 * \endcode
 *
 * \tparam DataPin The pin at the serial input (SER) of the first 74HC595.
 * \tparam ClockPin The pin at the shift clock (SRCLK) of every 74HC595.
 * \tparam StoragePin The pin at the storage clock (RCLK) of every 74HC595.
 */
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
class LedCube
{
  private:
    using DataOutput = ledcube::internal::OutputPin<DataPin>;
    using ClockOutput = ledcube::internal::OutputPin<ClockPin>;
    using StorageOutput = ledcube::internal::OutputPin<StoragePin>;

    byte CurrentLayer;
    LedCubeStateType State;
    pFrameType CurrentFrame;
    pFrameType NextFrame;
    BufferType Buffer1;
    BufferType Buffer2;
    boolean NextFrameReady;
    boolean NextFrameTimeElapsed;

    // functions
    void clearCube();
    stdReturnType showLayer(byte);
    void sendData(byte Data);
    void switchBufferPointer();
    stdReturnType setLayer();
    stdReturnType showNextFrame();

  public:
    LedCube();
    ~LedCube();

    // get methods
    byte getCurrentLayer() { return CurrentLayer; }
    LedCubeStateType getState() { return State; }
    boolean getNextFrameReady() { return NextFrameReady; }
    boolean getNextFrameTimeElapsed() { return NextFrameTimeElapsed; }

    // set methods
    stdReturnType setNextFrameReady();
    stdReturnType setNextFrameTimeElapsed();

    // methods
    void init();
    stdReturnType start();
    stdReturnType stop();
    void task();

    stdReturnType setVoxel(byte, byte, byte);
    stdReturnType clearVoxel(byte, byte, byte);
    stdReturnType writeVoxel(byte, byte, byte, boolean);
    stdReturnType getVoxel(byte, byte, byte, boolean*);

    void setVoxelFast(byte X, byte Y, byte Z) { bitSet(NextFrame[Y][Z], X); }
    void clearVoxelFast(byte X, byte Y, byte Z) { bitClear(NextFrame[Y][Z], X); }
    void writeVoxelFast(byte X, byte Y, byte Z, boolean Value) { bitWrite(NextFrame[Y][Z], X, Value); }
    byte getVoxelFast(byte X, byte Y, byte Z) { return bitRead(CurrentFrame[Y][Z], X); }

    stdReturnType getVoxelsX(byte, byte, byte*);
    stdReturnType setVoxelsX(byte, byte, byte);
    stdReturnType getVoxelsY(byte, byte, byte*);
    stdReturnType setVoxelsY(byte, byte, byte);
    stdReturnType getVoxelsZ(byte, byte, byte*);
    stdReturnType setVoxelsZ(byte, byte, byte);

    byte getVoxelsXFast(byte, byte);
    void setVoxelsXFast(byte, byte, byte);
    byte getVoxelsYFast(byte, byte);
    void setVoxelsYFast(byte, byte, byte);
    byte getVoxelsZFast(byte, byte);
    void setVoxelsZFast(byte, byte, byte);

    void clearVoxels() { memset(NextFrame, 0, sizeof(BufferType)); }
    void setVoxels() { memset(NextFrame, 255, sizeof(BufferType)); }

    void copyCurrentFrameToNextFrame() { memcpy(NextFrame, CurrentFrame, sizeof(BufferType)); }
};


/******************************************************************************************************************************************************
 * P U B L I C   F U N C T I O N S
 *****************************************************************************************************************************************************/

/******************************************************************************************************************************************************
  CONSTRUCTOR OF LedCube
******************************************************************************************************************************************************/
/*! \brief          LedCube Constructor
 *  \details        Instantiation of the LedCube library
 *
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::LedCube()
{
    CurrentLayer = 0;
    State = LEDCUBE_STATE_NONE;
    NextFrameReady = false,
    NextFrameTimeElapsed = false;
    CurrentFrame = Buffer1;
    NextFrame = Buffer2;
    memset(Buffer1, 0, sizeof(Buffer1));
    memset(Buffer2, 0, sizeof(Buffer2));
} /* LedCube */


/******************************************************************************************************************************************************
  DESTRUCTOR OF LedCube
******************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
LedCube<DataPin, ClockPin, StoragePin>::~LedCube()
{

} /* ~LedCube */


/******************************************************************************************************************************************************
  init()
******************************************************************************************************************************************************/
/*! \brief          initialization of the Led Cube hardware
 *  \details        this function initializes the IO hardware and the shift registers 74HC595
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::init()
{
    State = LEDCUBE_STATE_INIT;
    clearCube();
    DataOutput::Begin();
    ClockOutput::Begin();
    StorageOutput::Begin();
    State = LEDCUBE_STATE_READY;
} /* init */


/******************************************************************************************************************************************************
  start()
******************************************************************************************************************************************************/
/*! \brief          starts the led cube showing a frame
 *  \details        this function starts with showing the current buffer on the led cube
 *                  therefore the task function has to be called periodically
 *
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::start()
{
    if(State == LEDCUBE_STATE_READY || LEDCUBE_STATE_STOPPED) {
        State = LEDCUBE_STATE_RUNNING;
        return E_OK;
    }
    return E_NOT_OK;
} /* start */


/******************************************************************************************************************************************************
  stop()
******************************************************************************************************************************************************/
/*! \brief          stops the led cube showing a frame
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::stop()
{
    if(State == LEDCUBE_STATE_RUNNING) {
        State = LEDCUBE_STATE_STOPPED;
        return E_OK;
    }
    return E_NOT_OK;
} /* start */


/******************************************************************************************************************************************************
  task()
******************************************************************************************************************************************************/
/*! \brief          show layer which is the next in line
 *  \details        this function shows one layer of the current frame on the cube
 *                  has to be called periodically
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::task()
{
    if(State == LEDCUBE_STATE_RUNNING) {
        if(CurrentLayer >= LEDCUBE_NUMBER_OF_LAYERS) {
            CurrentLayer = 0;
            /* is next frame ready to show? */
            if(NextFrameReady && NextFrameTimeElapsed) { 
                showNextFrame();
                NextFrameReady = false;
                NextFrameTimeElapsed = false;
            }
        }
        showLayer(CurrentLayer);
        CurrentLayer++;
    }
} /* task */


/******************************************************************************************************************************************************
  clearCube()
******************************************************************************************************************************************************/
/*! \brief          clear all LEDs on the cube
 *  \details        this function initializes the shift registers with zero
 *
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::clearCube()
{
    DataOutput::Write(LOW);

    for(byte i = 0; i < (LEDCUBE_NUMBER_OF_SHIFT_REGISTERS * LEDCUBE_SHIFT_REGISTER_BITWIDTH); i++) {
        ClockOutput::Write(HIGH);
        ClockOutput::Write(LOW);
    }
    StorageOutput::Write(HIGH);
    StorageOutput::Write(LOW);
} /* clearCube */


/******************************************************************************************************************************************************
  setVoxel()
******************************************************************************************************************************************************/
/*! \brief          set a given voxel in the frame buffer
 *  \details        
              
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxel(byte X, byte Y, byte Z)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        bitSet(NextFrame[Y][Z], X);
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* setVoxel */


/******************************************************************************************************************************************************
  clearVoxel()
******************************************************************************************************************************************************/
/*! \brief          clear a given voxel in the frame buffer
 *  \details        
 *
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::clearVoxel(byte X, byte Y, byte Z)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        bitClear(NextFrame[Y][Z], X);
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* clearVoxel */


/******************************************************************************************************************************************************
  writeVoxel()
******************************************************************************************************************************************************/
/*! \brief          set given voxel to a given value in the frame buffer
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[in]      Value      voxel of value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::writeVoxel(byte X, byte Y, byte Z, boolean Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        bitWrite(NextFrame[Y][Z], X, Value);
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* writeVoxel */


/******************************************************************************************************************************************************
  getVoxel()
******************************************************************************************************************************************************/
/*! \brief          get a given voxel of the frame buffer
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[out]     Value      voxel of value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxel(byte X, byte Y, byte Z, boolean* Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        *Value = bitRead(CurrentFrame[Y][Z], X);
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* getVoxel */


/******************************************************************************************************************************************************
  getVoxelsX()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the x-axis of the frame buffer 
 *  \details        
 *                  
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[out]     Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsX(byte Y, byte Z, byte* Value)
{
    if(Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        *Value = CurrentFrame[Y][Z];
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* getVoxelsX */


/******************************************************************************************************************************************************
  setVoxelsX()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the x-axis in the frame buffer with a given value
 *  \details        
 *                  
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[in]      Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsX(byte Y, byte Z, byte Value)
{
    if(Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        NextFrame[Y][Z] = Value;
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* setVoxelsX */


/******************************************************************************************************************************************************
  getVoxelsY()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the y-axis of the frame buffer 
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[out]     Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsY(byte X, byte Z, byte* Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            bitWrite(*Value, Y, bitRead(CurrentFrame[Y][Z], X));
        }
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* getVoxelsY */


/******************************************************************************************************************************************************
  setVoxelsY()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the y-axis in the frame buffer with a given value
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[in]      Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsY(byte X, byte Z, byte Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            bitWrite(NextFrame[Y][Z], X, bitRead(Value, Y));
        }
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* setVoxelsY */


/******************************************************************************************************************************************************
  getVoxelsZ()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the z-axis of the frame buffer 
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[out]     Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::getVoxelsZ(byte X, byte Y, byte* Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        for(byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
            bitWrite(*Value, Z, bitRead(CurrentFrame[Y][Z], X));
        }
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* getVoxelsZ */


/******************************************************************************************************************************************************
  setVoxelsZ()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the z-axis in the frame buffer with a given value
 *  \details        
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Value      voxels with value on or off
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setVoxelsZ(byte X, byte Y, byte Value)
{
    if(X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE && Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE)
    {
        for(byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
            bitWrite(NextFrame[Y][Z], X, bitRead(Value, Z));
        }
        return E_OK;
    } else {
        return E_NOT_OK;
    }
} /* setVoxelsZ */


/******************************************************************************************************************************************************
  getVoxelsXFast()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the x-axis of the frame buffer 
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[out]     Value      voxels with value on or off
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
byte LedCube<DataPin, ClockPin, StoragePin>::getVoxelsXFast(byte Y, byte Z)
{
    return CurrentFrame[Y][Z];
} /* getVoxelsXFast */


/******************************************************************************************************************************************************
  setVoxelsXFast()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the x-axis in the frame buffer with a given value
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      Y          y-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \return         voxels with value on or off
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsXFast(byte Y, byte Z, byte Value)
{
    NextFrame[Y][Z] = Value;
} /* setVoxelsXFast */


/******************************************************************************************************************************************************
  getVoxelsYFast()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the y-axis of the frame buffer 
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \param[out]     Value      voxels with value on or off
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
byte LedCube<DataPin, ClockPin, StoragePin>::getVoxelsYFast(byte X, byte Z)
{   
    byte Value = 0;

    for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
        bitWrite(Value, Y, bitRead(CurrentFrame[Y][Z], X));
    }
    return Value;
} /* getVoxelsYFast */


/******************************************************************************************************************************************************
  setVoxelsYFast()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the y-axis in the frame buffer with a given value
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Z          z-axis coordinate
 *  \return         voxels with value on or off
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsYFast(byte X, byte Z, byte Value)
{
    for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
        bitWrite(NextFrame[Y][Z], X, bitRead(Value, Y));
    }
} /* setVoxelsYFast */


/******************************************************************************************************************************************************
  getVoxelsZFast()
******************************************************************************************************************************************************/
/*! \brief          get a all voxels from the z-axis of the frame buffer 
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
byte LedCube<DataPin, ClockPin, StoragePin>::getVoxelsZFast(byte X, byte Y)
{
    byte Value = 0;

    for(byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        bitWrite(Value, Z, bitRead(CurrentFrame[Y][Z], X));
    }
    return Value;
} /* getVoxelsZFast */


/******************************************************************************************************************************************************
  setVoxelsZFast()
******************************************************************************************************************************************************/
/*! \brief          set all voxels from the z-axis in the frame buffer with a given value
 *  \details        no check for out of bounds
 *                  
 *  \param[in]      X          x-axis coordinate
 *  \param[in]      Y          y-axis coordinate
 *  \return         voxels with value on or off
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::setVoxelsZFast(byte X, byte Y, byte Value)
{
    for(byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        bitWrite(NextFrame[Y][Z], X, bitRead(Value, Z));
    }
} /* setVoxelsZFast */


/******************************************************************************************************************************************************
  setNextFrameReady()
******************************************************************************************************************************************************/
/*! \brief          set next frame to ready state
 *  \details        
 *                  
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setNextFrameReady()
{
    if(State == LEDCUBE_STATE_RUNNING) {
        NextFrameReady = true;
        return E_OK;
    } else {
        return E_NOT_OK;
    }
}


/******************************************************************************************************************************************************
  setNextFrameTimeElapsed()
******************************************************************************************************************************************************/
/*! \brief          set next frame time elapsed
 *  \details        
 *                  
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::setNextFrameTimeElapsed()
{
    if(State == LEDCUBE_STATE_RUNNING) {
        NextFrameTimeElapsed = true;
        return E_OK;
    } else {
        return E_NOT_OK;
    }
}


/******************************************************************************************************************************************************
 * P R I V A T E   F U N C T I O N S
 *****************************************************************************************************************************************************/

/******************************************************************************************************************************************************
  sendData()
******************************************************************************************************************************************************/
/*! \brief          send data to shift registers
 *  \details        this function shifts the given data to the shift registers 74HC595
 *
 *  \param[in]      Data    data (Byte) to send
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::sendData(byte Data)
{
    for (byte i = 0; i < 8; i++) {
        // LSB First
        //digitalWrite(DataInPin, !!(Data & (1 << i)));
        // MSB First
        DataOutput::Write(!!(Data & (1 << (7 - i))));
        // toggle clock pin
        ClockOutput::Write(HIGH);
        ClockOutput::Write(LOW);
    }
} /* sendData */


/******************************************************************************************************************************************************
  showLayer()
******************************************************************************************************************************************************/
/*! \brief          show a given layer on the cube
 *  \details        this function shows a given layer on the cube
 *                  
 *  \param[in]      Layer       layer which should be shown on the cube
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::showLayer(byte Layer)
{
    stdReturnType ReturnValue = E_NOT_OK;
    
    if(Layer < LEDCUBE_NUMBER_OF_LAYERS) {
        /* put on given layer */
        sendData(1U << Layer);
        /* send the layer data to the shift registers */
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            sendData(CurrentFrame[Y][Layer]);
        }
        // toggle storage pin
        StorageOutput::Write(HIGH);
        StorageOutput::Write(LOW);

        ReturnValue = E_OK;
    }
    return ReturnValue;
} /* showLayer */


/******************************************************************************************************************************************************
  showNextFrame()
******************************************************************************************************************************************************/
/*! \brief          show next frame on the cube
 *  \details        this function activates the next frame only if next frame is ready
 *                  function is called from timer interrupt normally
 *  \return         E_OK
 *                  E_NOT_OK
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
stdReturnType LedCube<DataPin, ClockPin, StoragePin>::showNextFrame()
{
    switchBufferPointer();
    return E_OK;
}


/******************************************************************************************************************************************************
  switchBufferPointer()
******************************************************************************************************************************************************/
/*! \brief          switch the two frame buffers
 *  \details        this function switches the two frame buffers CubeBuffer1 and CubeBuffer2
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
template <uint8_t DataPin, uint8_t ClockPin, uint8_t StoragePin>
void LedCube<DataPin, ClockPin, StoragePin>::switchBufferPointer()
{
        pFrameType pTmp;

        pTmp = CurrentFrame;
        CurrentFrame = NextFrame;
        NextFrame = pTmp;
} /* switchBufferPointer */


#endif

/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/
