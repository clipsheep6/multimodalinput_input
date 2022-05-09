/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DISPLAY_INFO_H
#define DISPLAY_INFO_H

#include <vector>
#include <string>

namespace OHOS {
namespace MMI {
enum Direction {
    /**
     * 屏幕顺时针旋转了0度
     *
     * @since 9
    */
    Direction0,

    /**
     * 屏幕顺时针旋转了90度
     *
     * @since 9
    */
    Direction90,

    /**
     * 屏幕顺时针旋转了180度
     *
     * @since 9
    */
    Direction180,

    /**
     * 屏幕顺时针旋转了270度
     *
     * @since 9
    */
    
    Direction270
};

struct WindowInfo {
    /**
     * 表示窗口不可触摸
     *
     * @since 9
    */
    static constexpr uint32_t FLAG_BIT_UNTOUCHABLE = 1;

    /**
     * 窗口的全局唯一标识符
     *
     * @since 9
    */
    int32_t id;

    /**
     * 窗口所在的进程id
     *
     * @since 9
    */
    int32_t pid;

    /**
     * 窗口所在进程的uid
     *
     * @since 9
    */
    int32_t uid;

    /**
     * 逻辑屏中热区窗口左上角的x坐标
     *
     * @since 9
    */
    int32_t hotZoneTopLeftX;

    /**
     * 逻辑屏中热区窗口左上角的y坐标
     *
     * @since 9
    */
    int32_t hotZoneTopLeftY;

    /**
     * 热区窗口的逻辑宽度
     *
     * @since 9
    */
    int32_t hotZoneWidth;

    /**
     * 热区窗口的逻辑高度
     *
     * @since 9
    */
    int32_t hotZoneHeight;

    /**
     * 逻辑屏id
     *
     * @since 9
    */
    int32_t displayId;

    /**
     * 代理窗口id
     *
     * @since 9
    */
    int32_t agentWindowId;

    /**
     * 逻辑显示窗口左上角的x坐标
     *
     * @since 9
    */
    int32_t winTopLeftX;

    /**
     * 逻辑显示窗口左上角的y坐标
     *
     * @since 9
    */
    int32_t winTopLeftY;

    /**
     * 窗口当前状态
     *
     * @since 9
    */
    uint32_t flags;
};

struct PhysicalDisplayInfo {
    /**
     * 物理屏幕的全局唯一id
     *
     * @since 9
    */
    int32_t id;

    /**
     * 左侧物理屏id
     *
     * @since 9
    */
    int32_t leftDisplayId;

    /**
     * 上层物理视图的全局唯一id
     *
     * @since 9
    */
    int32_t upDisplayId;

    /**
     * 屏幕左上角的x坐标
     *
     * @since 9
    */
    int32_t topLeftX;

    /**
     * 屏幕左上角的y坐标
     *
     * @since 9
    */
    int32_t topLeftY;

    /**
     * 屏幕宽度
     *
     * @since 9
    */
    int32_t width;

    /**
     * 屏幕高度
     *
     * @since 9
    */
    int32_t height;

    /**
     * 显示名称，用于调试
     *
     * @since 9
    */
    std::string name;

    /**
     * 显示座位号，与触摸屏相关联的屏幕必须配置非空座位号
     *
     * @since 9
    */
    std::string seatId;

    std::string seatName;

    /**
     * 显示逻辑宽度
     *
     * @since 9
    */
    int32_t logicWidth;

    /**
     * 显示逻辑高度
     *
     * @since 9
    */
    int32_t logicHeight;

    /**
     * 显示方向
     *
     * @since 9
    */
    Direction direction;
};

struct LogicalDisplayInfo {
    /**
     * 逻辑显示的全局唯一id
     *
     * @since 9
    */
    int32_t id;

    /**
     * 逻辑显示左上角的x坐标
     *
     * @since 9
    */
    int32_t topLeftX;

    /**
     * 逻辑显示左上角的y坐标
     *
     * @since 9
    */
    int32_t topLeftY;

    /**
     * 逻辑显示宽度
     *
     * @since 9
    */
    int32_t width;

    /**
     * 逻辑显示高度
     *
     * @since 9
    */
    int32_t height;

    /**
     * 逻辑屏幕名称，用于调试
     *
     * @since 9
    */
    std::string name;

    /**
     * 逻辑显示seatId，不独立于触摸屏的显示使用此属性与输入关联
     *
     * 保持它为空，除非你确定你在做什么
     *
     * @since 9
    */
    std::string seatId;

    std::string seatName;

    int32_t focusWindowId;

    /**
     * 窗口信息按Z顺序排列的列表，顶部窗口在顶部
     *
     * @since 9
    */
    std::vector<WindowInfo> windowsInfo;
};
} // namespace MMI
} // namespace OHOS
#endif // DISPLAY_INFO_H