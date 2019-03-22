#ifndef __TASK_NET_H
#define __TASK_NET_H

#include "sys.h"
#include "rtos_task.h"
#include "task_sensor.h"
#include "internal.h"




extern TaskHandle_t xHandleTaskNET;
extern SensorMsg_S *p_tSensorMsgNet;

extern u8 SignalIntensity;				//bg96���ź�ǿ��

extern nbiot_device_t *dev;

extern nbiot_value_t in_put_current;			//������� float		3317 0 5700
extern nbiot_value_t out_put_current;			//������� float		3317 1 5700
extern nbiot_value_t in_put_voltage;			//�����ѹ float		3316 0 5700
extern nbiot_value_t out_put_voltage;			//�����ѹ float		3316 1 5700
extern nbiot_value_t in_put_freq;				//����Ƶ�� float		3318 0 5700
extern nbiot_value_t in_put_power_p;			//�й����� float		3328 0 5700
extern nbiot_value_t in_put_power_q;			//�޹����� float		3328 1 5700
extern nbiot_value_t in_put_power_s;			//���ڹ��� float		3328 2 5700
extern nbiot_value_t in_put_energy_p;			//�й����� float		3331 0 5805
extern nbiot_value_t in_put_energy_q;			//�޹����� float		3331 0 5805
extern nbiot_value_t in_put_energy_s;			//�������� float		3331 0 5805
extern nbiot_value_t signal_intensity;			//�ź�ǿ�� float		3331 0 5805

//extern nbiot_value_t light_control_switch;		//�ƾ߿��� Boolean		3311 0 5850
extern nbiot_value_t light_control_dimmer;		//�ƾ߿��� Integer		3311 0 5851
extern nbiot_value_t device_uuid;				//�豸UUID string		3308 0 5750
extern nbiot_value_t ota;						//�������� string		3308 0 5750
extern nbiot_value_t reset;						//Զ�̸�λ string		3308 1 5750
extern nbiot_value_t work_mode;					//����ģʽ string		3308 2 5750
extern nbiot_value_t time_strategy;				//ʱ����� string		3308 3 5750
extern nbiot_value_t data_upload_interval;		//�ϴ���� float		3308 0 5900
extern nbiot_value_t power_interface;			//��Դ�ӿ� float		3308 1 5900


void vTaskNET(void *pvParameters);
































#endif
