# 实验过程

## 内核配置
```javascript
CONFIG_NO_HZ_FULL=y
CONFIG_CPUSETS=y
CONFIG_TRACING=y
```

## 引导参数配置
```javascript
//修改/boot/cmdline.txt ，在文件最后添加如下配置
nohz_full=3
```
重启生效

## 配置cpuset

在超级用户权限下运行以下两个脚本
```javascript
#initcpuset.sh
echo -1 > /proc/sys/kernel/sched_rt_runtime_us
mkdir /sys/fs/cgroup/cpuset
mount -t cgroup -ocpuset cpuset /sys/fs/cgroup/cpuset
cd /sys/fs/cgroup/cpuset
mkdir housekeeping
mkdir isolated
echo 0-2 >  housekeeping/cpuset.cpus
echo 0 > housekeeping/cpuset.mems
echo 3 > isolated/cpuset.cpus
echo 0 > isolated/cpuset.mems
echo 0 > cpuset.sched_load_balance 
echo 0 > isolated/cpuset.sched_load_balance
while read P
do
  echo $P > housekeeping/cgroup.procs
done < cgroup.procs
```
```javascript
#irqdisable.sh
# Migrate irqs to CPU 0-2 (exclude CPU 3)
for I in $(ls /proc/irq)
do
    if [[ -d "/proc/irq/$I" ]]
    then
        echo "Affining vector $I to CPUs 0-2"
        echo 0-2 > /proc/irq/$I/smp_affinity_list
    fi
done
```

## 在发布程序中开启cpuset
确保在SHMPub_main.cpp文件中包含以下头文件
```javascript
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
```
在main函数中，以下代码没有被注释
```javascript
    int fd = open("/sys/fs/cgroup/cpuset/isolated/cgroup.procs", O_WRONLY);
    if (fd < 0) {
        perror("Can't open cpuset file...\n");
        return 0;
    }
    write(fd, "0\n", 2);
    ……
    close(fd);
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