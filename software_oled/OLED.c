#include "OLED.h"
#include "OLED_Font.h"
#include "stdio.h"
#include "delay.h"

static void OLED_I2C_Delay(void)
{
	delay_1us(1);
}

/**
  * @brief  IIC��ʼ�ź�
  * @param  ��
  * @retval ��
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
  * @brief  IICֹͣ�ź�
  * @param  ��
  * @retval ��
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
  * @brief  I2C����һ���ֽ�
  * @param  Byte Ҫ���͵�һ���ֽ�
  * @retval ���IIC
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
  * @brief  OLEDд����
  * @param  Command Ҫд�������
  * @retval Ӳ��IIC
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//�ӻ���ַ
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(0x00);		//д����
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(Command);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

/**
  * @brief  OLEDд����
  * @param  Data Ҫд�������
  * @retval ���IIC
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//�ӻ���ַ
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(0x40);		//д����
	OLED_I2C_WaitAck();
	OLED_I2C_SendByte(Data);
	OLED_I2C_WaitAck();
	OLED_I2C_Stop();
}

/**
  * @brief  OLED��ʼ��
  * @param  ��
  * @retval ��
  */
void OLED_Init(void)
{
	delay_cycles(1000000);
	OLED_WriteCommand(0xAE);	//�ر���ʾ
	OLED_WriteCommand(0xD5);	//������ʾʱ�ӷ�Ƶ��/����Ƶ��
	OLED_WriteCommand(0x80);
	OLED_WriteCommand(0xA8);	//���ö�·������
	OLED_WriteCommand(0x3F);	//0x3f : 128*64  0x1f  128*32
	OLED_WriteCommand(0xD3);	//������ʾƫ��
	OLED_WriteCommand(0x00);	//Ĭ����ƫ��
	OLED_WriteCommand(0x40);	//������ʾ��ʼ��
	OLED_WriteCommand(0xA1);	//�������ҷ���0xA1���� 0xA0���ҷ���
	OLED_WriteCommand(0xC8);	//�������·���0xC8���� 0xC0���·���
	OLED_WriteCommand(0xDA);	//����COM����Ӳ������
	OLED_WriteCommand(0x12);
	OLED_WriteCommand(0x81);	//���öԱȶȿ���
	OLED_WriteCommand(0xCF);
	OLED_WriteCommand(0xD9);	//����Ԥ�������
	OLED_WriteCommand(0xF1);
	OLED_WriteCommand(0xDB);	//����VCOMHȡ��ѡ�񼶱�
	OLED_WriteCommand(0x30);
	OLED_WriteCommand(0xA4);	//����������ʾ��/�ر�
	OLED_WriteCommand(0xA6);	//��������/��ת��ʾ
	OLED_WriteCommand(0x8D);	//���ó���
	OLED_WriteCommand(0x14);	//��������ʾ������ʱ��ʹ�ã�0x10:�������ڿ���ǰʹ��
	OLED_WriteCommand(0xAF);	//������ʾ
	OLED_Clear();				//OLED����
}

/**
  * @brief  OLED���ù��λ��
  * @param  Y �����Ͻ�Ϊԭ�㣬���·�������꣬��Χ��0~7
  * @param  X �����Ͻ�Ϊԭ�㣬���ҷ�������꣬��Χ��0~127
  * @retval ��
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//����Yλ��
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//����Xλ�ø�4λ
	OLED_WriteCommand(0x00 | (X & 0x0F));			//����Xλ�õ�4λ
}

/**
  * @brief  OLED����
  * @param  ��
  * @retval ��
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
 * @brief  OLED_ON����OLED�������л���
 * @param  ��
 * @retval ��
 */
void OLED_ON(void)
{
	OLED_WriteCommand(0X8D);  //���õ�ɱ�
	OLED_WriteCommand(0X14);  //������ɱ�
	OLED_WriteCommand(0XAF);  //OLED����
}


/**
 * @brief  OLED_OFF����OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
 * @param  ��
 * @retval ��
 */
void OLED_OFF(void)
{
	OLED_WriteCommand(0X8D);  //���õ�ɱ�
	OLED_WriteCommand(0X10);  //�رյ�ɱ�
	OLED_WriteCommand(0XAE);  //OLED����
}

/**
  * @brief  OLED��ʾһ���ַ�
  * @param  Line ��λ�ã���Χ��1~4
  * @param  Column ��λ�ã���Χ��1~16
  * @param  Char Ҫ��ʾ��һ���ַ�����Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//���ù��λ�����ϰ벿��
	for (i = 0; i < 8; i++) {
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//��ʾ�ϰ벿������
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//���ù��λ�����°벿��
	for (i = 0; i < 8; i++) {
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//��ʾ�°벿������
	}
}

/**
  * @brief  OLED��ʾ�ַ���
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  String Ҫ��ʾ���ַ�������Χ��ASCII�ɼ��ַ�
  * @retval ��
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++) {
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED�η�����
  * @retval ����ֵ����X��Y�η�
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
  * @brief  OLED��ʾ���֣�ʮ���ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~4294967295
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++) {
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED��ʾ���֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��-2147483648~2147483647
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @retval ��
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
  * @brief  OLED��ʾ���֣�ʮ�����ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~0xFFFFFFFF
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~8
  * @retval ��
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
  * @brief  OLED��ʾ���֣������ƣ�������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ�����֣���Χ��0~1111 1111 1111 1111
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~16
  * @retval ��
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++) {
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  OLED��ʾ�������֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��1~16
  * @param  Number Ҫ��ʾ������
  * @param  Length Ҫ��ʾ���ֵĳ��ȣ���Χ��1~10
  * @param  Flength Ҫ��ʾ��С�����λ
  * @retval MSPM0G3507���ܷɳ���
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
	//��������ת��������Ȼ����ʾ
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
  * @brief  OLED��ʾ�������֣�ʮ���ƣ�����������
  * @param  Line ��ʼ��λ�ã���Χ��1~4
  * @param  Column ��ʼ��λ�ã���Χ��0~255
  * @param  Number Ҫ��ʾ������
  * @param  Length Ҫ��ʾ��С�����λ
  * @retval MSPM0G3507����ʹ��
  */
void OLED_ShowFloat(uint8_t Line, uint8_t Column, float Number, uint8_t Length)
{
	char Data[] = "  ";
	sprintf((char *)Data, "%.*f", Length, Number);
	OLED_ShowString(Line, Column, Data);
}

/**
 * @brief  OLED_ShowCN����ʾOLED_Font.h�еĺ���,16*16����
 * @param  x,y: ��ʼ������(x:0~127, y:0~7);
 *		   N:������codetab.h�е�����
 * @retval ��
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
 * @brief  OLED_DrawBMP����ʾBMPλͼ
 * @param  x0,y0 :��ʼ������(x0:0~127, y0:0~7);
 * x1,y1 : ���Խ���(������)������(x1:1~128,y1:1~8)
 * @retval ��
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
