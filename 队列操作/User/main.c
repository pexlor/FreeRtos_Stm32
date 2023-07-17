#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
#include "queue.h"
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
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//字
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 128//字
#define TASK2_PRIO  3
TaskHandle_t task2_handle;

#define TASK3_STACK_SIZE 128//字
#define TASK3_PRIO  4
TaskHandle_t task3_handle;


void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);

void freedemo(void);


QueueHandle_t key_queue; //小数据队列句柄
QueueHandle_t big_date_queue; //大数据队列句柄
char buff[10]={"asdasd"};

int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
	freedemo();
}



void start_task(void *pvParameters){
	taskENTER_CRITICAL();//进入临界区
	key_queue=xQueueCreate( 2, sizeof(uint8_t) );
big_date_queue = xQueueCreate( 1, sizeof(char*) );
	printf("创建成功！！");
  
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
	xTaskCreate((TaskFunction_t) task3,
						 (char *)"task3",
						 (configSTACK_DEPTH_TYPE)TASK3_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK3_PRIO,
						 (TaskHandle_t *)&task3_handle);							 
	  printf("创建完成");
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//退出临界区
}

//实现入队
void task1(void *pvParameters){
	uint8_t keyNum;
	BaseType_t err=pdFALSE;
	char * buf =buff;
	for(;;)
	{
		keyNum=Key_GetNum();
		if(keyNum==1){
			err= xQueueSend( key_queue, &keyNum, portMAX_DELAY );
			if(err!=pdTRUE){
				printf("发送失败");
			}else{
				printf("发送成功");
			}
		}else if(keyNum==2){
			err=xQueueSend(big_date_queue,&buf,portMAX_DELAY);
			if(err!=pdTRUE){
				printf("发送失败");
			}else{
				printf("发送成功");
			}
		}
		vTaskDelay(10);
	}
}

//实现小数据出队
void task2(void *pvParameters){
  uint8_t key=0;
	BaseType_t err=pdFALSE;
	for(;;)
  {
		printf("读取失败");
		err=xQueueReceive(key_queue,&key,portMAX_DELAY);
		if(err!=pdTRUE){
			printf("读取失败");
		}else{
			printf("队列成功，数据：%d",key);
		}
  }
}
//实现大数据出队
void task3(void *pvParameters){
	char* buf;
	BaseType_t err=pdFALSE;
	for(;;)
  {
		err=xQueueReceive(big_date_queue,&buf,portMAX_DELAY);
		if(err!=pdTRUE){
			printf("读取失败");
		}else{
			printf("队列成功，数据：%s",buf);
		}
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
