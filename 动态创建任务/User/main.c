#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
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


#define TASK1_STACK_SIZE 50//字
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 50
#define TASK2_PRIO  3
TaskHandle_t task2_handle;

#define TASK3_STACK_SIZE 128
#define TASK3_PRIO  3
TaskHandle_t task3_handle;

void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void task2(void *pvParameters);
void task1(void *pvParameters);
void task3(void *pvParameters);
void freedemo(void);
BaseType_t xTaskCreate( TaskFunction_t pxTaskCode,
                            const char * const pcName, 
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask );
int main(void){
	SysTick_Init(72);
	LED_Init();
	USART1_Init(9600);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4
	freedemo();
}


void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;//结构体
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;//配置端口驱动模式（推挽输出）
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_0;//选择端口（可多选）
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置IO口速度
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//用结构体参数初始GPIO口
	GPIO_SetBits(GPIOA,GPIO_Pin_1);//把指定的端口设为高电平
	GPIO_SetBits(GPIOA,GPIO_Pin_2);//把指定的端口设为高电平
}

void start_task(void *pvParameters){
	taskENTER_CRITICAL();//进入临界区
	 xTaskCreate((TaskFunction_t) task1,
						 (char *)"task1",
						 (configSTACK_DEPTH_TYPE)TASK1_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK1_PRIO,
						 (TaskHandle_t *)&task1_handle);
	 xTaskCreate((TaskFunction_t) task2,
						 (char *)"task2",
						 (configSTACK_DEPTH_TYPE)TASK2_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK2_PRIO,
						 (TaskHandle_t *)&task2_handle);
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//退出临界区
}
void task1(void *pvParameters){
	for(;;)
	{
	//GPIO_SetBits(GPIOA,GPIO_Pin_1);
		vTaskDelay(500);
	//	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
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


