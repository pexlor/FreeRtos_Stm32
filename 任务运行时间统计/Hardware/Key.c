#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//����ʱ�ӿ���APB2
	GPIO_InitTypeDef GPIO_InitStruct;//�ṹ��
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;//���ö˿�����ģʽ(��������)
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_11|GPIO_Pin_0;//ѡ��˿ڣ��ɶ�ѡ��
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//����IO���ٶ�
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);//�ýṹ�������ʼGPIO��
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//��freertos���
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;//ֻʹ����ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	//NVIC_Init(&NVIC_InitStruct);
	
	//GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);//Selects the GPIO pin used as EXTI Line.
}

uint8_t Key_GetNum(void){
	uint8_t KeyNum=0;
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0){
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)==0);
		delay_ms(20);
		KeyNum=1;
	};
	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0){
		delay_ms(20);
		while(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)==0);
		delay_ms(20);
		KeyNum=2;
	};
	return KeyNum;
}
