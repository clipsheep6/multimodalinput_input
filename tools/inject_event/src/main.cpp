/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "input_manager_command.h"

int main(int argc, char** argv)
{
    // std::vector<std::string> keyEvent;
    // for (int i = 1; i < argc; i++) {
    //     keyEvent.push_back(std::string(argv[i]));
    //     /*如果是key      inject_event     keyboard KEY_DOWN shift DELAY 1000 KEY_DOWN A   键盘组合
    //                                     KEY_CLICK                                         键盘点击
    //         mouse       inject_event    mouse MOUSE_MOVE x y                              鼠标移动
    //                                     MOUSE_CLICK                                       鼠标左右键点击
    //                                     MOUSE_SCROLL                                      鼠标滚轮
    //         TOUCH       inject_event         TOUCH_DOWN                                   触摸屏点击
    //                                                                                       触摸屏双指
    //     */

    // }
    OHOS::InputManagerCommand command;
    return command.ParseCommand(argc, argv);
    
}