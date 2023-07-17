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

/* �������API
		�������У�
						xQueueCreate() ��̬��������
						xQueueCreateStatic() ��̬��������
		д����У�
						xQueueSend()  д��β��
						xQueueSendToBack() д��β��
						xQueueSendToFront() д��ͷ��
						xQueueOverwrite() ��д
						
						xQueueSendFromISR()
						xQueueSendToBackFromISR()
						xQueueSendToFrontFromISR()
						xQueueOverwriteFromISR()
		��ȡ��Ϣ
						xQueueReceive() �Ӷ���ͷ����ȡ��Ϣ����ɾ��
						xQueuePeek()		�Ӷ���ͷ����ȡ��Ϣ
						xQueueReceiveFromISR()
						xQueuePeekFromISR()

*/

/*�ź�����һ�ֽ��ͬ������Ļ��ƣ�����ʵ�ֶԹ�����Դ���������

��ֵ�ź������ź������ֵΪ1 ������ʻ���������ͬ�� �������ȼ���ת������ ���ʺ�ͬ��

xSemaphoreCreateBinary()������ֵ�ź���

xSemaphoreCreateBinaryStatic()

xSemaphoreGive() �ͷ��ź���
xSemaphoreGiveFromISR()

xSemaphoreTake() ��ȡ��ֵ�ź���
xSemaphoreTakeFromISR()


�������ź������ź������ֵ����1 �¼���������Դ����
xSemaphoreCreateCounting()
xSemaphoreCreateCountingStatic()
uxSemaphoreGetCount();
�ͷźͻ�ȡ���ֵ�ź�����ͬ

�����ź���(���������ж���)��
xSemaphoreCreateMutex();
xSemaphoreCreateMutexStatic();

*/
/*���ȼ���ת�������ȼ������񷴶���ִ�У������ȼ���������ִ�� 
*/

/*һ������ֻ��������䴫����ϢΪͬһ���������ͣ������ڲ�ͬ����֮�䴫�ݲ�ͬ�������ͣ��Ϳ���ʹ�ö��м�
	xQueueCreateSet() �������м�
	xQueueAddToSet()	��Ӷ���
	xQueueRemoveFromSet() �Ӷ��м����Ƴ�����
	xQueueSelectFromSet() ��ȡ���м�����Ч��Ϣ����
	xQueueSelectFromSetFromISR() �жϺ���
*/

/*�¼���־�飺��һ���¼���־λ�ļ��� EventBits_t(�߰�λ��Ϊ�洢�¼���־��Ŀ�����Ϣ���Ͷ�ʮ��Ϊ�����洢�¼���־)
	�¼���־λ����һ��λ����ʾʱ���Ƿ���
	xEventGroupCreate()��̬������־��
	xEventGroupCreateStatic()
	xEventGroupClearBits()����¼���־λ
	xEventGroupClearBitsFromISR()
	
	xEventGroupSetBits()�����¼���־λ
	xEventGroupSetBitsFromISR()
	
	xEventGroupWaitBits()�ȴ��¼���־λ
	xEventGroupWaitBitsFromISR()
	
	xEnentFroupSync()ͬ������
	
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


void task1(void *pvParameters);
void task2(void *pvParameters);


void freedemo(void);
EventGroupHandle_t eventGroupHandle;


int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
	freedemo();
}



void start_task(void *pvParameters){
	taskENTER_CRITICAL();//�����ٽ���
	eventGroupHandle=xEventGroupCreate();
	if(eventGroupHandle!=NULL){
		printf("�¼���־�鴴���ɹ�");
	}
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


void task2(void *pvParameters){
	EventBits_t event_bits=0;
	for(;;)
	{
		event_bits=xEventGroupWaitBits( eventGroupHandle,
																		0x01|0x02,
																		pdTRUE,
																		pdTRUE, 
																		portMAX_DELAY );
		printf("�¼���־λ%#x\r\n",event_bits);
	}
}


void task1(void *pvParameters){
	uint8_t key;
  for(;;)
	{
		key=Key_GetNum();
		if(key==1)
		{
			xEventGroupSetBits(eventGroupHandle,0x01);
		}else if(key==2)
		{
			xEventGroupSetBits(eventGroupHandle,0x02);
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

