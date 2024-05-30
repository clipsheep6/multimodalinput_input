# 添加设备 删除设备 接口开发
> 入参使用 InputDevice 是否OK，这样可以节省很多IPC时候序列化 反序列化的代码流程
> 但是里边的大部分关键信息是不能传入的, 因为 deviceId 必须要由多模服务端去产生，保证唯一
> 是否能够复用 EnableInputDevice 接口进行使能和去使能 ---不需要，这个接口将要被下掉
> device_status 仓中获取本端设备的逻辑需要重新搞一下, 保证获取到的外设信息足以支撑跨端去创建一个外设信息
> 穿入 AddVirtualInputDevice
> 穿出 RemoveVirtualInputDevice
> 穿越管理维护其他外设的信息更为合理，尽量与多模解耦
>
## 待办项
> 虚拟设备 id 生成算法需要优化，不能直接自增 +1 