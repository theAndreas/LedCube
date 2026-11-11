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
/**     \file       Transformation.c
 *      \brief      Main file of Transformation library
 *
 *      \details    Arduino Transformation library for 8x8x8 LED Cube
 *                  
 *
 *****************************************************************************************************************************************************/
#define _TRANSFORMATION_SOURCE_

/******************************************************************************************************************************************************
 * INCLUDES
 *****************************************************************************************************************************************************/
#include "Transformation.h"


/******************************************************************************************************************************************************
 * P U B L I C   F U N C T I O N S
 *****************************************************************************************************************************************************/

/******************************************************************************************************************************************************
  CONSTRUCTOR OF Transformation
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *
 *  \param[in]      
 *  \param[in]      
 *  \param[in]      
 *  \return         -
 *****************************************************************************************************************************************************/
Transformation::Transformation(LedCube* pLedCube)
{
    pCube = pLedCube;
} /* Transformation */


/******************************************************************************************************************************************************
  DESTRUCTOR OF Transformation
******************************************************************************************************************************************************/
Transformation::~Transformation()
{

} /* ~Transformation */


/******************************************************************************************************************************************************
  shiftForward()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftForward(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte X = 0; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; X++) {
            pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X + 1, Z));
        }
        if(Rotate) pCube->setVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, pCube->getVoxelsYFast(0, Z));
        else pCube->setVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, 0);
    }
} /* shiftForward */


/******************************************************************************************************************************************************
  shiftBackward()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftBackward(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte X = 1; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; X++) {
            pCube->setVoxelsYFast(X, Z, pCube->getVoxelsYFast(X - 1, Z));
        }
        if(Rotate) pCube->setVoxelsYFast(0, Z, pCube->getVoxelsYFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z));
        else pCube->setVoxelsYFast(0, Z, 0);
    }
} /* shiftBackward */


/******************************************************************************************************************************************************
  shiftUpward()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftUpward(boolean Rotate)
{
    for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
        for (byte Z = 1; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z - 1));
        }
        if(Rotate) pCube->setVoxelsXFast(Y, 0, pCube->getVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1));
        else pCube->setVoxelsXFast(Y, 0, 0);
    }
} /* shiftUpward */


/******************************************************************************************************************************************************
  shiftDownward()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftDownward(boolean Rotate)
{
    for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; Y++) {
        for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z + 1));
        }
        if(Rotate) pCube->setVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, pCube->getVoxelsXFast(Y, 0));
        else pCube->setVoxelsXFast(Y, LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, 0);
    }
} /* shiftDownward */


/******************************************************************************************************************************************************
  shiftLeft()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftLeft(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1; Y++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y + 1, Z));
        }
        if(Rotate) pCube->setVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, pCube->getVoxelsXFast(0, Z));
        else pCube->setVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z, 0);
    }
} /* shiftLeft */


/******************************************************************************************************************************************************
  shiftRight()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftRight(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte Y = 1; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y - 1, Z));
        }
        if(Rotate) pCube->setVoxelsXFast(0, Z, pCube->getVoxelsXFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Z));
        else pCube->setVoxelsXFast(0, Z, 0);
    }
} /* shiftRight */


/******************************************************************************************************************************************************
  shiftForwardFast()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftForwardFast(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) >> 1);
            if(Rotate) pCube->writeVoxelFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Y, Z, pCube->getVoxelFast(0, Y, Z));
        }
    }
} /* shiftForwardFast */


/******************************************************************************************************************************************************
  shiftBackwardFast()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
void Transformation::shiftBackwardFast(boolean Rotate)
{
    for (byte Z = 0; Z < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Z++) {
        for(byte Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            pCube->setVoxelsXFast(Y, Z, pCube->getVoxelsXFast(Y, Z) << 1);
            if(Rotate) pCube->writeVoxelFast(0, Y, Z, pCube->getVoxelFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - 1, Y, Z));
        }
    }
} /* shiftBackwardFast */


/******************************************************************************************************************************************************
 * P R I V A T E   F U N C T I O N S
 *****************************************************************************************************************************************************/

/******************************************************************************************************************************************************
  shiftCircle()
******************************************************************************************************************************************************/
/*! \brief          
 *  \details        
 *                  
 *  \return         -
 *****************************************************************************************************************************************************/
stdReturnType Transformation::shiftCircle(LedCubeAxisType ShiftAxis, TransformationRotationDirectionType Direction, byte Circle)
{   
    const byte LEDs = LEDCUBE_NUMBER_OF_LEDS_PER_SIDE;

	if(Circle > 0 && Circle <= LEDs / 2) {
		Circle--;
		if(TRANSFORMATION_ROTATION_CLOCKWISE == Direction) {
			if(LEDCUBE_AXIS_Z == ShiftAxis) {
				for(byte Z = 0; Z < LEDs; Z++) {
					/* shift front, right, back, left */
					for(int8_t Y = Circle; Y < LEDs - 1 - Circle; Y++) {
                        pCube->writeVoxelFast(Circle, Y, Z, pCube->getVoxelFast(Circle, Y + 1, Z));
                    }
					for(int8_t X = Circle; X < LEDs - 1 - Circle; X++) {
                        pCube->writeVoxelFast(X, LEDs - 1 - Circle, Z, pCube->getVoxelFast(X + 1, LEDs - 1 - Circle, Z));
                    }
					for(int8_t Y = LEDs -1 - Circle; Y > Circle; Y--) {
                        pCube->writeVoxelFast(LEDs - 1 - Circle, Y, Z, pCube->getVoxelFast(LEDs - 1 - Circle, Y - 1, Z));
                    }
					for(int8_t X = LEDs -1 - Circle; X > Circle; X--) {
                        pCube->writeVoxelFast(X, Circle, Z, pCube->getVoxelFast(X - 1, Circle, Z));
                    }
				}
			}
		}
		if(TRANSFORMATION_ROTATION_COUNTERCLOCKWISE == Direction) {
			if(LEDCUBE_AXIS_Z == ShiftAxis) {
				for(byte Z = 0; Z < LEDs; Z++) {
					/* shift left, back, right, front */
					for(int8_t X = Circle; X < LEDs - 1 - Circle; X++) {
                        pCube->writeVoxelFast(X, Circle, Z, pCube->getVoxelFast(X + 1, Circle, Z));
                    }
					for(int8_t Y = Circle; Y < LEDs - 1 - Circle; Y++) {
                        pCube->writeVoxelFast(LEDs - 1 - Circle, Y, Z, pCube->getVoxelFast(LEDs - 1 - Circle, Y + 1, Z));
                    }
					for(int8_t X = LEDs - 1 - Circle; X > Circle; X--) {
                        pCube->writeVoxelFast(X, LEDs - 1 - Circle, Z, pCube->getVoxelFast(X - 1, LEDs - 1 - Circle, Z));
                    }
					for(int8_t Y = LEDs - 1 - Circle; Y > Circle; Y--) {
                        pCube->writeVoxelFast(Circle, Y, Z, pCube->getVoxelFast(Circle, Y - 1, Z));
                    }
				}
			}
		}
		return E_OK;
	} 
	else { 
		return E_NOT_OK;
	}
} /* shiftCircle */

/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/
 