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
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 128//��
#define TASK2_PRIO  2
TaskHandle_t task2_handle;

void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void task1(void *pvParameters);
void task2(void *pvParameters);
void freedemo(void);

List_t          TestList;           /* ��������б� */
ListItem_t      ListItem1;          /* ��������б���1 */
ListItem_t      ListItem2;          /* ��������б���2 */
ListItem_t      ListItem3;          /* ��������б���3 */

int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
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
void task1(void *pvParameters){
	uint32_t task1_num=0;
	for(;;)
	{
		taskENTER_CRITICAL();//�����ٽ���
		printf("task1:%d\r\n",task1_num++);
		taskEXIT_CRITICAL();//�˳��ٽ���
		delay_xms(10);
	}
}

void task2(void *pvParameters){
  uint32_t task2_num=0;
	while(1)
  {
		taskENTER_CRITICAL();//�����ٽ���
    printf("task2:%d\r\n",task2_num++);
		taskEXIT_CRITICAL();//�˳��ٽ���
		delay_xms(10);
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