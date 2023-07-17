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
#define START_TASK_PRIO  1
TaskHandle_t satrt_task_handle;
#define START_TASK_STACK_SIZE 128


#define TASK1_STACK_SIZE 50//��
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
	taskENTER_CRITICAL();//�����ٽ���
	 xTaskCreate((TaskFunction_t) task1,
						 (char *)"task1",
						 (configSTACK_DEPTH_TYPE)TASK1_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK1_PRIO,
						 (TaskHandle_t *)&task1_handle);
	
						 
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//�˳��ٽ���
}
void task1(void *pvParameters){
	uint16_t task1_num=0;
	for(;;)
	{
		if(++task1_num==5){
			task1_num=0;
			printf("���ж�\r\n ");
			portDISABLE_INTERRUPTS();
			delay_xms(5000);
			portENABLE_INTERRUPTS();
			printf("���ж� \r\n");
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