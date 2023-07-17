#include "stm32f10x.h"  // Device header
#include "FreeRTOS.h"
#include "task.h"
#include "SysTick.h"
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

#define START_TASK_PRIO  1
TaskHandle_t satrt_task_handle;
#define START_TASK_STACK_SIZE 128
StackType_t start_task_stack[START_TASK_STACK_SIZE];
StaticTask_t start_task_tcb;

#define TASK1_STACK_SIZE 50//��
#define TASK1_PRIO  2
TaskHandle_t task1_handle;
StackType_t task1_stack[START_TASK_STACK_SIZE];
StaticTask_t task1_tcb;

#define TASK2_STACK_SIZE 50
#define TASK2_PRIO  3
TaskHandle_t task2_handle;
StackType_t task2_stack[START_TASK_STACK_SIZE];
StaticTask_t task2_tcb;



void LED_Init(void);
void vTaskDelay( const TickType_t xTicksToDelay );
void start_task(void *pvParameters);
void task2(void *pvParameters);
void task1(void *pvParameters);
void task3(void *pvParameters);
void freedemo(void);
int main(void){
	SysTick_Init(72);
	LED_Init();
	USART1_Init(9600);
	freedemo();
}


void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;//�ṹ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;//���ö˿�����ģʽ�����������
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_0;//ѡ��˿ڣ��ɶ�ѡ��
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//����IO���ٶ�
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);//�ýṹ�������ʼGPIO��
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);//��ָ���Ķ˿���Ϊ�ߵ�ƽ
	GPIO_ResetBits(GPIOA,GPIO_Pin_2);//��ָ���Ķ˿���Ϊ�ߵ�ƽ
	GPIO_ResetBits(GPIOA,GPIO_Pin_0);//��ָ���Ķ˿���Ϊ�ߵ�ƽ
}


void freedemo(void){
	/*TaskHandle_t xTaskCreateStatic( TaskFunction_t pxTaskCode,
                                    const char * const pcName, 
                                    const uint32_t ulStackDepth,
                                    void * const pvParameter,
                                    UBaseType_t uxPriority,
                                    StackType_t * const puxStackBuffer,
                                    StaticTask_t * const pxTaskBuffer )*/
	
	satrt_task_handle=xTaskCreateStatic( (TaskFunction_t) 				 start_task,
																	(char *        ) 				 "start_task", 
																	(uint32_t			 )         START_TASK_STACK_SIZE,
																	(void *				 ) 				 NULL,
																	(UBaseType_t   ) 				 START_TASK_PRIO,
																	(StackType_t * ) 				 start_task_stack,
																	(StaticTask_t *) 				 &start_task_tcb
																	);
    
	
	vTaskStartScheduler();
}
void start_task(void *pvParameters){
		taskENTER_CRITICAL();//�����ٽ���
	  task1_handle=xTaskCreateStatic((TaskFunction_t) 				 task1,
																		(char *        ) 				 "task1", 
																		(uint32_t			 )         TASK1_STACK_SIZE,
																		(void *				 ) 				 NULL,
																		(UBaseType_t   ) 				 TASK1_PRIO,
																		(StackType_t * ) 				 task1_stack,
																		(StaticTask_t *) 				 &task1_tcb
																		);
	 task2_handle= xTaskCreateStatic((TaskFunction_t) 				 task2,
																		(char *        ) 				 "task2", 
																		(uint32_t			 )         TASK2_STACK_SIZE,
																		(void *				 ) 				 NULL,
																		(UBaseType_t   ) 				 TASK2_PRIO,
																		(StackType_t * ) 				 task2_stack,
																		(StaticTask_t *) 				 &task2_tcb
																		);
														
		vTaskDelete(satrt_task_handle);
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
	for(;;)
	{
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		vTaskDelay(500);
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		vTaskDelay(500);
		
	}
}
StaticTask_t idle_tack_tcb;
StackType_t idle_task_stack[configMINIMAL_STACK_SIZE];
void vApplicationGetIdleTaskMemory( StaticTask_t ** ppxIdleTaskTCBBuffer, 
																		StackType_t ** ppxIdleTaskStackBuffer,
																		uint32_t *pulIdleTaskStackSize )//���������ڴ����
{
	* ppxIdleTaskTCBBuffer=&idle_tack_tcb;
	* ppxIdleTaskStackBuffer=idle_task_stack;
	*pulIdleTaskStackSize=configMINIMAL_STACK_SIZE;
}


StaticTask_t time_tack_tcb;
StackType_t time_task_stack[configTIMER_TASK_STACK_DEPTH];
void vApplicationGetTimerTaskMemory( StaticTask_t ** ppxTimerTaskTCBBuffer, 
																StackType_t ** ppxTimerTaskStackBuffer, 
																uint32_t *pulTimerTaskStackSize )
{//�����ʱ���ڴ����
				* ppxTimerTaskTCBBuffer=&time_tack_tcb;
				* ppxTimerTaskStackBuffer=time_task_stack;
				*pulTimerTaskStackSize=configTIMER_TASK_STACK_DEPTH;
}


