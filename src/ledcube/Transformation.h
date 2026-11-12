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
/**     \file       Transformation.h
 *      \brief      Main header file of LedCube Transformation library
 *
 *      \details    Arduino Animation library for 8x8x8 LED Cube
 *                  
 *
 *****************************************************************************************************************************************************/
#ifndef _TRANSFORMATION_H_
#define _TRANSFORMATION_H_

/******************************************************************************************************************************************************
 * INCLUDES
 *****************************************************************************************************************************************************/
#include "StandardTypes.h"
#include "Arduino.h"
#include "LedCube.h"

/******************************************************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 *****************************************************************************************************************************************************/


/******************************************************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 *****************************************************************************************************************************************************/


/******************************************************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 *****************************************************************************************************************************************************/
/* type which describes rotation in degrees */
typedef enum {
    TRANSFORMATION_ROTATION_DEGREE_0,
    TRANSFORMATION_ROTATION_DEGREE_90,
    TRANSFORMATION_ROTATION_DEGREE_180,
    TRANSFORMATION_ROTATION_DEGREE_270
} TransformationRotationDegreeType;

/* type which describes direction of rotation */
typedef enum {
    TRANSFORMATION_ROTATION_CLOCKWISE,
    TRANSFORMATION_ROTATION_COUNTERCLOCKWISE
} TransformationRotationDirectionType;


/******************************************************************************************************************************************************
 *  CLASS  LedCube
 *****************************************************************************************************************************************************/
/*!
 * \brief Shifts and turns the voxels of a cube.
 *
 * \code{.cpp}
 * LedCube<13, 11, 12> cube;
 * Transformation<decltype(cube)> transformation(&cube);
 * \endcode
 *
 * \tparam Cube The type of the cube, a `LedCube`.
 */
template <typename Cube>
class Transformation
{
  private:
    //stdReturnType shiftCircle(LedCube*, LedCubeAxisType, TransformationRotationDirectionType, byte);
    Cube* pCube;
   
  public:
    Transformation(Cube* pLedCube);
    ~Transformation();

    stdReturnType shiftCircle(LedCubeAxisType, TransformationRotationDirectionType, byte);
    void rotate(LedCubeAxisType ShiftAxis, TransformationRotationDegreeType Degrees);
    void mirror(LedCubeAxisType ShiftAxis);
    stdReturnType rotateWithShift(LedCubeAxisType ShiftAxis, TransformationRotationDirectionType Direction, byte Clock);
    void shiftForward(boolean = false);
    void shiftBackward(boolean = false);
    void shiftUpward(boolean = false);
    void shiftDownward(boolean = false);
    void shiftLeft(boolean = false);
    void shiftRight(boolean = false);

    void shiftForwardFast(boolean = false);
    void shiftBackwardFast(boolean = false);
};


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
template <typename Cube>
Transformation<Cube>::Transformation(Cube* pLedCube)
{
    pCube = pLedCube;
} /* Transformation */


/******************************************************************************************************************************************************
  DESTRUCTOR OF Transformation
******************************************************************************************************************************************************/
template <typename Cube>
Transformation<Cube>::~Transformation()
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
template <typename Cube>
void Transformation<Cube>::shiftForward(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftBackward(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftUpward(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftDownward(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftLeft(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftRight(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftForwardFast(boolean Rotate)
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
template <typename Cube>
void Transformation<Cube>::shiftBackwardFast(boolean Rotate)
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
template <typename Cube>
stdReturnType Transformation<Cube>::shiftCircle(LedCubeAxisType ShiftAxis, TransformationRotationDirectionType Direction, byte Circle)
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


#endif

/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/