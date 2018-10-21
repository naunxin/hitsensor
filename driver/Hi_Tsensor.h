#ifndef __HI_TSENSOR_H__ 
#define __HI_TSENSOR_H__

//HiTsensor 数据结构体
typedef struct {
	unsigned int  adress;
	unsigned int  value;
}Hi_Tsensor_info;

#define SET_MODE		0x100	//设置采集模式，单次采集value设为0，循环采集value设为1
#define SET_PERIOD		0x200	//循环采集周期设置，单位为ms，value设置范围2ms-512ms
#define SET_OVERUP		0x300	//温度上限溢出报警阈值，value设置区间值-40-125摄氏度
#define SET_OVERDN		0x400	//温度下限溢出报警阈值，value区间值-40-125摄氏度
#define SET_START		0x500	//功能启动，value为1启动，value为0关闭 
#define GET_TEMPER		0x600	//读取当前温度值，value为返回值
#define GET_OVERUP		0x700	//读取温度上限溢出报警标志位，value为返回值
#define GET_OVERDN		0x800	//读取温度下限溢出报警标志位，value为返回值


#endif
