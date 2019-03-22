#include "task_main.h"
#include "common.h"
#include "delay.h"
#include "usart.h"
#include "inventr.h"
#include "pwm.h"
#include "mcp4725.h"


TaskHandle_t xHandleTaskMAIN = NULL;

u8 MirrorLightLevelPercent = 0;
u8 MirrorPowerINTFC = 0xFF;
unsigned portBASE_TYPE MAIN_Satck;

void vTaskMAIN(void *pvParameters)
{
	time_t times_sec = 0;

	SetLightLevel(PowerINTFC, 100);
	delay_ms(1000);
	SetLightLevel(PowerINTFC, INIT_LIGHT_LEVEL);

	while(1)
	{
		if(DeviceWorkMode == MODE_AUTO)						//ֻ�����Զ�ģʽ�²Ž��в����ж�
		{
			if(GetSysTick1s() - times_sec >= 1)
			{
				times_sec = GetSysTick1s();

				AutoLoopRegularTimeGroups(&LightLevelPercent);
			}
		}

		if(MirrorLightLevelPercent != LightLevelPercent || \
			MirrorPowerINTFC != PowerINTFC)
		{
			MirrorLightLevelPercent = LightLevelPercent;
			MirrorPowerINTFC = PowerINTFC;

			SetLightLevel(PowerINTFC, LightLevelPercent);
		}

		if(NeedToReset == 1)								//���յ�����������
		{
			NeedToReset = 0;
			delay_ms(1000);

			__disable_fault_irq();							//����ָ��
			NVIC_SystemReset();
		}

		delay_ms(100);
//		MAIN_Satck = uxTaskGetStackHighWaterMark(NULL);
	}
}

//��ѯʱ�����
void AutoLoopRegularTimeGroups(u8 *percent)
{
	u8 i = 0;

	if(GetTimeOK != 0)
	{
		for(i = 0; i < MAX_GROUP_NUM; i ++)
		{
			switch(RegularTimeStruct[i].type)
			{
				case TYPE_WEEKDAY:		//��һ������
					if(calendar.week >= 1 && calendar.week <= 5)		//�ж������Ƿ��ǹ�����
					{
						if(RegularTimeStruct[i].hour == calendar.hour &&
						   RegularTimeStruct[i].minute == calendar.min)
						{
							*percent = RegularTimeStruct[i].percent * 2;

							i = MAX_GROUP_NUM;
						}
					}
				break;

				case TYPE_WEEKEND:		//����������
					if(calendar.week >= 6 && calendar.week <= 7)		//�ж������Ƿ�������������
					{
						if(RegularTimeStruct[i].hour == calendar.hour &&
						   RegularTimeStruct[i].minute == calendar.min)
						{
							*percent = RegularTimeStruct[i].percent * 2;

							i = MAX_GROUP_NUM;
						}
					}
				break;

				case TYPE_HOLIDAY:		//�ڼ���
					if(RegularTimeStruct[i].year + 2000 == calendar.w_year &&
					   RegularTimeStruct[i].month == calendar.w_month &&
					   RegularTimeStruct[i].date == calendar.w_date &&
					   RegularTimeStruct[i].hour == calendar.hour &&
					   RegularTimeStruct[i].minute == calendar.min)
					{
						*percent = RegularTimeStruct[i].percent * 2;

						i = MAX_GROUP_NUM;
					}
				break;

				default:

				break;
			}
		}
	}
}


































