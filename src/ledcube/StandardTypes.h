// Copyright 2017 Andreas Burnickl
// SPDX-License-Identifier: GPL-3.0-or-later

/*!
 * \file
 * \brief The library's return type, switches and bit macros.
 */

#ifndef _STANDARD_TYPES_H_
#define _STANDARD_TYPES_H_

/* standard type for configuration */
#define STD_ON 1u
#define STD_OFF 0u

#define STD_NULL_CHARACTER '\0'

/* set bit */
#define setBit(Var, Bit) ((Var) |= (1 << (Bit)))

/* clear bit */
#define clearBit(Var, Bit) ((Var) &= (unsigned)~(1 << (Bit)))

/* toggle bit */
#define toggleBit(Var, Bit) ((Var) ^= (1 << (Bit)))

/* read bit */
#define readBit(Var, Bit) (((Var) & (1 << (Bit))) >> Bit)

/* write bit */
#define writeBit(Var, Bit, Value) \
  ((Var) = ((Var & (unsigned)~(1 << Bit)) | (Value << Bit)))

/* is bit set */
#define isBitSet(Var, Bit) ((Var) & (1 << (Bit)))

/* is bit cleared */
#define isBitCleared(Var, Bit) !isBitSet(Var, Bit)

/* read Bit Group */
#define readBitGroup(Var, BitGroupMask, BitGroupPosition) \
  (Var = ((Var & ((uint8_t)BitGroupMask)) >> BitGroupPosition))

/* write Bit Group */
#define writeBitGroup(Var, BitGroupMask, BitGroupPosition, Value) \
  (Var = ((Var & ~((uint8_t)BitGroupMask)) |                      \
          ((Value << BitGroupPosition) & ((uint8_t)BitGroupMask))))

/* binary to decimal */
#define B(x)                                                       \
  ((0##x >> 0 & 0001) | (0##x >> 2 & 0002) | (0##x >> 4 & 0004) |  \
   (0##x >> 6 & 0010) | (0##x >> 8 & 0020) | (0##x >> 10 & 0040) | \
   (0##x >> 12 & 0100) | (0##x >> 14 & 0200))

/* standard return type for functions */
typedef enum { E_OK = 0, E_NOT_OK = 1 } stdReturnType;

#endif
