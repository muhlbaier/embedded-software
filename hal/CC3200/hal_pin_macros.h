// Contains macros.h include
#include "library.h"

/** These macros assume you are using the GPIO number of the pin as the input.  Do not use the pin
 * number.  Also do not append 0 in front of single digit GPIO numbers.
 *
 * 		Ex. GPIO2 is PIN_57 so use GET_GPIO_PORT(2)
 */
#define GET_GPIO_PORT(x) (GPIOA0_BASE + (0x1000 * (x/8)))
#define GET_GPIO_PORT_PERIPHERAL(x) (PRCM_GPIOA0 + (x/8))
#define GET_GPIO_BIT(x) (1 << (x % 8))
#define GET_GPIO_PIN(x) CAT3(GPIO, x, _PIN_NUM)

#define GPIO_READ(x) ((GPIOPinRead(GET_GPIO_PORT(x), GET_GPIO_BIT(x)) >> (x % 8)) & 0x01)

/******************************************************
 * Macros for modes of each pin
 *****************************************************/

#define GPIO0_UART0_RTS		PIN_MODE_3
#define GPIO0_McAXR0		PIN_MODE_4
#define GPIO0_McAXR1		PIN_MODE_6
#define GPIO0_GT_CCP00		PIN_MODE_7
#define GPIO0_GSPI_CS 		PIN_MODE_9
#define GPIO0_UART1_RTS		PIN_MODE_10
#define GPIO0_UART0_CTS	 	PIN_MODE_12

#define GPIO1_UART0_TX 		PIN_MODE_3
#define GPIO1_PIXCLK 		PIN_MODE_4
#define GPIO1_UART1_TX		PIN_MODE_6
#define GPIO1_GT_CCP01 		PIN_MODE_7

#define GPIO2_UART0_RX 		PIN_MODE_3
#define GPIO2_UART1_RX		PIN_MODE_6
#define GPIO2_GT_CCP02 		PIN_MODE_7

#define GPIO3_CAM_D3 		PIN_MODE_4
#define GPIO3_UART1_TX		PIN_MODE_6

#define GPIO4_CAM_D2 		PIN_MODE_4
#define GPIO4_UART1_RX		PIN_MODE_6

#define GPIO5_CAM_D1 		PIN_MODE_4
#define GPIO5_McAXR1		PIN_MODE_6
#define GPIO5_GT_CCP05 		PIN_MODE_7

#define GPIO6_UART1_CTS		PIN_MODE_3
#define GPIO6_CAM_D0 		PIN_MODE_4
#define GPIO6_UART0_RTS		PIN_MODE_5
#define GPIO6_UART0_CTS		PIN_MODE_6
#define GPIO6_GT_CCP06 		PIN_MODE_7

#define GPIO7_UART1_RTS		PIN_MODE_3
#define GPIO7_UART0_RTS		PIN_MODE_10
#define GPIO7_UART0_TX		PIN_MODE_11
#define GPIO7_McACLKX 		PIN_MODE_13

#define GPIO8_SDCARD_IRQ	PIN_MODE_6
#define GPIO8_McAFSX		PIN_MODE_7
#define GPIO8_GT_CCP06 		PIN_MODE_12

#define GPIO9_GT_PWM05		PIN_MODE_3
#define GPIO9_SDCARD_D0		PIN_MODE_6
#define GPIO9_McAXR0		PIN_MODE_7
#define GPIO9_GT_CCP00 		PIN_MODE_12

#define GPIO10_I2C_SCL		PIN_MODE_1
#define GPIO10_GT_PWM06		PIN_MODE_3
#define GPIO10_SDCARD_CLK	PIN_MODE_6
#define GPIO10_UART1_TX		PIN_MODE_7
#define GPIO10_GT_CCP01 	PIN_MODE_12

#define GPIO11_I2C_SDA		PIN_MODE_1
#define GPIO11_GT_PWM07		PIN_MODE_3
#define GPIO11_XVCLK		PIN_MODE_4
#define GPIO11_SDCARD_CMD	PIN_MODE_6
#define GPIO11_UART1_RX 	PIN_MODE_7
#define GPIO11_GT_CCP02		PIN_MODE_12
#define GPIO11_McAFSX	 	PIN_MODE_13

#define GPIO12_McACLK		PIN_MODE_3
#define GPIO12_VSYNC		PIN_MODE_4
#define GPIO12_I2C_SCL		PIN_MODE_5
#define GPIO12_UART0_TX		PIN_MODE_7
#define GPIO12_GT_CCP03 	PIN_MODE_12

#define GPIO13_HSYNC		PIN_MODE_4
#define GPIO13_I2C_SDA		PIN_MODE_5
#define GPIO13_UART0_RX		PIN_MODE_7
#define GPIO13_GT_CCP04 	PIN_MODE_12

#define GPIO14_CAM_D4		PIN_MODE_4
#define GPIO14_I2C_SCL		PIN_MODE_5
#define GPIO14_GSPI_CLK		PIN_MODE_7
#define GPIO14_GT_CCP05 	PIN_MODE_12

#define GPIO15_CAM_D5		PIN_MODE_4
#define GPIO15_I2C_SCL		PIN_MODE_5
#define GPIO15_GSPI_MIMO	PIN_MODE_7
#define GPIO15_SDCARD_D0	PIN_MODE_8
#define GPIO15_GT_CCP06 	PIN_MODE_13

#define GPIO16_CAM_D6		PIN_MODE_4
#define GPIO16_UART1_TX		PIN_MODE_5
#define GPIO16_GSPI_MOSI	PIN_MODE_7
#define GPIO16_SDCARD_CLK	PIN_MODE_8
#define GPIO16_GT_CCP07 	PIN_MODE_13

#define GPIO17_CAM_D7		PIN_MODE_4
#define GPIO17_UART1_RX		PIN_MODE_5
#define GPIO17_GSPI_CS		PIN_MODE_7
#define GPIO17_SDCARD_CMD 	PIN_MODE_8

#define GPIO22_GTCCP04		PIN_MODE_5
#define GPIO22_McAFSC 		PIN_MODE_7

/******************************************************
 * Macros for getting pin number from GPIO Number
 *****************************************************/

#define GPIO0_PIN_NUM 		PIN_50
#define GPIO1_PIN_NUM 		PIN_55
#define GPIO2_PIN_NUM 		PIN_57
#define GPIO3_PIN_NUM 		PIN_58
#define GPIO4_PIN_NUM 		PIN_59
#define GPIO5_PIN_NUM 		PIN_60
#define GPIO6_PIN_NUM 		PIN_61
#define GPIO7_PIN_NUM 		PIN_62
#define GPIO8_PIN_NUM 		PIN_63
#define GPIO9_PIN_NUM 		PIN_64
#define GPIO10_PIN_NUM 		PIN_01
#define GPIO11_PIN_NUM 		PIN_02
#define GPIO12_PIN_NUM 		PIN_03
#define GPIO13_PIN_NUM 		PIN_04
#define GPIO14_PIN_NUM 		PIN_05
#define GPIO15_PIN_NUM 		PIN_06
#define GPIO16_PIN_NUM 		PIN_07
#define GPIO17_PIN_NUM 		PIN_08
#define GPIO22_PIN_NUM 		PIN_15
#define GPIO28_PIN_NUM 		PIN_18
#define GPIO30_PIN_NUM 		PIN_53
























