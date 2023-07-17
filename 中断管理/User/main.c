#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
#include "Timer.h"
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

/*任务挂起与恢复
void vTaskSuspend( TaskHandle_t xTaskToSuspend );//任务挂起,NULL表示挂起任务自身
void vTaskResume( TaskHandle_t xTaskToResume );//挂起任务恢复
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ;//中断中恢复挂起任务,返回pdTRUE(需要进行任务切换),pdFALSE
FromISR专用于中断中的函数
配置宏INCLUDE_vTaskSuspend
*/

/*
	中断管理
portDISABLE_INTERRUPTS(); //关中断
portENABLE_INTERRUPTS();
*/

/*临界代码保护函数
taskENTER_CARTICAL()
taskEXIT_CAITICAL()
taskENTER_CRITCAL_FROM_ISR()
taskEXIT_CRITICAL_FROM_ISE()
*/
/*
	vListInitialise() 初始化列表
	vListInitialiseItem() 初始化列表项
	vListInsertEnd() 列表末尾插入列表项
	vListInsert() 列表插入列表项 有序插入
	uxListRemove() 列表移除列表项
*/
#define START_TASK_PRIO  1
TaskHandle_t satrt_task_handle;
#define START_TASK_STACK_SIZE 128


#define TASK1_STACK_SIZE 50//字
#define TASK1_PRIO  2
TaskHandle_t task1_handle;


void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void task1(void *pvParameters);
void freedemo(void);

int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
	Tmier_Init();
	
	freedemo();
}



void start_task(void *pvParameters){
	taskENTER_CRITICAL();//进入临界区
	 xTaskCreate((TaskFunction_t) task1,
						 (char *)"task1",
						 (configSTACK_DEPTH_TYPE)TASK1_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK1_PRIO,
						 (TaskHandle_t *)&task1_handle);
	
						 
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//退出临界区
}
void task1(void *pvParameters){
	uint16_t task1_num=0;
	for(;;)
	{
		if(++task1_num==5){
			task1_num=0;
			printf("关中断\r\n ");
			portDISABLE_INTERRUPTS();
			delay_xms(5000);
			portENABLE_INTERRUPTS();
			printf("开中断 \r\n");
		}
		delay_ms(1000);
	}
}

void freedemo(void){
	/*BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName, 
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask ) */
	xTaskCreate((TaskFunction_t) start_task,
						 (char *)"start_task",
						 (configSTACK_DEPTH_TYPE)START_TASK_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) START_TASK_PRIO,
						 (TaskHandle_t *)&satrt_task_handle);
	vTaskStartScheduler();
}

void TIM2_IRQHandler (void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){
		printf("TIM2 runing \n");
	}
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
}
void TIM3_IRQHandler (void){
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET){
		printf("TIM3\n");
	}
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}