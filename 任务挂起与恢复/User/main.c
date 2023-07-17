#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
#include "Key.h"
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
#define START_TASK_PRIO  1
TaskHandle_t satrt_task_handle;
#define START_TASK_STACK_SIZE 128


#define TASK1_STACK_SIZE 50//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;

#define TASK2_STACK_SIZE 50
#define TASK2_PRIO  3
TaskHandle_t task2_handle;

#define TASK3_STACK_SIZE 128
#define TASK3_PRIO  1
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
	Key_Init();
	freedemo();
}


void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;//�ṹ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;//���ö˿�����ģʽ�����������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_0;//ѡ��˿ڣ��ɶ�ѡ��
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//����IO���ٶ�
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//�ýṹ�������ʼGPIO��
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//��ָ���Ķ˿���Ϊ�ߵ�ƽ
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);//��ָ���Ķ˿���Ϊ�ߵ�ƽ
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
	 xTaskCreate((TaskFunction_t) task3,
						 (char *)"task3",
						 (configSTACK_DEPTH_TYPE)TASK3_STACK_SIZE,
						 (void *) NULL,
						 (UBaseType_t) TASK3_PRIO,
						 (TaskHandle_t *)&task3_handle);
						 
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();//�˳��ٽ���
}
void task1(void *pvParameters){
	for(;;)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_1);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_1);
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
	uint16_t keyNum;
	for(;;)
	{
		keyNum=Key_GetNum();
		if(keyNum==1){
			printf("asdasd\n");
			vTaskSuspend(task1_handle);
		}else if(keyNum==2){
			printf("asdasdasd\n");
			vTaskResume(task1_handle);
		}
		vTaskDelay(20);
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

void EXTI0_IRQHandler(void){
	BaseType_t xYieldRequired;
	if(EXTI_GetITStatus(EXTI_Line0==SET)){
		printf("ok\n");
		xYieldRequired=xTaskResumeFromISR(task1_handle);
		if(xYieldRequired==pdTRUE){
			portYIELD_FROM_ISR(xYieldRequired);//�����л�
		}
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

