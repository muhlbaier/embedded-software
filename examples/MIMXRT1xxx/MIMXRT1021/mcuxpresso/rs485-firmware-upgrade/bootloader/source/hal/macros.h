/**
 * @defgroup macros Macros
 * @ingroup helpers
 *
 * File:   macros.h
 *
 * This header provides macros used to assist in writing reusable code.
 *  
 * Created on: Mar 12, 2014
 *      Author: Michael Muhlbaier
 * Updated on: Feb 7, 2015
 *      Author: Anthony Merlino
 * @{
 */

#ifndef _MACROS_H_
#define	_MACROS_H_

// special ASCII definitions
#define ASCII_BACKSPACE 0x08 ///< backspace code
#define ASCII_LF 0x0A ///< line feed code
#define ASCII_NL ASCII_LF ///< new line code
#define ASCII_CR 0x0D ///< carrage return code
#define ASCII_DEL 0x7F ///< delete code
#define ASCII_ESC 0x1B ///< escape character code

/** concatenate 2 strings together
 *
 * Useful for making reusable defines
 * For example, the timing module uses this macro to concatenate the letters PR
 * with the TIMING_TIMER number to make the period register define
 * @code
 * #define TIMER_PERIOD CAT2(PR,TIMING_TIMER)
 * // if TIMING_TIMER is 2 this would be equivalent to
 * #define TIMER_PERIOD PR2
 * // but using CAT2 allows the user to just specify which timer they want to
 * // use and the defines in timing.h take care of the rest automatically.
 * @endcode
 */
#define CAT2(s1,s2) CAT2B(s1,s2)
#define CAT2B(s1,s2) s1##s2 ///< helper for CAT2

/** concatenate 3 strings together
 * 
 * Useful for making reusable defines
 * For example, the timing module uses this macro to concatenate the letter T
 * with the TIMING_TIMER number with the letters CON:
 * @code
 * #define ConfigureTimer() CAT3(T,TIMING_TIMER,CON) = 0xA010
 * // if TIMING_TIMER is 2 this would be equivalent to
 * #define ConfigureTimer() T2CON = 0xA010
 * // but using CAT3 allows the user to just specify which timer they want to
 * // use and the defines in timing.h take care of the rest automatically.
 * @endcode
 */
#define CAT3(s1,s2,s3) CAT3B(s1,s2,s3)
#define CAT3B(s1,s2,s3) s1##s2##s3 ///< helper for CAT3

/** concatenate 4 strings together
 * 
 * Useful for making reusable defines
 * For example:
 * @code
 * #define TIMER_INTERRUPT_FLAG CAT4(TIMING_IFS,bits.T,TIMING_TIMER,IF)
 * // if TIMING_TIMER is 2 and TIMING_IFS is IFS0 this would be equivalent to
 * #define TIMER_INTERRUPT_FLAG IFS0bits.T2IF
 * @endcode
 */
#define CAT4(s1,s2,s3,s4) CAT4B(s1,s2,s3,s4)
#define CAT4B(s1,s2,s3,s4) s1##s2##s3##s4 ///< helper for CAT4

/** concatenate 5 strings together
 * 
 * Useful for making reusable defines
 * For example:
 * @code
 * #define TIMER_INTERRUPT_FLAG CAT4(TIMING_IFS,bits.T,TIMING_TIMER,IF)
 * // if TIMING_TIMER is 2 and TIMING_IFS is IFS0 this would be equivalent to
 * #define TIMER_INTERRUPT_FLAG IFS0bits.T2IF
 * @endcode
 */
#define CAT5(s1,s2,s3,s4,s5) CAT5B(s1,s2,s3,s4,s5)
#define CAT5B(s1,s2,s3,s4,s5) s1##s2##s3##s4##s5 ///< helper for CAT5

/** @} */
#endif	/* _MACROS_H_ */

