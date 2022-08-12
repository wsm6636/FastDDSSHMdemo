# fastdds 安装
## 环境配置
链接: [https://blog.csdn.net/u012739527/article/details/124687387?spm=1001.2014.3001.5502](https://blog.csdn.net/u012739527/article/details/124687387?spm=1001.2014.3001.5502)

其中Fast-DDS-Gen安装参考：
 [https://blog.csdn.net/u012739527/article/details/124731579?spm=1001.2014.3001.5502](https://blog.csdn.net/u012739527/article/details/124731579?spm=1001.2014.3001.5502)

**如果下面一步报错**
```javascript
gradle assemble 
```
卸载已有gradle，安装6.3版本

参考： [https://discuss.px4.io/t/installing-ros2-fast-rtps-gen-installation-error/17133](https://discuss.px4.io/t/installing-ros2-fast-rtps-gen-installation-error/17133)

安装步骤参考[https://gradle.org/releases/](https://gradle.org/releases/)

---
## pc和树莓派通信

pc：VMbox虚拟机，Ubuntu18

树莓派：4.19+preemptrt

前提：可以互相ping通，关闭pc机防火墙，vmbox桥连

分别安装fastdds，在同一网络下，分别运行各自的helloworldexample
在上位机publisher，下位机subscriber可以看到输出信息

**为什么不要配置ip等信息，只在同一局域网就可以？
若不在同一网络下，如何配置
xml文件如何使用**

---
## pc和树莓派传文件

demo：[https://github.com/wsm6636/fast-dds-demo](https://github.com/wsm6636/fast-dds-demo)

### base64编码解码，通过string::message传输
文件传输参考：[https://blog.csdn.net/u013749051/article/details/83308710?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-83308710-blog-104654660.pc_relevant_aa2&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-83308710-blog-104654660.pc_relevant_aa2&utm_relevant_index=1](https://blog.csdn.net/u013749051/article/details/83308710?spm=1001.2101.3001.6661.1&utm_medium=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-83308710-blog-104654660.pc_relevant_aa2&depth_1-utm_source=distribute.pc_relevant_t0.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-1-83308710-blog-104654660.pc_relevant_aa2&utm_relevant_index=1)

base64参考：[https://andyli.blog.csdn.net/article/details/79559097?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-79559097-blog-120388396.pc_relevant_multi_platform_whitelistv2&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-79559097-blog-120388396.pc_relevant_multi_platform_whitelistv2&utm_relevant_index=4](https://andyli.blog.csdn.net/article/details/79559097?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-79559097-blog-120388396.pc_relevant_multi_platform_whitelistv2&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-79559097-blog-120388396.pc_relevant_multi_platform_whitelistv2&utm_relevant_index=4)


**传输size受限问题：HelloWorld.cxx文件，更改getCdrSerializedSize数值**

### 接收文件后增加可执行权限
```javascript
//chmod函数
//头文件
#include <sys/types.h>
#include <sys/stat.h>
//返回值int，参数：路径，权限（读、写、执行）
chmod(subfilename, S_IREAD | S_IWRITE | S_IEXEC);
```
参考：[https://blog.csdn.net/FallingU/article/details/75222625?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-75222625-blog-117115930.pc_relevant_multi_platform_whitelistv1_exp2&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-75222625-blog-117115930.pc_relevant_multi_platform_whitelistv1_exp2&utm_relevant_index=4](https://blog.csdn.net/FallingU/article/details/75222625?spm=1001.2101.3001.6650.2&utm_medium=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-75222625-blog-117115930.pc_relevant_multi_platform_whitelistv1_exp2&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2%7Edefault%7ECTRLIST%7Edefault-2-75222625-blog-117115930.pc_relevant_multi_platform_whitelistv1_exp2&utm_relevant_index=4)

### 命令行传输参数(文件路径)

### 下位机接收文件后自动运行，并获取输出信息
```javascript
//system函数
//头文件
#include <cstdlib>
system(subfilename);

//运行并获取输出
#include <stdio.h>
#include <string.h>
popen(subfilename, "r")；
fgets(buf, sizeof(buf), fp)；
strcat(result, buf);
```

参考：[https://blog.csdn.net/CFH1021/article/details/104838342](https://blog.csdn.net/CFH1021/article/details/104838342) 

---
### 下一步：




并向上位机发送log

每个程序可以同时做publisher和subscriber



