# CircleDisk
example to CircleDisk
# 时钟芯片的使用
lmk04808为双环路PLL，先进行简单的使用，板上晶振源为TXC晶振，频率为125M，lmk04808外部滤波环路滤波中心频率为3000Mhz，所有这里需要设置为3000Mhz，可自行设置分频系数，下图计算为3000=125*2*12，需注意TiCS pro的一些小问题，VCO Divider Mux的设置不会反映到PLL2设置页面去。
![image](https://user-images.githubusercontent.com/52576519/151962793-33ac5c78-41a8-4326-87bd-369dcc27e216.png)
User control页面，主要更改oscin频率范围
![image](https://user-images.githubusercontent.com/52576519/151963630-078b136c-e5ad-4b77-bd16-8d45b3a1cd08.png)
Clock out 页面，更改分频系数得到需要的频率，如3000/75=40，输出到射频端和网卡端，板上网络phy芯片需要提供125m时钟给gtx使用
![image](https://user-images.githubusercontent.com/52576519/151963839-a389abd5-18c6-4f56-a9fb-28194970d77a.png)
导出 HexRegisterValues.txt文件，将其中的数据写入lmk04808即可
