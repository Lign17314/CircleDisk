
# 时钟芯片的使用
lmk04808为双环路PLL，先进行简单的使用，板上晶振源为TXC晶振，频率为125M，lmk04808外部滤波环路滤波中心频率为3000Mhz，所有这里需要设置为3000Mhz，可自行设置分频系数，下图计算为3000=125*2*12，需注意TiCS pro的一些小问题，VCO Divider Mux的设置不会反映到PLL2设置页面去。
![image](https://user-images.githubusercontent.com/52576519/151962793-33ac5c78-41a8-4326-87bd-369dcc27e216.png)
User control页面，主要更改oscin频率范围
![image](https://user-images.githubusercontent.com/52576519/152682767-6a8a9d4c-0b63-406d-a5ae-94d817c3b4a5.png)
Clock out 页面，更改分频系数得到需要的频率，如3000/75=40，输出到射频端和网卡端，板上网络phy芯片需要提供125m时钟给gtx使用
![image](https://user-images.githubusercontent.com/52576519/152682762-b21cbd5d-3f9b-4c51-b39e-44a6e304cb33.png)
导出 HexRegisterValues.txt文件，使用python处理成数组然后写入c代码中，将其中的数据写入lmk04808即可
lmk读写时序
Write(R0-R5 需要额外的时钟，使用写两次reg实现)
![image](https://user-images.githubusercontent.com/52576519/152682547-553015cf-027e-42db-8568-41aecfc25b32.png)
Readback
![image](https://user-images.githubusercontent.com/52576519/152682560-45cfa205-2876-4989-96ba-9b04be0d7708.png)
该芯片的microwire暂时无法使用zynq的硬件spi实现，问题未知，故使用mio模拟microwire总线
# Vivado 工程搭建
使用gpio模拟microwire，测试文件见helloworld.c文件，可读可写
