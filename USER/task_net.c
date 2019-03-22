#include "task_net.h"
#include "common.h"
#include "delay.h"
#include "net_protocol.h"
#include "rtc.h"
#include <nbiot.h>
#include <utils.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "platform.h"
#include "platform_config.h"
#include "bcxx.h"
#include "ringbuf.h"
#include "usart2.h"


TaskHandle_t xHandleTaskNET = NULL;
SensorMsg_S *p_tSensorMsgNet = NULL;			//����װ�ڴ��������ݵĽṹ�����
unsigned portBASE_TYPE NET_Satck;

u8 SignalIntensity = 0;						//M5310���ź�ǿ��


char uri[] = "coap://183.230.40.40:5683";
nbiot_device_t *dev = NULL;

nbiot_value_t in_put_current;			//������� float		3317 0 5700
nbiot_value_t out_put_current;			//������� float		3317 1 5700
nbiot_value_t in_put_voltage;			//�����ѹ float		3316 0 5700
nbiot_value_t out_put_voltage;			//�����ѹ float		3316 1 5700
nbiot_value_t in_put_freq;				//����Ƶ�� float		3318 0 5700
nbiot_value_t in_put_power_p;			//�й����� float		3328 0 5700
nbiot_value_t in_put_power_q;			//�޹����� float		3328 1 5700
nbiot_value_t in_put_power_s;			//���ڹ��� float		3328 2 5700
nbiot_value_t in_put_energy_p;			//�й����� float		3331 0 5805
nbiot_value_t in_put_energy_q;			//�޹����� float		3331 1 5805
nbiot_value_t in_put_energy_s;			//�������� float		3331 2 5805
nbiot_value_t signal_intensity;			//�ź�ǿ�� float		3330 0 5805

//nbiot_value_t light_control_switch;		//�ƾ߿��� Boolean		3311 0 5850
nbiot_value_t light_control_dimmer;		//�ƾ߿��� Integer		3311 0 5851
nbiot_value_t device_uuid;				//�豸UUID string		3308 0 5750
nbiot_value_t ota;						//�������� string		3308 1 5750
nbiot_value_t reset;					//Զ�̸�λ string		3308 2 5750
nbiot_value_t work_mode;				//����ģʽ string		3308 3 5750
nbiot_value_t time_strategy;			//ʱ����� string		3308 4 5750
nbiot_value_t data_upload_interval;		//�ϴ���� float		3308 5 5900
nbiot_value_t power_interface;			//��Դ�ӿ� float		3308 6 5900



void write_callback( uint16_t       objid,
                     uint16_t       instid,
                     uint16_t       resid,
                     nbiot_value_t  *data )
{
#ifdef DEBUG_LOG
    printf( "write /%d/%d/%d��%d\r\n",objid,instid,resid,data->value.as_bool);
#endif
//    if(objid == 3311 && instid == 0 && resid == 5850)			//���ص�
//	{
//		ControlLightSwitch();
//	}
	if(objid == 3311 && instid == 0 && resid == 5851)			//����
	{
		ControlLightLevel();
	}
	else if(objid == 3308 && instid == 0 && resid == 5750)		//����UUID
	{
		SetDeviceUUID();
	}
	else if(objid == 3308 && instid == 1 && resid == 5750)		//OTA
	{
		SetUpdateFirmWareInfo();
	}
	else if(objid == 3308 && instid == 2 && resid == 5750)		//Reset
	{
		ControlDeviceReset();
	}
	else if(objid == 3308 && instid == 3 && resid == 5750)		//����ģʽ
	{
		SetDeviceWorkMode();
	}
	else if(objid == 3308 && instid == 4 && resid == 5750)		//ʱ�����
	{
		SetRegularTimeGroups();
	}
	else if(objid == 3308 && instid == 5 && resid == 5900)		//�ϴ����
	{
		SetDeviceUpLoadINCL();
	}
	else if(objid == 3308 && instid == 6 && resid == 5900)		//��Դ�ӿ�
	{
		SetDevicePowerIntfc();
	}
}

