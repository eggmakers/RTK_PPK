### 诺瓦泰615/617d板卡

##### 硬件连接

MCU UART 2 <-->  novtal串口1 需要注意要跳线帽选择连接MCU和novtel . （另一端是直接跳线帽连接novtel和外部串口接口）  该串口可以用来做为debug

MCU UART 3 < -- > novtel 串口2



### 串口驱动解析

采用dma中断加串口空闲中断的形式，再把数据拷贝到环形队列中 



### 调试过程

安装novtal conncet 在Tools/NovAtelConnectSetup.zip

先用串口助手直连novtel板卡

###### 注意：用串口连接到Novtal的上位机的时候，软件会把波特率改为921600

协议参考：Document/NovTel/Firmware_refference_manual.pdf



板卡自带3V3，5V，1v2，1V8，电压电流温度的监控，需要跟硬件确定这部分功能是否实现了 参考软件上中间标题栏的 Status - >Advance status 中可以看出来



命令：

LOG COM1 BESTPOSA ONTIME 1		// 以novtel 的asscci协议在串口1输出1hz的位置信息

log rxconfiga once // 输出当前的配置

unlogall 	// 取消所有输出





### 基站配置

Com com2 57600 n 8 1 n off

Interfacemode com2 none rtcmv3 off

Posave on 0.01【求该点的平均坐标,如果每次坐标点变化了都需要输入】

//[如果有已知点坐标，则输入fix position [纬度（°）] [经度（°）] [高（m）]   //fix position 40.1011567611 116.1315918139 68.473]

log com2 rtcm1084 ontime 1   

log com2 rtcm1005 ontime 10

log com2 rtcm1033 ontime 10

log com2 rtcm1074 ontime 1

log com2 rtcm1124 ontime 1

Saveconfig

 

### 千寻状态下的移动站配置

serialconfig com2 57600

Interfacemode com2 rtcmv3 novatel off

log com2 bestvel ontime 1

Saveconfig

 

