#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
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

/*			时间片调度
	一个时间片就等于一个SysTick中断周期
*/

/* 任务相关API
	uxTaskPriorityGet() 获取任务优先级
	vTaskPrioritySet() 设置任务优先级
	uxTaskGetNumberOfTasks() 获取系统中任务的数量
	uxTaskGetSystemState() 获取所有任务状态信息
	vTaskFetInfo() 获取指定单个任务的信息
	xTaskGetCurrentTaskHandle() 获取当前任务的任务句柄
	xTaskGetHandle() 根据任务名获取该任务的任务句柄
	uxTaskGetStackHighWaterMark() 获取任务的任务栈历史剩余最小值
	eTaskGetState() 获取任务状态
	vTaskList() 以表格形式获取所有任务的信息
	vTaskGetRunTimeStats() 获取任务运行时间
*/

/* 队列相关API
		创建队列：
						xQueueCreate() 动态创建队列
						xQueueCreateStatic() 静态创建队列
		写入队列：
						xQueueSend()  写入尾部
						xQueueSendToBack() 写入尾部
						xQueueSendToFront() 写入头部
						xQueueOverwrite() 覆写
						
						xQueueSendFromISR()
						xQueueSendToBackFromISR()
						xQueueSendToFrontFromISR()
						xQueueOverwriteFromISR()
		读取消息
						xQueueReceive() 从队列头部读取消息，并删除
						xQueuePeek()		从队列头部读取消息
						xQueueReceiveFromISR()
						xQueuePeekFromISR()

*/

/*信号量：一种解决同步问题的机制，可以实现对共享资源的有序访问

二值信号量：信号量最大值为1 互斥访问或任务任务同步 存在优先级翻转的问题 更适合同步

xSemaphoreCreateBinary()创建二值信号量

xSemaphoreCreateBinaryStatic()

xSemaphoreGive() 释放信号量
xSemaphoreGiveFromISR()

xSemaphoreTake() 获取二值信号量
xSemaphoreTakeFromISR()


计数型信号量：信号量最大值大于1 事件计数和资源管理
xSemaphoreCreateCounting()
xSemaphoreCreateCountingStatic()
uxSemaphoreGetCount();
释放和获取与二值信号量相同

互斥信号量(不能用于中断中)：
xSemaphoreCreateMutex();
xSemaphoreCreateMutexStatic();

*/
/*优先级翻转：高优先级的任务反而慢执行，低优先级的任务先执行 
*/

/*一个队列只允许任务间传递消息为同一种数据类型，如需在不同任务之间传递不同数据类型，就可以使用队列集
	xQueueCreateSet() 创建队列集
	xQueueAddToSet()	添加队列
	xQueueRemoveFromSet() 从队列集中移除队列
	xQueueSelectFromSet() 获取队列集中有效消息队列
	xQueueSelectFromSetFromISR() 中断函数
*/

/*事件标志组：是一组事件标志位的集合 EventBits_t(高八位作为存储事件标志组的控制信息，低二十四为用作存储事件标志)
	事件标志位：用一个位来表示时间是否发送
	xEventGroupCreate()动态创建标志组
	xEventGroupCreateStatic()
	xEventGroupClearBits()清除事件标志位
	xEventGroupClearBitsFromISR()
	
	xEventGroupSetBits()设置事件标志位
	xEventGroupSetBitsFromISR()
	
	xEventGroupWaitBits()等待事件标志位
	xEventGroupWaitBitsFromISR()
	
	xEnentFroupSync()同步函数
	
*/

/*任务通知：用来通知任务
	效率更高，使用内存更小
	无法发送数据给ISR
	无法广播给多个任务
	无法缓存多个数据
	发送受阻不支持阻塞
	使用任务控制块中的ulNotifiedValue
	
	API：①发送通知
					xTaskNotify() 	发送通知，带有通知值
					xTaskNotifyAndQuery() 发送通知，带有通知值并且保留接收任务的原通知值
					xTaskNotifyGive()发送通知，不带通知值
					中断型：有
			 ②接受通知（只有在任务中）
					ulTaskNotifyTake();获取任务通知，可以设置在退出函数是任务通知清零或减一。
					xTaskNotifyWait();获取任务通知，更复杂，模拟队列或时间标志组。	
*/

/*软件定时器
	configUSE_TIMERS
	configTIMER_TASK_PRIORITY
	configTIMER_QUEUE_LENGTH 定时器命令队列长度
	单次定时器或周期定时器
	xTimerCreate()
	xTimerCreateStatic()
	xTimerStart()
	xTimerStartFromISR()
	xTimerStop()
	xTimerStopFromISR()
	xTimerReset()
	xTimerResetFromISR()
	xTimerChangePeriod()
	xTimerChangePeriodFromISR()
*/
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//字
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 128//字
#define TASK2_PRIO  3
TaskHandle_t task2_handle;


void task1(void *pvParameters);
void task2(void *pvParameters);
void vTimer1Callback( TimerHandle_t pxTimer );

void freedemo(void);
EventGroupHandle_t eventGroupHandle;


int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
	freedemo();
}

void vTimer1Callback( TimerHandle_t pxTimer ){
		uint32_t timer=0;
		printf("Timer1的运行次数：%d",timer++);
	
}
void vTimer2Callback( TimerHandle_t pxTimer ){
		uint32_t timer=0;
		printf("Timer2的运行次数：%d",timer++);
}

TimerHandle_t timer1_handle;
TimerHandle_t timer2_handle;
void start_task(void *pvParameters){
	taskENTER_CRITICAL();//进入临界区
	
	/*单次定时器*/
	timer1_handle= xTimerCreate( "TIM1",
                                1000,
                                pdFALSE,
                                (void *)1,
                                vTimer1Callback );
	/*周期定时器*/
	timer1_handle= xTimerCreate( "TIM2",
                                1000,
                                pdTRUE,
                                (void *)1,
                                vTimer2Callback );
																
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


void task2(void *pvParameters){
	uint32_t rev=0;
	for(;;)
	{
		xTaskNotifyWait(0,0xFFFFFFFF,&rev,portMAX_DELAY);
		printf("接收成功！！键值为：%d\r\n",rev);
		//vTaskDelay(1000);
	}
}

//按键扫描并控制软件定时器
void task1(void *pvParameters){
	uint8_t key;
  for(;;)
	{
		key=Key_GetNum();
		if(key==1){
			xTimerStart(timer1_handle,portMAX_DELAY);
			xTimerStart(timer2_handle,portMAX_DELAY);
		}else if(key==2){
			xTimerStop(timer1_handle,portMAX_DELAY);
			xTimerStop(timer2_handle,portMAX_DELAY);
		}
		vTaskDelay(10);
	}
	
}

void freedemo(void){
	xTaskCreate((TaskFunction_t) start_task,
						 (char *)"start_task",
						 (configSTACK_DEPTH_TYPE)START_TASK_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) START_TASK_PRIO,
						 (TaskHandle_t *)&satrt_task_handle);
	vTaskStartScheduler();
}