void read_callback( uint16_t       objid,
                    uint16_t       instid,
                    uint16_t       resid,
                    nbiot_value_t *data )
{
#ifdef DEBUG_LOG
	printf( "read /%d/%d/%d\r\n",objid,instid,resid );
#endif
}

void execute_callback( uint16_t       objid,
                       uint16_t       instid,
                       uint16_t       resid,
                       nbiot_value_t  *data,
                       const void     *buff,
                       size_t         size)
{
#ifdef DEBUG_LOG
    printf( "execute /%d/%d/%d\r\n",objid,instid,resid );
#endif
}

#ifdef NOTIFY_ACK
void notify_ack_callback( uint16_t       objid,
                          uint16_t       instid,
                          uint16_t       resid,
                          nbiot_value_t *data,
                          bool           ack )
{
#ifdef DEBUG_LOG
    printf( "notify ack /%x/%x/%x(%s)\r\n",objid,instid,resid,ack ? "true":"false" );
#endif
}
#endif


void res_update(time_t interval)
{
	static time_t last_time = 0;
	static time_t cur_time = 0;
	
	if(cur_time >= last_time + interval)
	{
		cur_time = 0;
		last_time = 0;
		
		out_put_current.flag 	|= NBIOT_UPDATED;
		out_put_voltage.flag 	|= NBIOT_UPDATED;
		signal_intensity.flag 	|= NBIOT_UPDATED;
		
		in_put_current.flag 	|= NBIOT_UPDATED;
		out_put_current.flag 	|= NBIOT_UPDATED;
		in_put_voltage.flag 	|= NBIOT_UPDATED;
		out_put_voltage.flag 	|= NBIOT_UPDATED;
		in_put_freq.flag 		|= NBIOT_UPDATED;
		in_put_power_p.flag 	|= NBIOT_UPDATED;
		in_put_power_q.flag 	|= NBIOT_UPDATED;
		in_put_power_s.flag 	|= NBIOT_UPDATED;
		in_put_energy_p.flag 	|= NBIOT_UPDATED;
		in_put_energy_q.flag 	|= NBIOT_UPDATED;
		in_put_energy_s.flag 	|= NBIOT_UPDATED;
		signal_intensity.flag 	|= NBIOT_UPDATED;
	}
	else if(cur_time == 0 && last_time == 0)
	{
		cur_time = nbiot_time();
		last_time = cur_time;
	}
	else
	{
		cur_time = nbiot_time();
	}
}


int create_device(void)
{
	int ret = 0;
	int life_time = 200;

	ret = nbiot_device_create( &dev,
                               life_time,
                               write_callback,
                               read_callback,
                               execute_callback );

	if ( ret )
	{
		nbiot_device_destroy( dev );
#ifdef DEBUG_LOG
		printf( "device create failed, code = %d.\r\n", ret );
#endif
	}

	return ret;
}

int add_object_resource(void)
{
	int ret = 0;

	in_put_current.type = NBIOT_FLOAT;
	in_put_current.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3317,0,5700,1,0,&in_put_current,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_current) failed, code = %d.\r\n", ret);
#endif
	}
	
	out_put_current.type = NBIOT_FLOAT;
	out_put_current.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3317,1,5700,1,0,&out_put_current,0,1);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(out_put_current) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_voltage.type = NBIOT_FLOAT;
	in_put_voltage.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3316,0,5700,1,0,&in_put_voltage,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_voltage) failed, code = %d.\r\n", ret);
