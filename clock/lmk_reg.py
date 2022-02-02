f = open("/mnt/c/Users/L/Desktop/HexRegisterValues.txt","r")   #设置文件对象
for j in range(26):
    line = f.readline()
    line = line[:-1]
    for i in range(len(line)):
        if line[i:i+2]=="0x":
            print(line[i:],",")
f.close() #关闭文件
