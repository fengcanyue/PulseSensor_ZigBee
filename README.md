
# PulseSensor_ZigBee
##环境：
使用iar8.30.3
##接线说明
脉搏传感器：P0_4  
温度传感器：P0_6  
串口：P0_2  P0_3  
rc522：  
 IC_CS P1_7  
 IC_SCK  P0_1  
 IC_MOSI P1_2  
 IC_MISO P0_4  
 IC_REST P0_5  
##演示说明：
打开工程文件PulseSensor_ZigBee\ZStack-CC2530-2.5.1a\Projects\zstack\Samples\SampleApp\CC2530DB\SampleApp.eww  
在工作区分别选择：CoordinatorEB协调器（控制中心节点），RouterEB路由器（组网站点），Router_Portable(携带设备)下载到不同的设备上。  
控制中心节点通过串口连接到电脑，上电后自动启动，再给携带设备和组网节点上电。  
在打开上位机程序或者串口调式助手，设置串口：端口号：COMx，波特率：38400，数据位：8，停止位：1，校验位：NONE，点击打开串口即可观察采集到的生理信号和位置信息。  


##刷卡功能：
在组网节点中刷卡，是将当前时间写入到卡片机中，在监控中心节点刷卡是将卡片中的时间读取出来，在串口助手中演示。  
通过按监控中心的S1按钮可将组网节点的时间设置为默认的时间，实现时间同步功能。  
