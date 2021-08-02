

1.版本
	标准版和普通版
	普通版做了SK1和SK3兼容，只发送UAVCAN中的ECEF数据
	标准版SK1是串口接收数据，SK3是CAN接收数据，发送fix2, auxliuxay, MagneticFieldStrength数据。
	
2.UAVCAN上位机端
	要实现参数获取，reboot, 更新固件等功能。


2020.6.30
	标准化进不了固定解---加上send gnss();一体控进入固定解，验证fix2是否进入了固定解。

2020.8.6
	修改：修改磁场读取不正确的bug
	
2020.9.4

	添加boot loader；
	RTCM数据链路测试OK；
	进入固定解OK；
	boot loader升级OK