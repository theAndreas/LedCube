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
/**     \file       LedCube.c
 *      \brief      Main file of LedCube library
 *
 *      \details    Arduino library to drive an 8x8x8 LED Cube
 *                  
 *
 *****************************************************************************************************************************************************/
#define _LECUBE_SOURCE_

/******************************************************************************************************************************************************
 * INCLUDES
 *****************************************************************************************************************************************************/
#include "LedCube.h"

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
LedCube::LedCube()
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
LedCube::~LedCube()
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
void LedCube::init()
{
    State = LEDCUBE_STATE_INIT;
    clearCube();
    pinModeFast(LEDCUBE_DATA_IN_PIN, OUTPUT);
    pinModeFast(LEDCUBE_CLOCK_PIN, OUTPUT);
    pinModeFast(LEDCUBE_STORAGE_PIN, OUTPUT);
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
stdReturnType LedCube::start()
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
stdReturnType LedCube::stop()
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
void LedCube::task()
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
void LedCube::clearCube()
{
    digitalWriteFast(LEDCUBE_DATA_IN_PIN, LOW);

    for(byte i = 0; i < (LEDCUBE_NUMBER_OF_SHIFT_REGISTERS * LEDCUBE_SHIFT_REGISTER_BITWIDTH); i++) {
        digitalWriteFast(LEDCUBE_CLOCK_PIN, HIGH);
        digitalWriteFast(LEDCUBE_CLOCK_PIN, LOW);
    }
    digitalWriteFast(LEDCUBE_STORAGE_PIN, HIGH);
    digitalWriteFast(LEDCUBE_STORAGE_PIN, LOW);
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
stdReturnType LedCube::setVoxel(byte X, byte Y, byte Z)
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
stdReturnType LedCube::clearVoxel(byte X, byte Y, byte Z)
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
stdReturnType LedCube::writeVoxel(byte X, byte Y, byte Z, boolean Value)
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
stdReturnType LedCube::getVoxel(byte X, byte Y, byte Z, boolean* Value)
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
stdReturnType LedCube::getVoxelsX(byte Y, byte Z, byte* Value)
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
stdReturnType LedCube::setVoxelsX(byte Y, byte Z, byte Value)
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
stdReturnType LedCube::getVoxelsY(byte X, byte Z, byte* Value)
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
stdReturnType LedCube::setVoxelsY(byte X, byte Z, byte Value)
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
stdReturnType LedCube::getVoxelsZ(byte X, byte Y, byte* Value)
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
stdReturnType LedCube::setVoxelsZ(byte X, byte Y, byte Value)
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
byte LedCube::getVoxelsXFast(byte Y, byte Z)
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
void LedCube::setVoxelsXFast(byte Y, byte Z, byte Value)
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
byte LedCube::getVoxelsYFast(byte X, byte Z)
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
void LedCube::setVoxelsYFast(byte X, byte Z, byte Value)
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
byte LedCube::getVoxelsZFast(byte X, byte Y)
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
void LedCube::setVoxelsZFast(byte X, byte Y, byte Value)
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
stdReturnType LedCube::setNextFrameReady()
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
stdReturnType LedCube::setNextFrameTimeElapsed()
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
void LedCube::sendData(byte Data)
{
    for (byte i = 0; i < 8; i++) {
        // LSB First
        //digitalWrite(DataInPin, !!(Data & (1 << i)));
        // MSB First
        digitalWriteFast(LEDCUBE_DATA_IN_PIN, !!(Data & (1 << (7 - i))));
        // toggle clock pin
        digitalWriteFast(LEDCUBE_CLOCK_PIN, HIGH);
        digitalWriteFast(LEDCUBE_CLOCK_PIN, LOW);
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
stdReturnType LedCube::showLayer(byte Layer)
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
        digitalWriteFast(LEDCUBE_STORAGE_PIN, HIGH);
        digitalWriteFast(LEDCUBE_STORAGE_PIN, LOW);

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
stdReturnType LedCube::showNextFrame()
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
void LedCube::switchBufferPointer()
{
        pFrameType pTmp;

        pTmp = CurrentFrame;
        CurrentFrame = NextFrame;
        NextFrame = pTmp;
} /* switchBufferPointer */


/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/
 