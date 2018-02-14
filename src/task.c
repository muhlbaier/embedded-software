#include "task.h"
#include "list.h"

typedef struct {
    task_fn_t fn; // function pointer to task
    void * pointer; // optional pointer parameter for task (0 if not used)
    uint32_t time; // time to execute (if scheduled) or time added (if queued)
    uint32_t period; // 0 for non-periodic, period in ms otherwise
}task_struct_t

// use the provided macro to create a task_t array, list_links_t array, and list_t for the tasks
LIST_ALLOCATE(task_struct_t, task_list, TASK_MAX_LENGTH);

// task pointer to function to run when idle (if set)
task_t idle_task;

// helper function to roll the time
static void RollTimer(void);

// return 1 if a should come before b, 0 otherwise
static uint8_t SortTask(task_struct_t * a, task_struct_t * b) {
    if(a->time < b->time) {
        return 1;
    }else {
        return 0;
    }
}

static uint8_t IdentifyTask(void (* identifier)(void), task_struct_t * item) {
    if(item->fn == identifier) {
        return 1;
    }else {
        return 0;
    }
}

void Task_Init(void) {
    // use flag so module only gets initialized once
    static uint8_t init_flag = 0;
    if(init_flag) return;
    init_flag = 1;
    // make sure the timing module is initialized
    Timing_Init();
    // use the provided macro to initialize the list module (List_Init())
    LIST_INIT(&task_list, TASK_MAX_LENGTH, sizeof(task_struct_t));
    List_SetSortFunction(&task_list, SortTask);
    List_SetIdentifyFunction(&task_list, IdentifyTask);
    // Reset the idle task
    idle_task = 0;
    // schedule the timer to be rolled over
    Task_Schedule(RollTimer, 0, TASK_ROLL_TIME, TASK_ROLL_TIME);
}

void SystemTick(void) {
    task_struct_t * task_ptr;
    task_fn_t fn_ptr;
    // check if any tasks need to be linked into the queue
    List_Link(&task_list);

    task_ptr = List_GetFirst(&task_list);
    if (task_ptr) {
        if (task_ptr->time <= TimeNow()) {
            // Unlink the front of the list so if SystemTick is called from a task it wont re-run
            List_UnlinkFirst(&task_list);
            fn_ptr = task_ptr->fn; // backup fn pointer
            if(task_ptr->pointer) {
                // if a pointer is used cast to the correct fn pointer type and run
                ((task_fn_pointer_input_t)task_ptr->fn)(task_ptr->pointer);
            }else {
                task_ptr->fn();
            }
            // if the function pointer has changed then it must have removed
            // itself. In this case don't reschedule or remove again
            BlockInterrupts();
            if(task_ptr->fn == fn_ptr) {
                RestoreInterrupts();
                if (task_ptr->period) {
                    task_ptr->time += task_ptr->period;
                    List_LinkItem(&task_list, task_ptr);
                } else {
                    List_Remove(&task_list, task_ptr);
                }
            }else {
                RestoreInterrupts();
            }
        } else {
            // Idle tasks to complete
            if(idle_task){
                idle_task();
            }
        }
    } else {
        // Idle tasks to complete
        if(idle_task){
            idle_task();
        }
    }
}

void Task_Queue(task_t fn, void * pointer) {
    task_struct_t * task_ptr;
    task_ptr = List_AddIndirect(&task_list);
    if(task_ptr == 0) return;
    task_ptr->fn = fn;
    task_ptr->pointer = pointer;
    task_ptr->time = TimeNow();
    task_ptr->period = 0;
}

int8_t Task_Schedule(task_t fn, void * pointer, uint32_t delay, uint32_t period) {
    task_struct_t * task_ptr;
    task_ptr = List_AddIndirect(&task_list);
    if (task_ptr == 0) {
		return -1;
    }
    // load the function, priority, next time to run, and period
    task_ptr->fn = fn;
    task_ptr->time = TimeNow() + delay;
    task_ptr->period = period;
    task_ptr->pointer = pointer;
    return 0;
}
        
int8_t Task_SetIdleTask(task_t fn) {
    if(idle_task == 0) {
        idle_task = fn;
        return 0;
    }else {
        idle_task = fn;
        return 1;
    }
}

void Task_Remove(task_t fn, void * pointer) {
    task_struct_t* task_ptr;
    uint8_t i;
    // find the first task that matches in the entire task_array
    // this is because the task that is running may not be on the list and may
    // want to remove itself
    for(i = 0; i < MAX_TASK_LENGTH; i++) {
        if(task_array[i].fn == fn && ((task_array[i].pointer == pointer) || pointer == 0 )) {
            task_ptr = &task_array[i];
            break;
        }
    }
    // if no task matches then i would be MAX_TASK_LENGTH
    while (i < MAX_TASK_LENGTH) {
        // clear the function pointer so SystemTick will know if the task it is 
        // running was removed
        task_ptr->fn = 0; 
        List_Remove(&task_list, task_ptr);
        // don't keep looking if the function and pointer were specified
        // it will be the users responsibility to call Task_Remove if they
        // added multiple tasks with the same input pointer
        if(pointer) break;
        // look for the next match
        for(; i < MAX_TASK_LENGTH; i++) {
            if(task_array[i].fn == fn) {
                task_ptr = &task_array[i];
                break;
            }
        }
    }
}

// delay a set number of milliseconds but call SystemTick() while we wait so
// we will run system processes while we wait
void WaitMs(uint32_t wait) {
    uint32_t time;
    // get the current time
    time = TimeNow();
    // while time since time is less than or equal to wait
    while (TimeSince(time) <= wait) {
        // call SystemTick()
        SystemTick();
    }
}

uint8_t Task_IsScheduled(task_t fn) {
    task_struct_t * task;
    task = List_GetItem(&task_list, &fn);
    if(task) return 1;
    return 0;
}

void Task_ChangePeriod(task_t fn, uint32_t period, uint8_t update_next_time) {
    task_struct_t * task;
    // try to find the task
    task = List_GetItem(&task_list, &fn);
    if(task) {
        // unlink the item
        List_UnlinkItem(&task_list, task);
        if(update_next_time) {
            // update the time based on the delta period
            task->time = task->time + period - task->period;
        }
        // update the period
        task->period = period;
        // relink the item
        List_LinkItem(&task_list, task);
    }
}


static void RollTimer(void) {
    uint32_t time;
    task_struct_t * task;
    // get the current time
    time = TimeNow();
    // for each task
    task = List_GetFirst(&task_list);
    while (task) {
        // if time is in the future then roll it
        // otherwise just let it be as TimeSince will handle the roll
        if (task->time > time) {
            // rolled task time = task time - current time
            // since current time is about to be set to 0
            task->time = task->time - time;
        }
        task = List_GetNext(&task_list, task);
    }

    // reset the system time and set the rollover time
    Timing_Roll();
}
