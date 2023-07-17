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
#define START_TASK_PRIO  1
#define START_TASK_STACK_SIZE 128
TaskHandle_t satrt_task_handle;

#define TASK1_STACK_SIZE 128//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 256//��
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
		GPIO_SetBits(GPIOA,GPIO_Pin_0);
		delay_xms(10);
	}
}

void task2(void *pvParameters){
  UBaseType_t task2_priority=0;
	UBaseType_t task_num=0;
	UBaseType_t task_num2=0;
	TaskStatus_t * status_arry =0;
	TaskStatus_t * status_array2=0;
	TaskHandle_t temp_handle;
	eTaskState taskstate;
	UBaseType_t minstack;
	vTaskPrioritySet(NULL,20);
	task2_priority =	uxTaskPriorityGet(NULL);
	task_num=uxTaskGetNumberOfTasks();
	char  pcWriteBuffer[300];
	
	status_arry = pvPortMalloc(sizeof(TaskStatus_t)*task_num);
	task_num2=uxTaskGetSystemState(status_arry,task_num,NULL);
	printf("������ �������ȼ� ������\r\n");
	for(int i=0;i<task_num2;i++){
		printf("%s %ld %ld\r\n",status_arry[i].pcTaskName,status_arry[i].uxBasePriority,status_arry[i].xTaskNumber);
	}
	
	
	status_array2 = pvPortMalloc(sizeof(TaskStatus_t));
	vTaskGetInfo(task1_handle,status_array2,pdTRUE,eInvalid);
	 
	printf("��������%s\r\n",status_array2->pcTaskName);
	printf("�������ȼ���%ld\r\n",status_array2->uxCurrentPriority);
	printf("�����ţ�%ld\r\n",status_array2->xTaskNumber);
	printf("����״̬��%d\r\n",status_array2->eCurrentState);
	
	temp_handle=xTaskGetHandle("task1");
	printf("��������ַ%#x\r\n",(int)temp_handle);
	
	
	vTaskList(pcWriteBuffer);
	printf("%s\r\n",pcWriteBuffer);
	for(;;)
  {
		minstack=uxTaskGetStackHighWaterMark(task2_handle);
		printf("task2 ʣ����С��ջ%ld",minstack);
		
		taskstate=eTaskGetState(task2_handle);
		printf("task2 ��״̬Ϊ��%d\r\n",taskstate);
		
		vTaskDelay(100);
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
