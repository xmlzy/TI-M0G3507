#include "OLED.h"
#include "OLED_Font.h"
#include "stdio.h"
#include "delay.h"

static void OLED_I2C_Delay(void)
{
	delay_1us(1);
}

/**
  * @brief  IIC开始信号
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN | OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_clearPins(OLED_PORT, OLED_SDA_PIN);
	OLED_I2C_Delay();
	DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
}

/**
  * @brief  IIC停止信号
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	DL_GPIO_clearPins(OLED_PORT, OLED_SDA_PIN);
	DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN);
	OLED_I2C_Delay();
}

void OLED_I2C_Ack(void)
{
	DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_clearPins(OLED_PORT, OLED_SDA_PIN);
	OLED_I2C_Delay();
	DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN);
	OLED_I2C_Delay();
}

void OLED_I2C_NAcK(void)
{
	DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN);
	OLED_I2C_Delay();
	DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
}

uint8_t OLED_I2C_WaitAck(void)
{
	uint8_t ret;
	DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN | OLED_SCL_PIN);
	OLED_I2C_Delay();
	if (DL_GPIO_readPins(OLED_PORT, OLED_SDA_PIN) != 0) {
		ret = 1;
	} else {
		ret = 0;
	}
	DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
	OLED_I2C_Delay();
	return ret;
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 软件IIC
  */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	for (i = 0; i < 8; i++) {
		if (Byte & 0x80) {
			DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN);
		} else {
			DL_GPIO_clearPins(OLED_PORT, OLED_SDA_PIN);
		}
		OLED_I2C_Delay();
		DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN);
		OLED_I2C_Delay();
		DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
		OLED_I2C_Delay();
		if (i == 7) {
			DL_GPIO_setPins(OLED_PORT, OLED_SDA_PIN);
			OLED_I2C_Delay();
		}
		Byte = Byte << 1;
	}
}

uint8_t OLED_I2C_ReadByte(void)
{
	uint8_t value = 0;
	uint8_t i;
	for (i = 0; i < 8; i++) {
		value = value << 1;
		DL_GPIO_setPins(OLED_PORT, OLED_SCL_PIN);
		OLED_I2C_Delay();
		if (DL_GPIO_readPins(OLED_PORT, OLED_SDA_PIN) != 0)
			value++;
		DL_GPIO_clearPins(OLED_PORT, OLED_SCL_PIN);
		OLED_I2C_Delay();
	}
	return value;
}

uint8_t I2C_CheckDevice(uint8_t Address)
{
	uint8_t ucACK;
	OLED_I2C_Stop();
	OLED_I2C_Start();
	OLED_I2C_SendByte(Address | 0);
	ucACK = OLED_I2C_WaitAck();
	OLED_I2C_Stop();
	return ucACK;
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 硬件IIC
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(0x00);		//写命令
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(Command);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 软件IIC
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(Data);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	delay_cycles(1000000);
	OLED_WriteCommand(0xAE);	//关闭显示
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);	//0x3f : 128*64  0x1f  128*32
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);	//默认无偏移
	OLED_WriteCommand(0x40);	//设置显示开始行
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置
	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);
	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);
	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);
	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭
	OLED_WriteCommand(0xA6);	//设置正常/倒转显示
	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);	//允许在显示开启的时候使用，0x10:不允许在开启前使用
	OLED_WriteCommand(0xAF);	//开启显示
	OLED_Clear();				//OLED清屏
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++) {
		OLED_SetCursor(j, 0);
		for (i = 0; i < 128; i++) {
			OLED_WriteData(0x00);
		}
	}
}

/**
 * @brief  OLED_ON，将OLED从休眠中唤醒
 * @param  无
 * @retval 无
 */
void OLED_ON(void)
{
	OLED_WriteCommand(0X8D);  //设置电荷泵
	OLED_WriteCommand(0X14);  //开启电荷泵
	OLED_WriteCommand(0XAF);  //OLED唤醒
}


/**
 * @brief  OLED_OFF，让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
 * @param  无
 * @retval 无
 */
void OLED_OFF(void)
{
	OLED_WriteCommand(0X8D);  //设置电荷泵
	OLED_WriteCommand(0X10);  //关闭电荷泵
	OLED_WriteCommand(0XAE);  //OLED休眠
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++) {
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++) {
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++) {
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--) {
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++) {
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0) {
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	} else {
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++) {
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++) {
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10) {
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		} else {
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++) {
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED显示浮点数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字
  * @param  Length 要显示数字的长度，范围：1~10
  * @param  Flength 要显示的小数点后几位
  * @retval MSPM0G3507会跑飞程序
  */
void OLED_ShowFNum(uint8_t Line, uint8_t Column, float Number, uint8_t Length, uint8_t Flength)
{
	uint8_t i;
	uint8_t flag = 1;
	float Number1;
	uint32_t Number2;
	if (Number >= 0) {
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	} else {
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	//将浮点数转换成整数然后显示
	Number2 = (int)(Number1 * OLED_Pow(10, Flength));

	for (i = Length; i > 0; i--) {
		if (i == (Length - Flength)) {
			OLED_ShowChar(Line, Column + i + flag, '.');
			flag = 0;
			OLED_ShowChar(Line, Column + i + flag, Number2 / OLED_Pow(10, Length - i) % 10 + '0');
		} else {
			OLED_ShowChar(Line, Column + i + flag, Number2 / OLED_Pow(10, Length - i) % 10 + '0');
		}
	}
}

/**
  * @brief  OLED显示浮点数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：0~255
  * @param  Number 要显示的数字
  * @param  Length 要显示的小数点后几位
  * @retval MSPM0G3507正常使用
  */
void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number, uint8_t Length)
{
	char Data[] = "  ";
	sprintf((char *)Data, "%.*f", Length, Number);
	OLED_ShowString(Line, Column, Data);
}

/**
 * @brief  OLED_ShowCN，显示OLED_Font.h中的汉字,16*16点阵
 * @param  x,y: 起始点坐标(x:0~127, y:0~7);
 *		   N:汉字在codetab.h中的索引
 * @retval 无
 */
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm = 0;
	unsigned int  adder = 32 * N;
	OLED_SetCursor(y, x);
	for (wm = 0; wm < 16; wm++) {
		OLED_WriteData(F16x16[adder]);
		adder += 1;
	}
	OLED_SetCursor(y + 1, x);
	for (wm = 0; wm < 16; wm++) {
		OLED_WriteData(F16x16[adder]);
		adder += 1;
	}
}


/**
 * @brief  OLED_DrawBMP，显示BMP位图
 * @param  x0,y0 :起始点坐标(x0:0~127, y0:0~7);
 * x1,y1 : 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
 * @retval 无
 */
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++) {
		OLED_SetCursor(y, x0);
		for (x = x0; x < x1; x++) {
			OLED_WriteData(BMP[j++]);
		}
	}
}
