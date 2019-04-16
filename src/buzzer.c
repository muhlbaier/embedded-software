#include "task.h"
#include "buzzer.h"

void Buzzer_Init(void){
    hal_Buzzer_Init();
}

void Buzzer_Sound(uint16_t frequency, uint16_t timeout){
    hal_Start_Buzzer(frequency);
    Task_Schedule((task_t) &hal_Stop_Buzzer, 0, timeout, 0);
}
