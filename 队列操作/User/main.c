#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
#include "queue.h"
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
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 128//��
#define TASK2_PRIO  3
TaskHandle_t task2_handle;

#define TASK3_STACK_SIZE 128//��
#define TASK3_PRIO  4
TaskHandle_t task3_handle;


void task1(void *pvParameters);
void task2(void *pvParameters);
void task3(void *pvParameters);

void freedemo(void);


QueueHandle_t key_queue; //С���ݶ��о��
QueueHandle_t big_date_queue; //�����ݶ��о��
char buff[10]={"asdasd"};

int main(void){
	SysTick_Init(72);
	USART1_Init(9600);
	Key_Init();
	freedemo();
}



void start_task(void *pvParameters){
	taskENTER_CRITICAL();//�����ٽ���
	key_queue=xQueueCreate( 2, sizeof(uint8_t) );
big_date_queue = xQueueCreate( 1, sizeof(char*) );
	printf("�����ɹ�����");
  
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
	  printf("�������");
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//�˳��ٽ���
}

//ʵ�����
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
				printf("����ʧ��");
			}else{
				printf("���ͳɹ�");
			}
		}else if(keyNum==2){
			err=xQueueSend(big_date_queue,&buf,portMAX_DELAY);
			if(err!=pdTRUE){
				printf("����ʧ��");
			}else{
				printf("���ͳɹ�");
			}
		}
		vTaskDelay(10);
	}
}

//ʵ��С���ݳ���
void task2(void *pvParameters){
  uint8_t key=0;
	BaseType_t err=pdFALSE;
	for(;;)
  {
		printf("��ȡʧ��");
		err=xQueueReceive(key_queue,&key,portMAX_DELAY);
		if(err!=pdTRUE){
			printf("��ȡʧ��");
		}else{
			printf("���гɹ������ݣ�%d",key);
		}
  }
}
//ʵ�ִ����ݳ���
void task3(void *pvParameters){
	char* buf;
	BaseType_t err=pdFALSE;
	for(;;)
  {
		err=xQueueReceive(big_date_queue,&buf,portMAX_DELAY);
		if(err!=pdTRUE){
			printf("��ȡʧ��");
		}else{
			printf("���гɹ������ݣ�%s",buf);
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
