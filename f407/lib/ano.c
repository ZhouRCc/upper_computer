#include "main.h"
#include "ano.h"
#include "usart.h"
#include "bsp_delay.h"
#include <stdarg.h>
//此宏定义，使用串口1时将此处改为1，使用串口6时改为6
#define WHICH_UART 1


/******小端模式，低字节在前***/
/*********数据拆分************/
#define BYTE0(dwTemp)       (*(char *)(&dwTemp))	 
#define BYTE1(dwTemp)       (*((char *)(&dwTemp) + 1))	 
#define BYTE2(dwTemp)       (*((char *)(&dwTemp) + 2))
#define BYTE3(dwTemp)       (*((char *)(&dwTemp) + 3))

uint8_t BUFF[100];//存储数据

/**
  * @brief          匿名上位机初始化
  * @param[in]    	none
  * @retval         none
  */
void Ano_Init(void)
{
	delay_init();
	//enable receive interrupt and idle interrupt
	//使能接收中断和空闲中断
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);  //receive interrupt
	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);  //idle interrupt
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);  //receive interrupt
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);  //idle interrupt
}

/**
  * @brief          匿名上位机发送数据
  * @param[in]      被传递的数据，需均为int，首个参数为长度
  * @retval         none
  */

void Ano_Send(uint8_t len, ...)
{
	int i;
	int intbuf;
	uint8_t sumcheck = 0;
	uint8_t addcheck = 0;
	uint8_t _cnt=0;
	
	va_list args;

	va_start(args,len);

	BUFF[_cnt++]=FRAME_HEADER;//帧头
	BUFF[_cnt++]=GENERAL_OUTPUT_ADDR;
	BUFF[_cnt++]=0xF1;//灵活格式帧，0xF1-0xFA
	BUFF[_cnt++]=4 * len;// 数据长度

	while(len --)
	{
		intbuf=va_arg(args,int);
		BUFF[_cnt ++] = BYTE0(intbuf);
		BUFF[_cnt ++] = BYTE1(intbuf);
		BUFF[_cnt ++] = BYTE2(intbuf);
		BUFF[_cnt ++] = BYTE3(intbuf);
	}

	va_end(args);

	for(i=0;i<BUFF[3]+4;i++) 
	{
		sumcheck+=BUFF[i];
		addcheck+=sumcheck;
	}
	BUFF[_cnt++]=sumcheck;	
	BUFF[_cnt++]=addcheck;	
#if WHICH_UART == 1
	
	HAL_UART_Transmit(&huart1,BUFF,_cnt,0xFFF);
#elif WHICH_UART == 6
	HAL_UART_Transmit(&huart6,BUFF,_cnt,0xFFF);
#else
	#error WHICH_UART must be define 1 or 6
#endif
}


//中断回调函数
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	
}


