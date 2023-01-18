/** 
 * File:   int_def.h
 *
 * This header includes stdint.h which includes the following compiler independent
 * definitions:
 * - uint8_t Unsigned Integer 8-bit
 * - uint16_t Unsigned Integer 16-bit
 * - uint32_t Unsigned Integer 32-bit
 * - uint64_t Unsigned Integer 64-bit
 * - int8_t Signed Integer 8-bit
 * - int16_t Signed Integer 16-bit
 * - int32_t Signed Integer 32-bit
 * - int64_t Signed Integer 64-bit
 *
 * Addionally union types are defined to assist with integer manipulation at the
 * word, byte, or bit level. Included are:
 * - union8_t 8-bit union with byte and bitfield access
 * - union16_t 16-bit union with word, byte array, and bitfield access
 * - union32_t 32-bit union with phrase, word array, byte array, and bitfield access
 *
 * @author Embedded Processors Class 2013
 * @date Created on February 13, 2013, 5:53 PM
 * @addtogroup int_def Integer Definitions
 * @{
 */

#ifndef INT_DEF_H
#define	INT_DEF_H

// add compiler int defs
#include <stdint.h>

/** 8-bit union type
 *
 * Example usage
 * @code
 * union8_t x;
 * x.b = 1; // same as x.bits.b0 = 1
 * x.bits.b2 = 1; // same as x.b |= 4
 * // x now equals 5
 * @endcode
 */
typedef union {
    uint8_t b; ///< byte member
    struct {
        uint8_t b0:1;
        uint8_t b1:1;
        uint8_t b2:1;
        uint8_t b3:1;
        uint8_t b4:1;
        uint8_t b5:1;
        uint8_t b6:1;
        uint8_t b7:1;
    } bits; ///< bitfield member, b0 LSb, b7 MSb
    struct {
        uint8_t low:4;
        uint8_t high:4;
    } nibble;
} union8_t;

/** 16-bit union type
 *
 * Example usage
 * @code
 * union16_t x;
 * x.word = 1; // same as x.bits.b0 = 1 or x.b[0] = 1
 * x.bits.b2 = 1; // same as x.b |= 4
 * // x now equals 5
 * @endcode
 */
typedef union {
    uint16_t word;
    uint8_t b[2]; ///< byte array member, b[0] LSB, b[1] MSB
    union8_t ub[2]; ///< 8-bit union array member, @see union8_t
    struct {
        uint16_t b0:1;
        uint16_t b1:1;
        uint16_t b2:1;
        uint16_t b3:1;
        uint16_t b4:1;
        uint16_t b5:1;
        uint16_t b6:1;
        uint16_t b7:1;
        uint16_t b8:1;
        uint16_t b9:1;
        uint16_t b10:1;
        uint16_t b11:1;
        uint16_t b12:1;
        uint16_t b13:1;
        uint16_t b14:1;
        uint16_t b15:1;
    } bits; ///< bitfield member, b0 LSb, b15 MSb
} union16_t;

/**
 */
typedef union {
    uint32_t phrase; ///< 32-bit integer member
    uint16_t word[2]; ///< word array member, word[0] LSW, b[1] MSW
    uint8_t b[4]; ///< byte array member, b[0] LSB, b[3] MSB
    union16_t uword[2]; ///< 16-bit union array member, @see union16_t
    union8_t ub[4]; ///< 8-bit union array member, @see union8_t
    float f;
    struct {
        uint32_t b0:1;
        uint32_t b1:1;
        uint32_t b2:1;
        uint32_t b3:1;
        uint32_t b4:1;
        uint32_t b5:1;
        uint32_t b6:1;
        uint32_t b7:1;
        uint32_t b8:1;
        uint32_t b9:1;
        uint32_t b10:1;
        uint32_t b11:1;
        uint32_t b12:1;
        uint32_t b13:1;
        uint32_t b14:1;
        uint32_t b15:1;
        uint32_t b16:1;
        uint32_t b17:1;
        uint32_t b18:1;
        uint32_t b19:1;
        uint32_t b20:1;
        uint32_t b21:1;
        uint32_t b22:1;
        uint32_t b23:1;
        uint32_t b24:1;
        uint32_t b25:1;
        uint32_t b26:1;
        uint32_t b27:1;
        uint32_t b28:1;
        uint32_t b29:1;
        uint32_t b30:1;
        uint32_t b31:1;
    } bits; ///< bitfield member, b0 LSb, b31 MSb
} union32_t;

/**
 */
typedef union {
    uint64_t quad_word;
    uint32_t double_word[2]; ///< 32-bit integer member
    uint16_t word[4]; ///< word array member, word[0] LSW, b[1] MSW
    uint8_t b[8]; ///< byte array member, b[0] LSB, b[3] MSB
    union32_t udouble_word[2];
    union16_t uword[4]; ///< 16-bit union array member, @see union16_t
    union8_t ub[8]; ///< 8-bit union array member, @see union8_t
    double f;
} union64_t;
/** @} */
#endif	/* INT_DEF_H */

