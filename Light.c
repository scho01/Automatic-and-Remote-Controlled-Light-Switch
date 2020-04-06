#include "Light.h"
#include "I2C.h"

bool light = true;
volatile uint32_t rawALS;
volatile uint16_t ir;
volatile uint16_t full;
volatile uint16_t visible;
volatile uint32_t lux;

uint8_t Address = (uint8_t)0x29;
bool init = false;
uint8_t gain = 0x00;
uint8_t integ = 0x00;
uint8_t Data_Send[2];
uint8_t Data_Receive[4];

bool Light_Init(void)
{
	Data_Send[0] = (uint8_t)0xB2;
	if (I2C_SendData(I2C1, Address, &Data_Send[0], 1) == 0)
	{
		I2C_ReceiveData(I2C1, Address, &Data_Receive[0], 1);
		if (Data_Receive[0] == (uint8_t)0xA2)
		{
			init = true;
			Light_Set_Gain(0x00);
			Light_Set_Time(0x00);
			Light_Disable();
			return true;
		}
	}
	return false;	
}

void Light_Enable(void)
{
	Data_Send[0] = 0xA0;
	Data_Send[1] = 0x93;
	I2C_SendData(I2C1, Address, &Data_Send[0], 2);
}

void Light_Disable(void)
{
	Data_Send[0] = 0xA0;
	Data_Send[1] = 0x00;
	I2C_SendData(I2C1, Address, &Data_Send[0], 2);	
}

void Light_Set_Gain(uint8_t gain1)
{
	Light_Enable();
	gain = gain1;
	Data_Send[0] = 0xA1;
	Data_Send[1] = gain | integ;
	I2C_SendData(I2C1, Address, &Data_Send[0], 2);
	Light_Disable();
}

void Light_Set_Time(uint8_t integ1)
{
	Light_Enable();
	integ = integ1;
	Data_Send[0] = 0xA1;
	Data_Send[1] = gain | integ;
	I2C_SendData(I2C1, Address, &Data_Send[0], 2);
	Light_Disable();
}

void Light_Get_ALS(void)
{
  Light_Enable();
	Data_Send[0] = 0xB4;
	I2C_SendData(I2C1, Address, &Data_Send[0], 1);
	I2C_ReceiveData(I2C1, Address, &Data_Receive[0], 4);
	rawALS = (((Data_Receive[3]<<24)|Data_Receive[2])<<16)|((Data_Receive[1]<<8)|Data_Receive[0]);
  Light_Disable();
  full = rawALS & 0xFFFF;
  ir = rawALS >> 16;
  visible = full - ir;
}

void Light_Calculate_Lux(void)
{
  float atime, again, cpl, lux1, lux2, lux3;
  if((full == 0xFFFF)|(ir == 0xFFFF)) {
    lux3 = 0;
    return;
  }
  atime = 100.0F;
  again = 1.0F;
  cpl = (atime * again) / (408.0F);
  lux1 = ((float)full - ((1.64F) * (float)ir)) / cpl;
  lux2 = (((0.59F) * (float)full ) - ((0.86F) * (float)ir)) / cpl;
  lux3 = lux1 > lux2 ? lux1 : lux2;
  lux = (uint32_t)lux3;
	if (lux < 50)
		light = false;
	else
		light = true;
}
