#include <iostream>
#include <stdio.h>

#define LEDCUBE_NUMBER_OF_LEDS_PER_SIDE     8

using namespace std;


// constant macros
/* Bit setzen */
#define setBit(Var, Bit) \
((Var) |= (1 << (Bit)))

/* Bit lˆschen */
#define clearBit(Var, Bit) \
((Var) &= (unsigned)~(1 << (Bit)))

/* Bit togglen */
#define toggleBit(Var, Bit) \
((Var) ^= (1 << (Bit)))

/* Bit Operationen */
#define readBit(Var, Bit) \
(((Var) & (1 << (Bit))) >> Bit)

#define writeBit(Var, Bit, Value) \
((Var) = ((Var & ~(1 << Bit)) | (Value << Bit)))

#define isBitSet(Var, Bit) \
((Var) & (1 << (Bit)))

#define isBitCleared(Var, Bit) \
!isBitSet(Var, Bit)

/* Bit Gruppen abfragen */
#define readRegisterBitGroup(Register, BitGroupMask, BitGroupPosition) \
((Register & ((uint8_t)BitGroupMask)) >> BitGroupPosition)

/* Bit Gruppen setzen */
#define writeRegisterBitGroup(Register, BitGroupMask, BitGroupPosition, Value) \
(Register = ((Register & ~((uint8_t)BitGroupMask)) | ((Value << BitGroupPosition) & ((uint8_t)BitGroupMask))))

// arduino macros
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


#define B(x) ( \
  (0##x >>  0 & 0001) | \
  (0##x >>  2 & 0002) | \
  (0##x >>  4 & 0004) | \
  (0##x >>  6 & 0010) | \
  (0##x >>  8 & 0020) | \
  (0##x >> 10 & 0040) | \
  (0##x >> 12 & 0100) | \
  (0##x >> 14 & 0200) )


// type definition
typedef char byte;

// variable declaration
byte CubeBuffer1[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE][LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];
byte CubeBuffer2[LEDCUBE_NUMBER_OF_LEDS_PER_SIDE][LEDCUBE_NUMBER_OF_LEDS_PER_SIDE];

// function declaration
void setVoxelFastStart(byte X, byte Y, byte Z) { bitSet(CubeBuffer1[Y][Z], X); }
void setVoxelFast(byte X, byte Y, byte Z) { bitSet(CubeBuffer2[Y][Z], X); }
void clearVoxelFast(byte X, byte Y, byte Z) { bitClear(CubeBuffer2[Y][Z], X); }
void writeVoxelFast(byte X, byte Y, byte Z, bool Value) { bitWrite(CubeBuffer2[Y][Z], X, Value); }
byte getVoxelFast(byte X, byte Y, byte Z) { return bitRead(CubeBuffer1[Y][Z], X); }

byte getVoxelFastAusgabe(byte X, byte Y, byte Z) { return bitRead(CubeBuffer2[Y][Z], X); }


char Voxel;

int main()
{
    const int LEDs = LEDCUBE_NUMBER_OF_LEDS_PER_SIDE;
    setVoxelFastStart(1,5,0);
    setVoxelFastStart(6,5,0);
    setVoxelFastStart(1,1,0);
    setVoxelFastStart(0,0,0);
    setVoxelFastStart(7,7,0);
    setVoxelFastStart(5,7,0);

    setVoxelFastStart(3,2,0);
    setVoxelFastStart(3,3,0);

    // original
    cout << "original" << endl;
    for(int X = 0; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; X++) {
        for(int Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            Voxel = getVoxelFast(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - X - 1, Y, 0);
             printf("%d,", Voxel);
        }
        printf("\n");
    }

    int Circle = 4;

    Circle--;

    // rechts drehen
    // vorne
    //for(int Y = Circle; Y < LEDs; Y++) writeVoxelFast(Circle, Y, 0, getVoxelFast(Circle, Y+1, 0));
    // rechts
    //for(int X = Circle; X < LEDs; X++) writeVoxelFast(X, LEDs-1-Circle, 0, getVoxelFast(X+1, LEDs-1-Circle, 0));
    // hinten
    //for(int Y = LEDs-1-Circle; Y > Circle; Y--) writeVoxelFast(LEDs-1-Circle, Y, 0, getVoxelFast(LEDs-1-Circle, Y-1, 0));
    // links
    //for(int X = LEDs-1-Circle; X > Circle; X--) writeVoxelFast(X, Circle, 0, getVoxelFast(X-1, Circle, 0));


    // links drehen
    // links
    for(int X = Circle; X < LEDs-1-Circle; X++) writeVoxelFast(X, Circle, 0, getVoxelFast(X+1, Circle, 0));
    // hinten
    for(int Y = Circle; Y < LEDs-1-Circle; Y++) writeVoxelFast(LEDs-1-Circle, Y, 0, getVoxelFast(LEDs-1-Circle, Y+1, 0));
    // rechts
    for(int X = LEDs-1-Circle; X > Circle; X--) writeVoxelFast(X, LEDs-1-Circle, 0, getVoxelFast(X-1, LEDs-1-Circle, 0));
    // vorne
    for(int Y = LEDs-1-Circle; Y > Circle; Y--) writeVoxelFast(Circle, Y, 0, getVoxelFast(Circle, Y-1, 0));


    cout << endl;
    cout << "transform" << endl;
    for(int X = 0; X < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; X++) {
        for(int Y = 0; Y < LEDCUBE_NUMBER_OF_LEDS_PER_SIDE; Y++) {
            Voxel = getVoxelFastAusgabe(LEDCUBE_NUMBER_OF_LEDS_PER_SIDE - X - 1, Y, 0);
             printf("%d,", Voxel);
        }
        printf("\n");
    }

    return 0;
}

//    // rechts drehen
//
//    // 1. Kreis auﬂen
//    // vorne
//    for(int Y = 0; Y < LEDs; Y++) writeVoxelFast(0, Y, 0, getVoxelFast(0, Y+1, 0));
//    // rechts
//    for(int X = 0; X < LEDs; X++) writeVoxelFast(X, LEDs-1, 0, getVoxelFast(X+1, LEDs-1, 0));
//    // hinten
//    for(int Y = LEDs-1; Y > 0; Y--) writeVoxelFast(LEDs-1, Y, 0, getVoxelFast(LEDs-1, Y-1, 0));
//    // links
//    for(int X = LEDs-1; X > 0; X--) writeVoxelFast(X, 0, 0, getVoxelFast(X-1, 0, 0));
//
//    // 2. Kreis auﬂen
//    // vorne
//    for(int Y = 1; Y < LEDs-1; Y++) writeVoxelFast(1, Y, 0, getVoxelFast(0, Y+1, 0));
//    // rechts
//    for(int X = 1; X < LEDs-1; X++) writeVoxelFast(X, LEDs-2, 0, getVoxelFast(X+1, LEDs-2, 0));
//    // hinten
//    for(int Y = LEDs-2; Y > 1; Y--) writeVoxelFast(LEDs-2, Y, 0, getVoxelFast(LEDs-2, Y-1, 0));
//    // links
//    for(int X = LEDs-2; X > 1; X--) writeVoxelFast(X, 1, 0, getVoxelFast(X-1, 1, 0));
