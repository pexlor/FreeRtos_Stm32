#include "stm32f10x.h"                  // Device header
// 配置GPIO 选择时钟 选择模式 配置时基单元 配置NVIC
void Tmier_Init(void){
	
	//TIM_ETRClockMode2Config(TIM2,TIM_ExtTRGPSC_OFF,TIM_ExtTRGPolarity_NonInverted,0x00);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;//时钟预分频
	TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;//选择计算模式
	TIM_TimeBaseInitStruct.TIM_Period=10000-1;//Specifies the period value to be loaded into the activeAuto-Reload Register at the next update event.
	TIM_TimeBaseInitStruct.TIM_Prescaler=7200-1;//pecifies the prescaler value used to divide the TIM clock.
	TIM_TimeBaseInitStruct.TIM_RepetitionCounter=0;//重复计算器（通用计算器无用）
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStruct);//初始化时基单元
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);//初始化时基单元
	
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);//清除标志位
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);//清除标志位
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//优先级分组
	
	NVIC_InitTypeDef NVIC_InitStruct;//申明NVIC结构体
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM2_IRQn;//指定中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//中断使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=5;//指定抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//指定响应优先级
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel=TIM3_IRQn;//指定中断通道
	NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;//中断使能
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=4;//指定抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;//指定响应优先级
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(TIM2,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}

/*void TIM2_IRQHandler(void){
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET){
		Num++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
}*/