#include "task.h"

typedef struct {
	task_fn_t fn; // function pointer to task
	void * pointer; // optional pointer parameter for task (0 if not used)
	uint32_t time; // time to execute (if scheduled) or time added (if queued)
	uint32_t period; // 0 for non-periodic, period in ms otherwise
}task_struct_t

// use the provided macro to create a task_t array, list_links_t array, and list_t for the tasks
LIST_ALLOCATE(task_struct_t, task, TASK_MAX_LENGTH);

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
	// use the provided macro to initialize the list module (List_Init())
	LIST_INIT(&task, TASK_MAX_LENGTH, sizeof(task_struct_t));
	List_SetSortFunction(&task, SortTask);
	List_SetIdentifyFunction(&task, IdentifyTask);
}

void SystemTick(void) {
	task_struct_t * task_ptr;
	List_Link(&task);
	
	task_ptr = List_GetFirst(&task);
	// check if next task is due to run
	if(task_ptr->time <= TimeNow()) {
		// unlink the task
		List_UnlinkFirst(&task);
		if(task_ptr->pointer) {
			task_ptr->fn(task_ptr->pointer);
		}else {
			task_ptr->fn();
		}
		if(task_ptr->period) {
			task_ptr->time += task_ptr->period;
			List_LinkItem(&task, task_ptr);
		}else {
			List_Remove(&task, task_ptr);
		}
	}
}

void Task_Queue(task_t fn, void * pointer) {
	task_struct_t * task_ptr;
	task_ptr = List_AddIndirect(&task);
	if(task_ptr == 0) return;
	task_ptr->fn = fn;
	task_ptr->pointer = pointer;
	task_ptr->time = TimeNow();
	task_ptr->period = 0;
}

int8_t Task_Schedule(task_t fn, void * pointer, uint32_t delay, uint32_t period) {
	task_struct_t * task_ptr;
	task_ptr = List_AddIndirect(&task);
	if(task_ptr == 0) return;
	task_ptr->fn = fn;
	task_ptr->pointer = pointer;
	task_ptr->time = TimeNow() + delay;
	task_ptr->period = period;
}
		
int8_t Task_SetIdleTask(task_t fn) {
	
}

void Task_Remove(task_t fn, void * pointer) {
	
}

void Task_ChangePeriod(task_t fn, uint32_t period, uint8_t update_next_time) {
	
}
