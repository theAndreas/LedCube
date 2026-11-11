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
#include "StandardTypes.h"
#include "Arduino.h"
#include "digitalWriteFast.h"


/******************************************************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 *****************************************************************************************************************************************************/
/* LedCube Configuration Parameter */
#define LEDCUBE_DATA_IN_PIN                         13
#define LEDCUBE_CLOCK_PIN                           11
#define LEDCUBE_STORAGE_PIN                         12

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
class LedCube
{
  private:
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

#endif

/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/