#endif
	}
	
	out_put_voltage.type = NBIOT_FLOAT;
	out_put_voltage.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3316,1,5700,1,0,&out_put_voltage,0,1);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(out_put_voltage) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_freq.type = NBIOT_FLOAT;
	in_put_freq.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3318,0,5700,1,0,&in_put_freq,0,1);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_freq) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_power_p.type = NBIOT_FLOAT;
	in_put_power_p.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3328,0,5700,1,0,&in_put_power_p,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_power_p) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_power_q.type = NBIOT_FLOAT;
	in_put_power_q.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3328,1,5700,1,0,&in_put_power_q,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_power_q) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_power_s.type = NBIOT_FLOAT;
	in_put_power_s.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3328,2,5700,1,0,&in_put_power_s,0,1);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_power_s) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_energy_p.type = NBIOT_FLOAT;
	in_put_energy_p.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3331,0,5805,1,0,&in_put_energy_p,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_energy_p) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_energy_q.type = NBIOT_FLOAT;
	in_put_energy_q.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3331,1,5805,1,0,&in_put_energy_q,0,0);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_energy_q) failed, code = %d.\r\n", ret);
#endif
	}
	
	in_put_energy_s.type = NBIOT_FLOAT;
	in_put_energy_s.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3331,2,5805,1,0,&in_put_energy_s,0,1);
	if (ret)
	{
#ifdef DEBUG_LOG
		nbiot_device_destroy(dev);
		printf("device add resource(in_put_energy_s) failed, code = %d.\r\n", ret);
#endif
	}
	
	signal_intensity.type = NBIOT_FLOAT;
	signal_intensity.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3330,0,5700,1,0,&signal_intensity,0,1);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(signal_intensity) failed, code = %d.\r\n", ret);
#endif
	}
	
	light_control_dimmer.type = NBIOT_INTEGER;
	light_control_dimmer.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3311,0,5851,1,0,&light_control_dimmer,0,1);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(light_control_dimmer) failed, code = %d.\r\n", ret);
#endif
	}

	device_uuid.type = NBIOT_STRING;
	device_uuid.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,0,5750,1,0,&device_uuid,0,0);
	
	if(DeviceUUID != NULL)
	{
		nbiot_free(device_uuid.value.as_buf.val);
		device_uuid.value.as_buf.val = nbiot_strdup((char *)DeviceUUID, UU_ID_LEN - 2);
		device_uuid.value.as_buf.len = UU_ID_LEN - 2;
	}
	
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(device_uuid) failed, code = %d.\r\n", ret);
#endif
	}
	
	ota.type = NBIOT_STRING;
	ota.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,1,5750,1,0,&ota,0,0);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(ota) failed, code = %d.\r\n", ret);
#endif
	}
	
	reset.type = NBIOT_STRING;
	reset.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,2,5750,1,0,&reset,0,0);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(reset) failed, code = %d.\r\n", ret);
#endif
	}
	
	work_mode.type = NBIOT_STRING;
	work_mode.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,3,5750,1,0,&work_mode,0,0);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(work_mode) failed, code = %d.\r\n", ret);
#endif
	}
	
	time_strategy.type = NBIOT_STRING;
	time_strategy.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,4,5750,1,0,&time_strategy,0,0);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(time_strategy) failed, code = %d.\r\n", ret);
#endif
	}
	
	data_upload_interval.type = NBIOT_FLOAT;
	data_upload_interval.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,5,5900,1,0,&data_upload_interval,0,0);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(data_upload_interval) failed, code = %d.\r\n", ret);
#endif
	}
	
	power_interface.type = NBIOT_FLOAT;
	power_interface.flag = NBIOT_READABLE|NBIOT_WRITABLE;
	ret = nbiot_resource_add(dev,3308,6,5900,1,0,&power_interface,0,1);
	if (ret)
	{
		nbiot_device_destroy(dev);
#ifdef DEBUG_LOG
		printf("device add resource(power_interface) failed, code = %d.\r\n", ret);
#endif
	}

	return ret;
}

void unregister_all_things(void)
{
	nbiot_device_close(dev,0);
	nbiot_device_destroy(dev);
	nbiot_clear_environment();
	
	USART2_Init(9600);
}

