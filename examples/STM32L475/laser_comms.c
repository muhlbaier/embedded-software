/*
 * LaserComms.c
 *
 *  Created on: Apr 17, 2018
 *      Author: Russell
 */
#include "timing.h"
#include "uart.h"
#include "project_settings.h"
#include "stdint.h"
#include "laser_comms.h"
#include "stm32l4xx_hal.h"
#include "stm32l475e_iot01.h"
#include "stm32l4xx_hal_tim.h"


/*
//////////////////////////////////////////////
// Hardware Setup                           //
//                                          //
// MCU   - STM32L475VG                      //
// Board - STM32L4 Discovery Kit IoT Node   //
//                                          //
// LED   - A.05 - Active High               //
// Up    - C.13 - External pull-up resistor //
// Down  - B.00 - No pull-up/down resistor  //
//////////////////////////////////////////////
#define PWM_INCREMENT 10   // Percentage increment/decrement to PWM on button-press
#define PWM_FREQUENCY 1000 // 1kHz output PWM clock

#define TIMER_INPUT_CLOCK 16000000 // 16MHz timer1 input clock
#define PWM_PERIOD        (uint32_t)(TIMER_INPUT_CLOCK / PWM_FREQUENCY - 1)

////////////////////////////////////////////////
// PWM - A.5 - Timer 2 - Alternate function 1 //
////////////////////////////////////////////////
#define PWM_TIMER_ENABLE()     __HAL_RCC_TIM2_CLK_ENABLE()  // PWM on timer 2
#define PWM_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE() // GPIOA Clock
#define PWM_GPIO_AF            GPIO_AF1_TIM2 // Alternate function 1 - Timer 2
#define PWM_GPIO_PIN           GPIO_PIN_5    // Pin 5
#define PWM_GPIO_PORT          GPIOA         // Port A
#define PWM_TIMER              TIM2          // Timer 2
#define PWM_CHANNEL            TIM_CHANNEL_1 // Channel 1

////////////////////////////////////////////////
// Up Button - C.13 - EXTI13 (EXTI15_10_IRQn) //
////////////////////////////////////////////////
#define UP_BUTTON_CLK_ENABLE() __HAL_RCC_GPIOC_CLK_ENABLE() // GPIOC Clock
#define UP_BUTTON_PIN          GPIO_PIN_13 // Pin 13
#define UP_BUTTON_PULL         GPIO_NOPULL // No pull-up or down
#define UP_BUTTON_TRIGGER      GPIO_MODE_IT_RISING // Trigger interrupt on rising edge
#define UP_BUTTON_PORT         GPIOC // Port C
#define UP_BUTTON_IRQ          EXTI15_10_IRQn

////////////////////////////////
// Down Button - B.00 - EXTI0 //
////////////////////////////////
#define DOWN_BUTTON_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE() // GPIOB Clock
#define DOWN_BUTTON_PIN          GPIO_PIN_0 // Pin 0
#define DOWN_BUTTON_PULL         GPIO_PULLUP // Internal pull-up resistor
#define DOWN_BUTTON_TRIGGER      GPIO_MODE_IT_RISING // Trigger interrupt on rising edge
#define DOWN_BUTTON_PORT         GPIOB // Port C
#define DOWN_BUTTON_IRQ          EXTI0_IRQn


////////////////////////////////////
// PWM Initialization and Control //
////////////////////////////////////
static void initializePWMTimer();
static void setPWMDutyCycle(uint8_t percent);
static void configurePWM_GPIO(void);


// Timer and output compare handler structure declaration
static TIM_HandleTypeDef  PWM_TimerHandle;
static TIM_OC_InitTypeDef PWM_OCHandle;

static volatile int8_t pwmDutyCycle = 10; // Duty cycle value between 0 and 100%

*/

void ClearFlag(void);


typedef void (* functionptr)(uint8_t);

uint8_t player_id = 0; //The preprocessed player ID to send via UART.
uint8_t min_limit = MIN_LIMIT; //The minimum time latency between getting hit and being able to get hit again.
uint8_t invincible= 0; //The flag used to determine whether the player is invincible from getting hit.
functionptr hit_callback = 0; //Hit callback to publish player who hit you
volatile unsigned int i; //Loop Counter


/*
 * @param playerid Lower 5 bits of a unique identifier.
 * @param * hit The pointer to a callback for getting hit.
 * @param limit The desired latency between shots specified by the game rules.
 */
void LaserComms_Init(uint8_t playerid, void (* hit)(uint8_t), uint32_t limit){
	Timing_Init();
	UART_Init(GUN_UART);
	UART_ReconfigureBaud(GUN_UART, FIRE_BAUD_RATE);
	UART_RegisterReceiver(GUN_UART,LaserComms_HitFilter);
	player_id = (playerid & 0x1F) | 0x20;
	hit_callback = hit;
	if(limit>MIN_LIMIT){
		min_limit = limit;
	}
//	initializePWMTimer();
//	configurePWM_GPIO();
//	setPWMDutyCycle( pwmDutyCycle );
}

