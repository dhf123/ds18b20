#include "stm32f0xx.h"
#include "led.h"
#include "usart.h"
#include "timer.h"
#include "adc.h"
#include "delay.h"
#include "ds18b20.h"
#include "wdog.h"
#include "exti.h"
#include <stdio.h>



#define LED_OFF                       GPIOA->BSRR = 0x40
#define LED_ON                        GPIOA->BRR = 0x40 
#define LED_TURN                      GPIOA->ODR ^= 0x40

#define LIGHT1_ON                       GPIOA->BSRR = 0x10
#define LIGHT1_OFF                        GPIOA->BRR = 0x10 
#define LIGHT1_TURN                      GPIOA->ODR ^= 0x10

#define LIGHT_ON                       GPIOA->BSRR = 0x80//LIGHT_ON
#define LIGHT_OFF                       GPIOA->BRR = 0x80//LIGHT_OFF
#define LIGHT_TURN											GPIOA->ODR ^= 0x80

#define BEEP_OFF  											 GPIO_SetBits(GPIOA,GPIO_Pin_5);	//�ߵ�ƽ
#define BEEP_ON                         GPIO_ResetBits(GPIOA,GPIO_Pin_5);//�͵�ƽ
#define BEEP_TURN 											GPIOA->ODR ^= 0x20
#define disable_irq() __disable_irq() 
#define enable_irq() __enable_irq() 


//�¶����¶ȴ��������
u8 T_cunzai=0;
u8 T_set;//�趨�¶�ֵ
extern u8 ds18b20_id[2][8];
short temperature;
//�����¶��������
u8 first_get_T_flag;//���ڳ���ʼ�ɼ��¶�����ֵ���ɼ���ɺ����㣬����ϵͳ��λ���²ɼ�

//��βɼ��¶�
u8 get_temp_times_over_zero;
u8 get_temp_times_over_fifteen;

int main(void)
{
	/*ϵͳʱ�ӳ�ʼ��Ϊ48M
	����ʱ������:
	LSI_RC=40K	HSI_RC=8M HSE=8M HSI14_RC=14M
	PLL=SYSCLK=HCLK=AHB=FCLK=APB1=48M
	*/
	SystemInit();
	/*��systick��Ϊϵͳ��ʱ���ȶ�ʱ��*/
	delay_init();
	usart_init();
	gpio_init();
	LED_OFF;
	delay_ms(500);
	LIGHT_OFF;
	LIGHT1_OFF;

	while(DS18B20_Init())		
	{
 		delay_ms(200);
	}	
	T_cunzai = 1;
	//printf("DS18B20 OK\n");
	/*��ȡ�¶ȴ�����ID*/
	DS18B20_ReadID(*ds18b20_id);
	
	//�¶����òɼ�
	first_get_T_flag=1;//�ɼ���ɺ�����
	
	/*��ʱ���жϳ�ʼ��*/
	timer14_init();
	
	
	
	
	while(1)          
	{	
		temperature=DS18B20S_Get_Temp(1);
		/*���¶ȴ�����ֵ��ӡ��������ʾ*/
		printf("\n");
		printf("temperature=%d",temperature/10);
		if(temperature>=150)
		{
			LED_ON;
			get_temp_times_over_fifteen++;
			if(get_temp_times_over_fifteen >=10)
			{
				LIGHT1_OFF;//�رռ���ģ��
				get_temp_times_over_fifteen = 0;
			}
			
		}
		else
		{
			LED_OFF;
			get_temp_times_over_fifteen = 0;
		}
		
		if(temperature>=0)
		{
			get_temp_times_over_zero++;
			if(get_temp_times_over_zero>=10)
			{
				LIGHT_ON;//��Դ��
				get_temp_times_over_zero = 0;
			}
			
		}
		else if(temperature<0)
		{
			LIGHT1_ON;//��������ģ��
			LIGHT_OFF;//��Դ��
			get_temp_times_over_zero = 0;
		}
		
	}
	
	

}

/*1s�ж�һ��*/
void TIM14_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) //���ָ����TIM�жϷ������:TIM �ж�Դ 
	{
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);  //���TIMx���жϴ�����λ:TIM �ж�Դ 
		disable_irq();
		
		enable_irq();	
	}
}


/*�ⲿ�ж�����*/
void EXTI4_15_IRQHandler(void)
{  
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET)  //�����ж�
	{  
		
		
	}  

	EXTI_ClearFlag(EXTI_Line9);  
} 



