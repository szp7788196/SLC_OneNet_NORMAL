#include "task_sensor.h"
#include "delay.h"
#include "sht2x.h"
#include "bh1750.h"
#include "task_net.h"
#include "common.h"
#include "inventr.h"
#include "rtc.h"
#include "usart.h"
#include "att7059x.h"

float InputCurrent = 0;
float InputVoltage = 0;
float InputFreq = 0.0f;
float InputPowerP = 0.0f;
float InputPowerQ = 0.0f;
float InputPowerS = 0.0f;
float InputEnergyP = 0.0f;
float InputEnergyQ = 0.0f;
float InputEnergyS = 0.0f;

TaskHandle_t xHandleTaskSENSOR = NULL;

SensorMsg_S *p_tSensorMsg = NULL;	
unsigned portBASE_TYPE SENSOR_Satck;
void vTaskSENSOR(void *pvParameters)
{
	time_t times_sec = 0;

	p_tSensorMsg = (SensorMsg_S *)mymalloc(sizeof(SensorMsg_S));

	while(1)
	{
		if(GetSysTick1s() - times_sec >= UpLoadINCL)		//ÿ��10�����һ�δ���������
		{
			times_sec = GetSysTick1s();

			InventrOutPutCurrent 	= InventrGetOutPutCurrent();	//��ȡ��Դ�������
			delay_ms(500);
			InventrOutPutVoltage 	= InventrGetOutPutVoltage();	//��ȡ��Դ�����ѹ
			delay_ms(300);
			InputCurrent 			= Att7059xGetCurrent1();
			delay_ms(300);
			InputVoltage 			= Att7059xGetVoltage();
			delay_ms(300);
			InputFreq 				= Att7059xGetVoltageFreq();
			delay_ms(300);
			InputPowerP 			= Att7059xGetChannel1PowerP();
			delay_ms(300);
			InputPowerQ 			= Att7059xGetChannel1PowerQ();
			delay_ms(300);
			InputPowerS 			= Att7059xGetChannel1PowerS();
			delay_ms(300);
			InputEnergyP 			= Att7059xGetEnergyP();
			delay_ms(300);
			InputEnergyQ 			= Att7059xGetEnergyQ();
			delay_ms(300);
			InputEnergyS 			= Att7059xGetEnergyS();
			delay_ms(300);
			
			if( dev->state == STATE_REGISTERED ||
				dev->state == STATE_REG_UPDATE_PENDING ||
				dev->state == STATE_REG_UPDATE_NEEDED)			//�豸��ʱ������״̬
			{
				p_tSensorMsg->in_put_current 	= InputCurrent;
				p_tSensorMsg->in_put_voltage 	= InputVoltage;
				p_tSensorMsg->in_put_freq 		= InputFreq;
				p_tSensorMsg->in_put_power_p 	= InputPowerP;
				p_tSensorMsg->in_put_power_q 	= InputPowerQ;
				p_tSensorMsg->in_put_power_s 	= InputPowerS;
				p_tSensorMsg->in_put_energy_p 	= InputEnergyP;
				p_tSensorMsg->in_put_energy_q 	= InputEnergyQ;
				p_tSensorMsg->in_put_energy_s 	= InputEnergyS;
				p_tSensorMsg->out_put_current = InventrOutPutCurrent;
				p_tSensorMsg->out_put_voltage = InventrOutPutVoltage;
				p_tSensorMsg->signal_intensity = (float)((-113) + (SignalIntensity * 2));
				p_tSensorMsg->hour = calendar.hour;
				p_tSensorMsg->minute = calendar.min;
				p_tSensorMsg->second = calendar.sec;

				memset(p_tSensorMsg->gps,0,32);

				if(GpsInfo != NULL && strlen((char *)GpsInfo) <= 32)
				{
					memcpy(p_tSensorMsg->gps,GpsInfo,strlen((char *)GpsInfo));
				}
				else
				{
					memcpy(p_tSensorMsg->gps,"3948.0975N11632.7539E",21);
				}

				if(xQueueSend(xQueue_sensor,(void *)p_tSensorMsg,(TickType_t)10) != pdPASS)
				{
#ifdef DEBUG_LOG
					printf("send p_tSensorMsg fail 1.\r\n");
#endif
				}
			}
		}

		delay_ms(100);
	}
}






































