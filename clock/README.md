
# 时钟芯片的使用
lmk04808为双环路PLL，先进行简单的使用，板上晶振源为TXC晶振，频率为125M，lmk04808外部滤波环路滤波中心频率为3000Mhz，所有这里需要设置为3000Mhz，可自行设置分频系数，下图计算为3000=125*2*12，需注意TiCS pro的一些小问题，VCO Divider Mux的设置不会反映到PLL2设置页面去。
![image](https://user-images.githubusercontent.com/52576519/151962793-33ac5c78-41a8-4326-87bd-369dcc27e216.png)
User control页面，主要更改oscin频率范围
![image](https://user-images.githubusercontent.com/52576519/152175240-fcd0b3a9-fc54-43ff-a770-e66e08f044ca.png)
Clock out 页面，更改分频系数得到需要的频率，如3000/75=40，输出到射频端和网卡端，板上网络phy芯片需要提供125m时钟给gtx使用
![image](https://user-images.githubusercontent.com/52576519/152175305-6698f72f-8cbd-4af4-a13c-e6982a9661d0.png)
导出 HexRegisterValues.txt文件，使用python处理成数组然后写入c代码中，将其中的数据写入lmk04808即可
注意，zynq硬件spi数据的大小端设置，不可直接将32位数据强制转换成8位送入buffer

# Vivado 工程搭建
![image](https://user-images.githubusercontent.com/52576519/152176902-61aa75b1-fcf0-4e7b-9bec-4399493ee80f.png)

勾选spi0，选择对应mio，在sdk中新建hello工程，导入c文件即可
