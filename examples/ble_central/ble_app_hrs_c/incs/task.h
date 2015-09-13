#ifndef Task_H_
#define Task_H_



void System_Tick_Hook(void);



#define get_self_taskstate(task_id,p_task_state)  \
		do{ \
		  p_task_state=os_get_taskstate();\
		  p_task_state=p_task_state+task_id;\
		}while(0)




#define USER_TASK_ID   0
#define GUI_TASK_ID    2
#define SYSTEM_TASK_ID 3				
#define SHELL_TASK_ID 4		
				


#endif

