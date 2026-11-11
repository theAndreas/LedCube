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
class Transformation
{
  private:
    //stdReturnType shiftCircle(LedCube*, LedCubeAxisType, TransformationRotationDirectionType, byte);
    LedCube* pCube;
   
  public:
    Transformation(LedCube* pLedCube);
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

#endif

/******************************************************************************************************************************************************
 *  E N D   O F   F I L E
 *****************************************************************************************************************************************************/