void sync_sensor_data(void)
{
	BaseType_t xResult;
	
	xResult = xQueueReceive(xQueue_sensor,
							(void *)p_tSensorMsgNet,
							(TickType_t)pdMS_TO_TICKS(50));
	if(xResult == pdPASS)
	{
		out_put_current.value.as_float 	= (double)p_tSensorMsgNet->out_put_current;
		out_put_voltage.value.as_float 	= (double)p_tSensorMsgNet->out_put_voltage;
		signal_intensity.value.as_float = (double)p_tSensorMsgNet->signal_intensity;
		
		in_put_current.value.as_float 	= 500;
		out_put_current.value.as_float 	= 500;
		in_put_voltage.value.as_float 	= 220;
//		out_put_voltage.value.as_float 	= 100;
		in_put_freq.value.as_float 		= 50;
		in_put_power_p.value.as_float 	= 100;
		in_put_power_q.value.as_float 	= -10;
		in_put_power_s.value.as_float 	= 110;
		in_put_energy_p.value.as_float 	= 1000;
		in_put_energy_q.value.as_float 	= 50;
		in_put_energy_s.value.as_float 	= 1050;
//		signal_intensity.value.as_float = ;
	}
}

//��ָ����NTP��������ȡʱ��
u8 SyncDataTimeFormBcxxModule(time_t sync_cycle)
{
	u8 ret = 0;
	struct tm tm_time;
	static time_t time_s = 0;
	char buf[32];

	if((GetSysTick1s() - time_s >= sync_cycle))
	{
		memset(buf,0,32);

		if(bcxx_get_AT_CCLK(buf))
		{
			tm_time.tm_year = 2000 + (buf[0] - 0x30) * 10 + buf[1] - 0x30 - 1900;
			tm_time.tm_mon = (buf[3] - 0x30) * 10 + buf[4] - 0x30 - 1;
			tm_time.tm_mday = (buf[6] - 0x30) * 10 + buf[7] - 0x30;

			tm_time.tm_hour = (buf[9] - 0x30) * 10 + buf[10] - 0x30;
			tm_time.tm_min = (buf[12] - 0x30) * 10 + buf[13] - 0x30;
			tm_time.tm_sec = (buf[15] - 0x30) * 10 + buf[16] - 0x30;

			time_s = mktime(&tm_time);

			time_s += 28800;

			SyncTimeFromNet(time_s);

			GetTimeOK = 1;
			
			ret = 1;
		}
	}

	return ret;
}

void vTaskNET(void *pvParameters)
{
	int ret = 0;
	time_t sync_csq_time = nbiot_time();

	p_tSensorMsgNet = (SensorMsg_S *)mymalloc(sizeof(SensorMsg_S));

	RE_INIT_BCXX:
	nbiot_init_environment();			//��ʼ��BC35

	ret = create_device();

	if(ret)
	{
		goto RE_INIT_BCXX;
	}

	ret = add_object_resource();

	if(ret)
	{
		goto RE_INIT_BCXX;
	}

	ret = nbiot_device_connect(dev,100);

	if(ret)
	{
#ifdef DEBUG_LOG
		printf( "connect OneNET failed.\r\n" );
#endif

		unregister_all_things();

		goto RE_INIT_BCXX;
	}
	else
	{
#ifdef DEBUG_LOG
		 printf( "connect OneNET success.\r\n" );
#endif
	}

	while(1)
	{
		if(Registered_Flag == 1)
		{
			if(nbiot_time() - sync_csq_time >= 10)
			{
				sync_csq_time = nbiot_time();
				
				SignalIntensity = bcxx_get_csq();
				
				SyncDataTimeFormBcxxModule(3600);
			}
		}
		
		ret = nbiot_device_step( dev, -1);

		if ( ret )
		{
#ifdef DEBUG_LOG
			printf( "device step error, code = %d.\r\n", ret );
#endif
//			if(ret != -12)
//			{
				unregister_all_things();

				goto RE_INIT_BCXX;
//			}
		}
		else
		{
			res_update(1200);
		}
		
		sync_sensor_data();
		
		delay_ms(100);

//		NET_Satck = uxTaskGetStackHighWaterMark(NULL);
	}
}






























