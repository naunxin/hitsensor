/************************************************************************************************
*****Describe: This program is writen to get HI35xx CPU Chip Temperature.                   *****
*****Email: lishuangliang@outlook.com                                              			*****
*****Author: shuang liang li																*****
*****Date: 2018-10-18																		*****
*************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/reboot.h>
#include <sys/ioctl.h>
#include <signal.h>
#include "Hi_Tsensor.h"

#ifdef debugprintf
	#define debugpri(mesg, args...) fprintf(stderr, "[HI Serial print:%s:%d:] " mesg "\n", __FILE__, __LINE__, ##args) 
#else
	#define debugpri(mesg, args...)
#endif

#define  HITSENSORDEV "/dev/hi_tsensor"
int hifd;

void Hi_Tsensor_sigsegv(int dummy)
{
	if(hifd > 0)
		close(hifd);
	fprintf(stderr, "Hi Tsensor Caught SIGSEGV, Abort!\n");
	fclose(stderr);
	abort();
}

void Hi_Tsensor_sigterm(int dummy)
{
	if(hifd > 0)
		close(hifd);
	fprintf(stderr, "Hi Tsensor Caught SIGTERM, Abort!\n");
	fclose(stderr);
	exit(0);
}

void Hi_init_signals(void)
{
	struct sigaction sa;
	sa.sa_flags = 0;
	
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGSEGV);
	sigaddset(&sa.sa_mask, SIGTERM);
	sigaddset(&sa.sa_mask, SIGPIPE);
	
	sa.sa_handler = Hi_Tsensor_sigsegv;
	sigaction(SIGSEGV, &sa, NULL);

	sa.sa_handler = Hi_Tsensor_sigterm;
	sigaction(SIGTERM, &sa, NULL);

	sa.sa_handler = SIG_IGN;
	sigaction(SIGPIPE, &sa, NULL);
}

int GetCurTemperature(int fd, float *tempera)
{
	float tmp;
	Hi_Tsensor_info hitsensor;
	
	//读取当前CPU温度值
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));		
	if(ioctl(fd, GET_TEMPER, &hitsensor) != 0)
	{
		debugpri("GET_TEMPER,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	debugpri("tmp %04x\n",hitsensor.value );
	tmp = ((hitsensor.value - 125)/806.0)*165-40;
	*tempera = tmp;
	printf("Hisi CPU current Temperature: %3.2f degree\n",tmp);
	
	//读取温度上限报警
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));		
	if(ioctl(fd, GET_OVERUP, &hitsensor) != 0)
	{
		debugpri("GET_OVERUP,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	if(hitsensor.value)
		printf("Hisi CPU current Temperature OVERUP\n");
	
	//读取温度下限报警
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));		
	if(ioctl(fd, GET_OVERDN, &hitsensor) != 0)
	{
		debugpri("GET_OVERDN,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	if(hitsensor.value)
		printf("Hisi CPU current Temperature OVERDOWN\n");
	return 0;
}

int main(int argc, char **argv)
{
	int ret = 0, i;
	float temperature;
	Hi_Tsensor_info hitsensor;
	
	Hi_init_signals();
	
	hifd = open(HITSENSORDEV,O_RDWR);
	if(hifd <= 0)
	{	
		printf("open %s error\n",HITSENSORDEV);
		return -1;
	}		
	
	//设置采集模式为循环采集
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));
	hitsensor.value = 1;
	if(ioctl(hifd, SET_MODE, &hitsensor) != 0)
	{
		debugpri("SET_MODE,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	
	//设置采集周期为16ms
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));
	hitsensor.value = 16;
	if(ioctl(hifd, SET_PERIOD, &hitsensor) != 0)
	{
		debugpri("SET_PERIOD,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	
	//设置温度报警上限阈值为85摄氏度
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));
	hitsensor.value = 85;
	hitsensor.value = (int)(((hitsensor.value+40)/165.0)*806+125);//温度值转换
	if(ioctl(hifd, SET_OVERUP, &hitsensor) != 0)
	{
		debugpri("SET_OVERUP,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	
	//设置温度报警下限阈值为5摄氏度
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));
	hitsensor.value = 5;
	hitsensor.value = (int)(((hitsensor.value+40)/165.0)*806+125);//温度值转换
	if(ioctl(hifd, SET_OVERDN, &hitsensor) != 0)
	{
		debugpri("SET_OVERDN,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	
	//启动温度采集
	memset(&hitsensor,0,sizeof(Hi_Tsensor_info));
	hitsensor.value = 1;
	if(ioctl(hifd, SET_START, &hitsensor) != 0)
	{
		debugpri("SET_START,value %d,error\r\n",hitsensor.value);
		return -1;
	}
	
	while(1)
	{
		GetCurTemperature(hifd,&temperature);		
		sleep(1);
		system("clear");
		printf("HiTsensor device detecting...\n");
	}	
	if(hifd > 0)
		close(hifd);

	return 0;
}

