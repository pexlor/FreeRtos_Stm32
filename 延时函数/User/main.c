#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
#include "Timer.h"
//xTaskCreate();
//xTaskCreateStatic();
/*��̬���񴴽�����
����Ӧ������Ϊ1
���庯����ڲ���
��д������
*/
/*��̬���񴴽�
���ú�
������������붨ʱ������������ջ��TCB
ʵ�������ӿں���
vAPPlicationGetIdleTaskMemory()���������ڴ����
vApplicationGetTimeTaskMemory()
���庯����ڲ���
��д������
*/

/*ɾ����������
���ú�
��ڲ�������Ҫɾ������ľ����NULL��ʾɾ�������������е����񣩣�
*/

/*���������ָ�
void vTaskSuspend( TaskHandle_t xTaskToSuspend );//�������,NULL��ʾ������������
void vTaskResume( TaskHandle_t xTaskToResume );//��������ָ�
BaseType_t xTaskResumeFromISR( TaskHandle_t xTaskToResume ;//�ж��лָ���������,����pdTRUE(��Ҫ���������л�),pdFALSE
FromISRר�����ж��еĺ���
���ú�INCLUDE_vTaskSuspend
*/

/*
	�жϹ���
portDISABLE_INTERRUPTS(); //���ж�
portENABLE_INTERRUPTS();
*/

/*�ٽ���뱣������
taskENTER_CARTICAL()
taskEXIT_CAITICAL()
taskENTER_CRITCAL_FROM_ISR()
taskEXIT_CRITICAL_FROM_ISE()
*/
/*
	vListInitialise() ��ʼ���б�
	vListInitialiseItem() ��ʼ���б���
	vListInsertEnd() �б�ĩβ�����б���
	vListInsert() �б�����б��� �������
	uxListRemove() �б��Ƴ��б���
*/

/*			ʱ��Ƭ����
	һ��ʱ��Ƭ�͵���һ��SysTick�ж�����
*/

/* �������API
	uxTaskPriorityGet() ��ȡ�������ȼ�
	vTaskPrioritySet() �����������ȼ�
	uxTaskGetNumberOfTasks() ��ȡϵͳ�����������
	uxTaskGetSystemState() ��ȡ��������״̬��Ϣ
	vTaskFetInfo() ��ȡָ�������������Ϣ
	xTaskGetCurrentTaskHandle() ��ȡ��ǰ�����������
	xTaskGetHandle() ������������ȡ�������������
	uxTaskGetStackHighWaterMark() ��ȡ���������ջ��ʷʣ����Сֵ
	eTaskGetState() ��ȡ����״̬
	vTaskList() �Ա����ʽ��ȡ�����������Ϣ
	vTaskGetRunTimeStats() ��ȡ��������ʱ��
*/

/*
	vTaskDelay() �����ʱ 
	
	
	vTaskDelayUntil() ������ʱ ����������������ڿ���һ�����壬��������Ҫ����һ��Ƶ�����е�����
*/
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 128//��
#define TASK2_PRIO  3
TaskHandle_t task2_handle;

void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void task1(void *pvParameters);
void task2(void *pvParameters);
void freedemo(void);
void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);

List_t          TestList;           /* ��������б� */
ListItem_t      ListItem1;          /* ��������б���1 */
ListItem_t      ListItem2;          /* ��������б���2 */
ListItem_t      ListItem3;          /* ��������б���3 */

int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	//Key_Init();
	//Tmier_Init();
	
	freedemo();
}



void start_task(void *pvParameters){
	taskENTER_CRITICAL();//�����ٽ���
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
		taskEXIT_CRITICAL();//�˳��ٽ���
}

//ʹ�������ʱ����
void task1(void *pvParameters){
	uint32_t task1_num=0;
	for(;;)
	{
		LED_Toggle(GPIOA,GPIO_Pin_0);
		delay_xms(100);
		vTaskDelay(500);
	}
}

//ʹ�þ�����ʱ����
void task2(void *pvParameters){
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
  {
		LED_Toggle(GPIOA,GPIO_Pin_1);
		delay_xms(100);
		vTaskDelayUntil(&xLastWakeTime,500);
  }
}

void LED_Toggle(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin){
	if(GPIO_ReadOutputDataBit(GPIOx,GPIO_Pin)==1){
		GPIO_ResetBits(GPIOx,GPIO_Pin);
	}else{
		GPIO_SetBits(GPIOx,GPIO_Pin);
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