void LaserComms_Fire(void){
	//fire SHOTS_PER_BURST times
	for(i=0;i<SHOTS_PER_BURST;i++){
		UART_WriteByte(GUN_UART,player_id);
	}

}

/*
 * Callback for UART Rx
 */
void LaserComms_HitFilter(uint8_t enemyPlayer){
	//get hit, but only if not just hit
	if(!invincible){

		//use * hit to publish enemyPlayer
		hit_callback(enemyPlayer);

		invincible = 1;
		Task_Schedule(ClearFlag,0,min_limit,0);
	}

}

void ClearFlag(){
	invincible = 0;
}

/*
static void configurePWM_GPIO(void) {
	GPIO_InitTypeDef   PWM_GPIO_InitStruct;

	// Enable peripherals and GPIO Clocks
	PWM_TIMER_ENABLE();
	PWM_GPIO_CLK_ENABLE();

	// Configure pin mode, pull, speed
	PWM_GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Push-pull
	PWM_GPIO_InitStruct.Pull = GPIO_PULLUP; // Pull-up resistor
	PWM_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // High-speed 50-80MHz

	// Configure pin alternate function, number, port
	PWM_GPIO_InitStruct.Alternate = PWM_GPIO_AF;
	PWM_GPIO_InitStruct.Pin = PWM_GPIO_PIN;
	HAL_GPIO_Init( PWM_GPIO_PORT, &PWM_GPIO_InitStruct );
}

static void initializePWMTimer() {
	uint32_t uhPrescalerValue = (uint32_t)(SystemCoreClock / 16000000) - 1;

	// Initialize timer peripheral as follows:
	//    + Prescaler = (SystemCoreClock / 16000000) - 1
	//    + Period = TIMER_INPUT_CLOCK / PWM_FREQUENCY - 1
	//    + ClockDivision = 0
	//    + Counter direction = Up
	PWM_TimerHandle.Instance = PWM_TIMER;

	PWM_TimerHandle.Init.Prescaler         = uhPrescalerValue;
	PWM_TimerHandle.Init.Period            = PWM_PERIOD;
	PWM_TimerHandle.Init.ClockDivision     = 0;
	PWM_TimerHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
	PWM_TimerHandle.Init.RepetitionCounter = 0;

	if (HAL_TIM_PWM_Init(&PWM_TimerHandle) != HAL_OK)	{
		Error_Handler(); // Initialization Error
	}

	// Configure PWM output-compare
	PWM_OCHandle.OCMode       = TIM_OCMODE_PWM1; // PWM mode
	PWM_OCHandle.OCPolarity   = TIM_OCPOLARITY_HIGH; // High polarity
	PWM_OCHandle.OCFastMode   = TIM_OCFAST_DISABLE; // Disable fast state
	PWM_OCHandle.OCNPolarity  = TIM_OCNPOLARITY_HIGH; // High N polarity
	PWM_OCHandle.OCNIdleState = TIM_OCNIDLESTATE_RESET; // N Idle state: reset
	PWM_OCHandle.OCIdleState  = TIM_OCIDLESTATE_RESET; // Idle state: reset

	PWM_OCHandle.Pulse = PWM_PERIOD; // Initialize at 100% duty cycle

	if (HAL_TIM_PWM_ConfigChannel(&PWM_TimerHandle, &PWM_OCHandle, PWM_CHANNEL) != HAL_OK) {
		Error_Handler(); // Configuration Error
	}

	// Start channel 1
	if (HAL_TIM_PWM_Start(&PWM_TimerHandle, PWM_CHANNEL) != HAL_OK) {
		Error_Handler(); // PWM Generation Error
	}
}

static void setPWMDutyCycle(uint8_t percent) {

	if ((percent < 0) || (percent > 100)) {
		return;
	}

	PWM_OCHandle.Pulse = PWM_PERIOD * percent / 100;

	// Initialize Timer
	if (HAL_TIM_PWM_Init(&PWM_TimerHandle) != HAL_OK)	{
		Error_Handler();
	}

	// Configure PWM Channel
	if (HAL_TIM_PWM_ConfigChannel(&PWM_TimerHandle, &PWM_OCHandle, PWM_CHANNEL) != HAL_OK)	{
		Error_Handler(); // Configuration Error
	}

	// Start PWM Channel
	if (HAL_TIM_PWM_Start(&PWM_TimerHandle, PWM_CHANNEL) != HAL_OK) {
		Error_Handler(); // PWM Generation Error
	}
}

*/


