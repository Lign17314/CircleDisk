# PHY inti and control
## Clock source
由lmk04808的OSCout0 Bypass晶振的125M，时钟无法更改
## SGMII Bridge from ZYNQ to PHY
![image](https://user-images.githubusercontent.com/52576519/152683082-84355eeb-f462-47a0-8b92-457b7d20bfc3.png)
![image](https://user-images.githubusercontent.com/52576519/152683094-a080bf02-9b07-4375-adab-6e9b29c1f41c.png)
![image](https://user-images.githubusercontent.com/52576519/152683101-9a0f6ff4-8449-4ed1-a161-1c641ea874c3.png)
![image](https://user-images.githubusercontent.com/52576519/152683106-f66fb94c-0b3d-41ba-92cd-48bfaa84f23d.png)
![image](https://user-images.githubusercontent.com/52576519/152683126-0dbed384-0652-4a19-9e94-5b1a17b70954.png)
将 signal_detect连接到1

# CODE 
BCM84860使用的是C45协议，但xilinx默认使用C22协议，且没在代码中支持该选项，需手动添加，主要代码为XEmacPs_PhyWrite_C45和XEmacPs_PhyRead_C45，流程为先复位，再加载固件，最后进行一些配置
## 使用方法
在sdk的lwip中引入.h和.c文件，在main中执行bcm_cfg()函数（需要修改ps的默认网卡速率，将返回值由10改为1000）
