#!/bin/bash

# 权限赋予
mount -o rw,remount /
mkdir -p /data/mmi
chmod 777 /data/mmi
chown u0_a1:u0_a1 /data/mmi
chmod 777 /system/etc/mmi_device_config.ini
chmod 777 /data/accounts/account_0/applications/com.example.jsdemoUT/com.example.jsdemoUT/assets/entry/resources/base/profile/*
mkdir /data/temp/
chmod 777 /data/temp/
mkdir /data/temp/symbol/
chmod 777 /data/temp/symbol/
rm -rf /data/temp/symbol/*

# 启动鼠标
hosmmi-virtual-device-manager start mouse &
sleep 2

#启动键盘
hosmmi-virtual-device-manager start keyboard &
sleep 2

#启动手柄
hosmmi-virtual-device-manager start gamepad &
sleep 2

#启动轨迹球
hosmmi-virtual-device-manager start trackball &
sleep 2

#启动遥控器
hosmmi-virtual-device-manager start remotecontrol &
sleep 2

#启动trackpad
hosmmi-virtual-device-manager start trackpad &
sleep 2

#启动旋钮
hosmmi-virtual-device-manager start knob &
sleep 2

#启动摇杆
hosmmi-virtual-device-manager start joystick &
sleep 2

#启动触摸板
hosmmi-virtual-device-manager start touchpad &
sleep 2

#启动触屏
hosmmi-virtual-device-manager start touchscreen &
sleep 2

# 启动hap包
aa start -d l -a com.example.jsdemoUT.MainAbility -b com.example.jsdemoUT > /proc/sys/kernel/printk &
sleep 3
# 监控event.ini、 event_s.ini是否存在，若不存在，则等待
traditionalEvent="/data/mmi/traditionalEvent.ini";
aiVoiceEvent="/data/mmi/aiVoiceEvent.ini";
jsonEventFile="/data/accounts/account_0/applications/com.example.jsdemoUT/com.example.jsdemoUT/assets/entry/resources/base/profile/";
while [ ! -f "$traditionalEvent" ]
do
sleep 0.1
echo "hap is starting"
done
echo "hap started"

# traditionalEvent.ini若不存在或文件存在但内容不为空，进入循环
while [ ! -f "$traditionalEvent" ] || [[ `cat $traditionalEvent` != "" ]] || [ ! -f "$aiVoiceEvent" ] || [[ `cat $aiVoiceEvent` != "" ]] 
do
# 如果event.ini、event_s.ini文件存在，则执行如下操作
if [ -f "$traditionalEvent" ] && [ ! -f "$aiVoiceEvent" ]
then
    # 传统设备虚拟注入
    arr_traditional=`cat $traditionalEvent`
    if [ $arr_traditional == "start" ]
    then
        echo arr_traditional:${arr_traditional}
        rm -rf $traditionalEvent
        rm -rf /data/mmi/*.skd
        rm -rf /data/mmi/hosmmi-event-injection*
        continue
    elif [ $arr_traditional == "end" ]
    then
        echo arr_traditional:${arr_traditional}
        rm -rf $traditionalEvent
        rm -rf /data/mmi/*.skd
        rm -rf /data/mmi/hosmmi-event-injection*
        break
    else
        hosmmi-event-injection json $jsonEventFile$arr_traditional
        echo arr_traditional:${arr_traditional}
        rm -rf $traditionalEvent
        rm -rf /data/mmi/*.skd
        rm -rf /data/mmi/hosmmi-event-injection*
    fi
elif [ -f "$aiVoiceEvent" ] && [ ! -f "$traditionalEvent" ]
then
    # AI语音虚拟注入
    echo "ai"
    arr_ai=`cat $aiVoiceEvent`
    hosmmi-event-injection aisensor-each ${arr_ai[0]} ${arr_ai[1]}
    echo arr_ai:${arr_ai[0]}
    echo line_ai:${arr_ai[1]}
    # 删除aiVoiceEvent.ini文件
    rm -rf /data/mmi/*.skd
    rm -rf /data/mmi/hosmmi-event-injection*
    rm -rf $aiVoiceEvent
elif [ ! -f "$traditionalEvent" ] && [ ! -f "$traditionalEvent" ]
then
    sleep 0.1
else
    rm -rf $traditionalEvent
    rm -rf $aiVoiceEvent
fi
done

# 删除deviceNode.ini、traditionalEvent.ini、aiVoiceEvent.ini、touchEvent.ini文件
rm -rf $traditionalEvent
rm -rf $aiVoiceEvent
# 获取hap进程
ps_hap=`ps -ef | grep 'com.example.jsdemoUT' | grep -v grep`
arr_hap=($ps_hap)
# 获取虚拟驱动进程
ps_vir=`ps -ef | grep 'hosmmi-virtual-device-manager' | grep -v grep`
arr_vir=($ps_vir)

# 关闭server进程
# kill -9 ${arr_server[1]}

# 关闭hap进程
kill -9 ${arr_hap[1]}

# 关闭虚拟驱动进程
for i in $(seq 1 10 ${#arr_vir[*]})
do
    kill -9 ${arr_vir[$i]}
done
rm -rf /data/temp/symbol
rm -rf /data/mmi/*.skd
rm -rf /data/mmi/hosmmi-event-injection*