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

# hdf摇杆设备热插
hosmmi-event-injection hdi hot add joyStick
sleep 2 
# hdf手柄设备热插       
hosmmi-event-injection hdi hot add gamePad
sleep 2
# hdf鼠标热插       
hosmmi-event-injection hdi hot add mouse
sleep 2
# hdf键盘热插            
hosmmi-event-injection hdi hot add keyboard
sleep 2
# hdf触屏热插        
hosmmi-event-injection hdi hot add touch
sleep 2
# hdf trackPad热插            
hosmmi-event-injection hdi hot add trackPad
sleep 2
# hdf pad热插        
hosmmi-event-injection hdi hot add pad
sleep 2
# hdf pen热插              
hosmmi-event-injection hdi hot add pen
sleep 2
# hdf finger热插              
hosmmi-event-injection hdi hot add finger
sleep 2
# hdf 旋钮热插           
hosmmi-event-injection hdi hot add knob
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
kill -9 ${arr_server[1]}

# 关闭hap进程
kill -9 ${arr_hap[1]}

# 关闭虚拟驱动进程
for i in $(seq 1 10 ${#arr_vir[*]})
do
    kill -9 ${arr_vir[$i]}
done
# hdf摇杆设备热拔
hosmmi-event-injection hdi hot remove joyStick 
# hdf手柄设备热拔       
hosmmi-event-injection hdi hot remove gamePad
# hdf鼠标热拔      
hosmmi-event-injection hdi hot remove mouse
# hdf键盘热拔            
hosmmi-event-injection hdi hot remove keyboard
# hdf触屏热拔        
hosmmi-event-injection hdi hot remove touch
# df trackPad热拔            
hosmmi-event-injection hdi hot remove trackPad
# hdf pad热拔        
hosmmi-event-injection hdi hot remove pad
# hdf pen热拔              
hosmmi-event-injection hdi hot remove pen
# hdf finger热拔              
hosmmi-event-injection hdi hot remove finger
# hdf 旋钮热拔           
hosmmi-event-injection hdi hot remove knob
sleep 2
rm -rf /data/temp/symbol
rm -rf /data/mmi/*.skd
rm -rf /data/mmi/hosmmi-event-injection*