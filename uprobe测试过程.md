# 实验过程
## 树莓派perf安装 

针对5.15内核

使用apt直接下载的5.10版本不兼容，需要源码安装

源码下载地址：[https://mirror.bjtu.edu.cn/kernel/linux/kernel/tools/perf/v5.15.0/perf-5.15.0.tar.gz](https://mirror.bjtu.edu.cn/kernel/linux/kernel/tools/perf/v5.15.0/perf-5.15.0.tar.gz)

编译环境和依赖参考：[https://zhuanlan.zhihu.com/p/446319798](https://zhuanlan.zhihu.com/p/446319798)

```javascrip
tar xzvf linux-5.15.0.tar.gz
cd perf-5.15.0/tools/perf
make -j4 && make install 
cp ./perf /usr/bin
```

## 内核配置
开启以下内核选项
```javascrip
CONFIG_UPROBE_EVENTS=y
CONFIG_MODULES=y
CONFIG_KALLSYMS=y
CONFIG_HIGH_RES_TIMERS=y
CONFIG_TRACEPOINTS=y
```
开启后可在```/sys/kernel/debug/tracing```目录下找到```uprobe_events```和```uprobe_profile```

## debug编译
分别重新编译发布订阅程序
```javascrip
#build.sh
mkdir build 
cd build
cmake .. -DCMAKE_PREFIX_PATH=~/Fast-DDS/install/ -DCMAKE_BUILD_TYPE=Debug
make
```

## 创建探针

在超级用户权限下分别在```SubDemo```和```PubDemo```目录运行以下两个脚本
```javascript
#pubprobe.sh
#SHMPub_main.cpp
#pub init
perf probe -x ./build/SHMPub pubinit_151=SHMPub_main.cpp:151
#main 调用publish
perf probe -x ./build/SHMPub tfunc_114=SHMPub_main.cpp:114

#SHMPubPublisher.cpp
#pub matched
perf probe -x ./build/SHMPub pubmatched_124=SHMPubPublisher.cpp:124
#pub unmatched
#perf probe -x ./build/SHMPub pubunmatched_130=SHMPubPublisher.cpp:130
#pub connection
perf probe -x ./build/SHMPub publish_144=SHMPubPublisher.cpp:144
#pub time
perf probe -x ./build/SHMPub pubtime_147=SHMPubPublisher.cpp:147
#pub write
perf probe -x ./build/SHMPub write_150=SHMPubPublisher.cpp:150

perf probe -x ./build/SHMPub pub_147_ind=SHMPubPublisher.cpp:147 pubind
```
```javascript
#subprobe.sh
#SHMSub_main.cpp
#sub init
perf probe -x ./build/SHMSub subinit_66=SHMSub_main.cpp:66
#sub run
perf probe -x ./build/SHMSub subrun_68=SHMSub_main.cpp:68

#SHMSubSubscriber.cpp
#sub matched
perf probe -x ./build/SHMSub submatched_122=SHMSubSubscriber.cpp:122
#sub unmatched
#perf probe -x ./build/SHMSub subunmatched_126=SHMSubSubscriber.cpp:126
#sub listen
perf probe -x ./build/SHMSub listen_136=SHMSubSubscriber.cpp:136
#sub read 
perf probe -x ./build/SHMSub read_141=SHMSubSubscriber.cpp:141
#sub infoalive
perf probe -x ./build/SHMSub infoalive_144=SHMSubSubscriber.cpp:144
#sub time
perf probe -x ./build/SHMSub subtime_147=SHMSubSubscriber.cpp:147
#latency
#perf probe -x ./build/SHMSub latency_153=SHMSubSubscriber.cpp:153

perf probe -x ./build/SHMSub latency_147_ind=SHMSubSubscriber.cpp:147 subind
```

## 开启追踪
在超级用户权限下运行以下脚本
```javascript
#uprobetraceon.sh
cd /sys/kernel/debug/tracing/
echo 1 > events/probe_SHMPub/enable 
echo 1 > events/probe_SHMSub/enable
echo 0 > tracing_on 
echo > trace
echo 1 > tracing_on 
```

## 测试步骤
开启两个终端，分别对应发布和订阅程序
分别在超级用户权限下运行以下脚本
```javascript
root@raspberrypi:/home/pi/demo/SHMdemo/PubDemo# ./pubrun.sh
root@raspberrypi:/home/pi/demo/SHMdemo/SubDemo# ./subrun.sh /home/pi/out/out.txt
```
```javascript
#pubrun.sh
export LD_LIBRARY_PATH=/home/pi/Fast-DDS/install/lib
./build/SHMPub 
```
```javascript
#subrun.sh
export LD_LIBRARY_PATH=/home/pi/Fast-DDS/install/lib
./build/SHMSub $1
```

## 获取结果
在超级用户权限下运行以下脚本
```javascript
#uprobetraceoff.sh
cd /sys/kernel/debug/tracing/
echo 0 > events/probe_SHMPub/enable 
echo 0 > events/probe_SHMSub/enable
echo 0 > tracing_on 
cat trace > $1
```