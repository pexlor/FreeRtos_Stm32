#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "SysTick.h"
//xTaskCreate();
//xTaskCreateStatic();
/*动态任务创建流程
将相应宏配置为1
定义函数入口参数
编写任务函数
*/

/*静态任务创建
配置宏
定义空闲任务与定时器任务的任务堆栈和TCB
实现两个接口函数
vAPPlicationGetIdleTaskMemory()空闲任务内存分配
vApplicationGetTimeTaskMemory()
定义函数入口参数
编写任务函数
*/

/*删除任务流程
配置宏
入口参数输入要删除任务的句柄（NULL表示删除本身（正在运行的任务））

*/

#define START_TASK_PRIO  1
TaskHandle_t satrt_task_handle;
#define START_TASK_STACK_SIZE 128
StackType_t start_task_stack[START_TASK_STACK_SIZE];
StaticTask_t start_task_tcb;

#define TASK1_STACK_SIZE 50//字
#define TASK1_PRIO  2
TaskHandle_t task1_handle;
StackType_t task1_stack[START_TASK_STACK_SIZE];
StaticTask_t task1_tcb;

#define TASK2_STACK_SIZE 50
#define TASK2_PRIO  3
TaskHandle_t task2_handle;
StackType_t task2_stack[START_TASK_STACK_SIZE];
StaticTask_t task2_tcb;



void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void start_task(void *pvParameters);
void task2(void *pvParameters);
void task1(void *pvParameters);
void task3(void *pvParameters);
void freedemo(void);
int main(void){
	SysTick_Init(72);
	LED_Init();
	USART1_Init(9600);
	freedemo();
}


void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;//结构体
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;//配置端口驱动模式（推挽输出）
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_0;//选择端口（可多选）
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置IO口速度
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//用结构体参数初始GPIO口
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//把指定的端口设为高电平
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//把指定的端口设为高电平
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);//把指定的端口设为高电平
}


void freedemo(void){
	/*TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                    const char * const pcName, 
                                    const uint32_t ulStackDepth,
                                    void * const pvParameter,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer )*/
	
	satrt_task_handle=xTaskCreateStatic( (TaskFunction_t) 				 start_task,
																	(char *        ) 				 "start_task", 
																	(uint32_t			 )         START_TASK_STACK_SIZE,
																	(void *				 ) 				 NULL,
																	(UBaseType_t   ) 				 START_TASK_PRIO,
																	(StackType_t * ) 				 start_task_stack,
																	(StaticTask_t *) 				 &start_task_tcb
																	);
    
	
	vTaskStartScheduler();
}
void start_task(void *pvParameters){
		taskENTER_CRITICAL();//进入临界区
	  task1_handle=xTaskCreateStatic((TaskFunction_t) 				 task1,
																		(char *        ) 				 "task1", 
																		(uint32_t			 )         TASK1_STACK_SIZE,
																		(void *				 ) 				 NULL,
																		(UBaseType_t   ) 				 TASK1_PRIO,
																		(StackType_t * ) 				 task1_stack,
																		(StaticTask_t *) 				 &task1_tcb
																		);
	 task2_handle= xTaskCreateStatic((TaskFunction_t) 				 task2,
																		(char *        ) 				 "task2", 
																		(uint32_t			 )         TASK2_STACK_SIZE,
																		(void *				 ) 				 NULL,
																		(UBaseType_t   ) 				 TASK2_PRIO,
																		(StackType_t * ) 				 task2_stack,
																		(StaticTask_t *) 				 &task2_tcb
																		);
														
		vTaskDelete(satrt_task_handle);
		taskEXIT_CRITICAL();//退出临界区
}


void task1(void *pvParameters){
	for(;;)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
		vTaskDelay(500);
		
	}
}
void task2(void *pvParameters){
	for(;;)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_0);
		vTaskDelay(500);
		
	}
}
void task3(void *pvParameters){
	for(;;)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		vTaskDelay(500);
		
	}
}
StaticTask_t idle_tack_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, 
																		StackType_t ** ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize )//空闲任务内存分配
{
	* ppxIdleTaskTCBBuffer=&idle_tack_tcb;
	* ppxIdleTaskStackBuffer=idle_task_stack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}


StaticTask_t time_tack_tcb;
StackType_t time_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, 
																StackType_t ** ppxTimerTaskStackBuffer, 
																uint32_t *pulTimerTaskStackSize )
{//软件定时器内存分配
				* ppxTimerTaskTCBBuffer=&time_tack_tcb;
				* ppxTimerTaskStackBuffer=time_task_stack;
				*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}


