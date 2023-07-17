#include "stm32f10x.h"                  // Device header
#include "FreeRTOS.h"
#include "SysTick.h"
#include "task.h"
void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//外设时钟控制APB2
	GPIO_InitTypeDef GPIO_InitStruct;//结构体
	
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IPU;//配置端口驱动模式(上拉输入)
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_1 | GPIO_Pin_11|GPIO_Pin_0;//选择端口（可多选）
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;//设置IO口速度
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB,GPIO_PinSource0);
	
	GPIO_Init(GPIOB,&GPIO_InitStruct);//用结构体参数初始GPIO口
	EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.EXTI_Line=EXTI_Line0;
	EXTI_InitStruct.EXTI_LineCmd=ENABLE;
	EXTI_InitStruct.EXTI_Mode=EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger=EXTI_Trigger_Falling;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//与freertos相符
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel=EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;//只使用抢占优先级
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
