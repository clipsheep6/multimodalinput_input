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
#include "multimodal_event_handler.h"
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <thread>
#include <algorithm>

#include "getopt.h"
//#include "ipc_skeleton.h"
//#include "iservice_registry.h"
//#include "mmi_log.h"
#include "string_ex.h"
#include "pointer_event.h"
#include "input_manager.h"
//#include "system_ability_definition.h"

class InputManagerCommand {
public:
    int32_t ParseCommand(int argc, char *argv[]);
    int32_t ConnectService();
    void ShowUsage();
private:
    void InitializeMouseDeathStub();
    // sptr<IMultimodalInputService> service_ { nullptr };
    // sptr<IMouseDeathListener> listenerStub_ { nullptr };
};


struct KeyEventI{
    int32_t mouseMove[2];
    int32_t mouseOperate;
    int32_t keyboardOperate;
    int32_t keyboardCombination[3];
    int32_t touchMove[4];
    int32_t touchOperate[2];
};

namespace OHOS {
namespace {
constexpr int32_t SLEEPTIME = 20;
constexpr int32_t ONE_MORE_COMMAND = 1;
constexpr int32_t THREE_MORE_COMMAND = 3;
}

int32_t InputManagerCommand::ParseCommand(int argc, char *argv[])
{
    struct option headOptions[] = {
        {"mouse", no_argument, NULL, 'M'},
        {"keyboard", no_argument, NULL, 'K'},
        {"touch", no_argument, NULL, 'T'},
        {"help", no_argument, NULL, '?'},
        {NULL, 0, NULL, 0}
    };
    struct option mouseSensorOptions[] = {    //  mouse -move 4 5  增加滚轮滚动 正负值来区分滚动方向 scroll
        {"move", required_argument, NULL, 'm'},
        {"click", required_argument, NULL, 'c'}, 
        {"down", required_argument, NULL, 'd'},
        {"up", required_argument, NULL, 'u'},
        {"scroll", required_argument, NULL, 's'},
        {NULL, 0, NULL, 0}
    };
    struct option keyboardSensorOptions[] = {   // 组合键 keyboard -d x -d x -u x -u x 按下两个抬起两个 按下的最后都要up
        {"down", required_argument, NULL, 'd'},
        {"up", required_argument, NULL, 'u'},      
        {NULL, 0, NULL, 0}
    };
    struct option touchSensorOptions[] = {
        {"move", required_argument, NULL, 'm'},
        {"down", required_argument, NULL, 'd'},
        {"up", required_argument, NULL, 'u'},
        {NULL, 0, NULL, 0}
    };
    int32_t c;
    int32_t optionIndex;
    optind = 0;
    KeyEventI *keyEventI = new KeyEventI{};
    
    /* parse the first word of the command */    // extern char *optarg; 表示参数的具体内容 extern int optind; 表下一个将被处理到的参数在 argv 中的下标值
    if ((c = getopt_long(argc, argv, "MKT?", headOptions, &optionIndex)) != -1) { //对于前两个参数argc和grgv[]，一般直接使用main()传递进来的数值
        //InputManagerCommand command;                  //用于规定合法选项(option)以及选项是否带参数(argument)。合法选项字母构成的字符串，如果字母后面带上冒号:就说明该选项必须有参数
        switch (c) {
            case 'M': {
                int32_t px, py, buttonId, scrollValue, delayTime; 
                /* parse commands for virtual mouse */
                while ((c = getopt_long(argc, argv, "m:d:u:c:s:", mouseSensorOptions, &optionIndex)) != -1) {
                    switch (c) {
                        case 'm': {
                            if (optind >= argc) {
                                std::cout << "argc is ," << argc << std::endl;
                                std::cout << "optind is ," << optind << std::endl;
                                std::cout << "optarg is ," << optarg << std::endl;
                                std::cout << "too few args" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            if (!StrToInt(optarg, px) || !StrToInt(argv[optind], py)) {  //指向当前选项参数的指针
                                std::cout << "invalid command to move mouse" << std::endl;
                                return -1;
                            }
                            std::cout << "move to " << px << " " << py << std::endl; // 构造新的keyEvent
                            if (argv[optind + 1] != nullptr) {
                                if (!StrToInt(argv[optind + 1], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->mouseMove[0] = px;
                            keyEventI->mouseMove[1] = py;

                            std::cout << "tools 138" << std::endl;
                            auto pointerEvent = MMI::PointerEvent::Create();
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(1);
                            item.SetGlobalX(px);
                            item.SetGlobalY(py);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetPointerId(1);
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
                            
                            std::cout << "tools 147" << std::endl;
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

                            std::cout << "tools 149" << std::endl;
                            //SimulateInput(keyEventI); // 客户端新增的注入接口
                            optind = optind + ONE_MORE_COMMAND;//移动有两个值，optarg占用一个值 optind占用一个值，加1才能指向下一个选项
                            break;
                        }
                        case 'd': {
                            if (!StrToInt(optarg, buttonId)) {
                                std::cout << "invalid button press command" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            if (buttonId >2) {
                               std::cout << "invalid button press command" << std::endl;
                                ShowUsage();
                                return -1; 
                            }
                            //  SOURCE_TYPE_MOUSE = 1; 鼠标源事件

                            std::cout << "press down 160 " << buttonId << std::endl;
                            auto pointerEvent = MMI::PointerEvent::Create();
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(1);
                            item.SetPressed(true);// anxia shi true
                            pointerEvent->SetPointerId(1);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetButtonId(buttonId); //当前事件按下的那个键        // 按下按键时用的是哪一个设置按下的键
                            pointerEvent->SetButtonPressed(buttonId);    // 当前事件下有哪些键按下（总共按下的键）  邵子云
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

                            std::cout << "tools 173" << std::endl;
                            if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->mouseOperate = buttonId;
                            //SimulateInput(keyEventI); // 客户端新增的注入接口
                            break;
                        }
                        case 'u': {
                            if (!StrToInt(optarg, buttonId)) {
                                std::cout << "invalid raise button command" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            if (buttonId >2) {
                               std::cout << "invalid button press command" << std::endl;
                                ShowUsage();
                                return -1; 
                            }
                            std::cout << "lift up button " << buttonId << std::endl;

                            auto pointerEvent = MMI::PointerEvent::Create();
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(1);
                            item.SetPressed(false);
                            pointerEvent->SetPointerId(1);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetButtonPressed(buttonId);
                            pointerEvent->SetButtonId(buttonId);
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_BUTTON_UP);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
                            std::cout << "tools 204" << std::endl;
                            if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->mouseOperate = buttonId;
                            //SimulateInput(keyEventI); 
                            break;
                        }
                        case 's': {
                            if (!StrToInt(optarg, scrollValue)) {
                                std::cout << "invalid  scroll button command" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            std::cout << "scroll wheel " << scrollValue << std::endl;
                            if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->mouseOperate = scrollValue;

                            auto pointerEvent = MMI::PointerEvent::Create();
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(1);
                            item.SetPressed(false);
                            pointerEvent->SetPointerId(1);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetButtonPressed(buttonId);
                            pointerEvent->SetButtonId(buttonId);

                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE);
                            pointerEvent->SetAxis(MMI::PointerEvent::AXIS_TYPE_SCROLL_VERTICAL);
                            pointerEvent->SetAxisValue(scrollValue);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_MOUSE);
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);

                            //SimulateInput(keyEventI); 
                            break;
                        }
                        case 'c': {
                            if (!StrToInt(optarg, buttonId)) {
                                std::cout << "invalid click button command" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            if (buttonId >2) {
                               std::cout << "invalid button press command" << std::endl;
                                ShowUsage();
                                return -1; 
                            }
                            std::cout << "click   " << buttonId << std::endl;
                            if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->mouseOperate = buttonId;
                            //SimulateInput(keyEventI); 
                            break;
                        }
                        default: {
                            std::cout << "invalid command to virtual mouse" << std::endl;
                            ShowUsage();
                            return -1;
                        }
                    }
                }
                break;
            }
            /* parse commands for keyboard */
            case 'K': {
                std::vector<int32_t> downKey;
                // std::vector<int32_t> upKey;
                int32_t keyID, delayTime, isCombinationKey = 0; //delayTime = 1000
                while ((c = getopt_long(argc, argv, "d:u:", keyboardSensorOptions, &optionIndex)) != -1) {
                    switch (c) {
                        case 'd': {
                            if (!StrToInt(optarg, keyID)) {
                                std::cout << "invalid command to down key" << std::endl;
                            }
                           if (optind == isCombinationKey + 2) {
                                
                                keyEventI->keyboardCombination[1] = keyID;
                                downKey.push_back(keyID);
                                std::cout << "You pressed the key combination " <<keyEventI->keyboardCombination[0]<<" + "<< keyEventI->keyboardCombination[1]<<std::endl;

                                if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                    delayTime = 0 ;
                                    std::cout << "No delay operation is performed" << std::endl;
                                   } else {
                                    std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                    }
                                }
                                //SimulateInput(keyEventI);
                                break;
                            }
                            std::cout << "You pressed the key " << keyID << std::endl;

                            if (argv[optind] != nullptr) {
                                if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }

                            keyEventI->keyboardCombination[0] = keyID;
                            downKey.push_back(keyID);
                            for (int32_t i = 0; i<downKey.size(); i++) {
                                if (keyID != downKey[i]) {
                                    std::cout << "downKey value is" << downKey[i] << std::endl;
                                }
                            }
                            isCombinationKey = optind;
                            //SimulateInput(keyEventI);
                            break;
                        }
                        case 'u': {

                            if (!StrToInt(optarg, keyID)) {
                                std::cout << "217 row invalid button press command" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            std::vector<int32_t>::iterator iter;
                            iter = std::find(downKey.begin(), downKey.end(), keyID);
                            if(iter != downKey.end()) {
                                keyEventI->keyboardOperate = keyID;
                                std::cout << "You raised the key " << keyID << std::endl;
                                
                                if (argv[optind] != nullptr) {
                                    if (!StrToInt(argv[optind], delayTime)) {  //可选参数延时操作
                                    delayTime = 0 ;
                                    std::cout << "No delay operation is performed" << std::endl;
                                    }else {
                                    std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                    }
                                }

                                //SimulateInput(keyEventI);
                                iter = downKey.erase(iter);
                                break;
                            }else {
                                std::cout << "Please press the " << keyID << " key first "<< std::endl;
                                return -1;
                            }
                            // for(iter=downKey.begin();iter!=downKey.end();++iter) {
                            //     if (*iter == keyID) {
                            //         keyEventI->keyboardOperate = keyID;
                            //         std::cout << "You raised the key " << keyID << std::endl;
                            //         //SimulateInput(keyEventI);
                            //         iter = downKey.erase(iter);
                            //         break;
                            //     }
                            // }                           
                            // break;
                        }
                        default: {
                            std::cout << "invalid command to keyboard key" << std::endl;
                            ShowUsage();
                            return -1;
                        }
                    }
                }
                for (int32_t i = 0; i < downKey.size(); i++) {
                    std::cout << "you have a key " << downKey[i]<<" not release"<< std::endl;                
                }
                break;
            }
            /* parse commands for touch */
            case 'T': {
                int32_t px1, py1, px2, py2, delayTime;      
                while ((c = getopt_long(argc, argv, "m:d:u:", touchSensorOptions, &optionIndex)) != -1) {
                    switch (c) {
                        case 'm': {
                            if (optind +2 >= argc) {
                                std::cout << " 234 row too few args" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            if (!StrToInt(optarg, px1) || !StrToInt(argv[optind], py1) || !StrToInt(argv[optind + 1], px2) || !StrToInt(argv[optind + 2], py2)) {
                                std::cout << "invalid command to input value" << std::endl;
                                return -1;
                            }
                            std::cout <<"move from location " << px1 << " " << py1 << " to "<< px2 << " " << py2 << std::endl;

                            if (argv[optind + 3] != nullptr) {
                                if (!StrToInt(argv[optind + 3], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            // SOURCE_TYPE_TOUCHSCREEN = 2;触摸屏
                            keyEventI->touchMove[0] = px1;
                            keyEventI->touchMove[1] = py1;
                            keyEventI->touchMove[2] = px2;
                            keyEventI->touchMove[3] = py2; // 注意 可能数组中在这儿给touchOperate[3]赋值，单在down时没有清除，将此值传入到down接口中
                            //SimulateInput(keyEventI);

                            auto pointerEvent = MMI::PointerEvent::Create();
                            std::cout << "tools 166" <<pointerEvent<<std::endl;
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(0);
                            item.SetGlobalX(px1); // 目标位置，只有一组目标值没有起始位置的值
                            item.SetGlobalY(py1);
                            pointerEvent->SetPointerId(0);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_MOVE);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
                            // pointerEvent->SetButtonId(buttonId);
                            std::cout << "tools 170" << std::endl;
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
                            std::cout << "tools 172" << std::endl;
                            optind =  optind + THREE_MORE_COMMAND;
                            break;
                        }
                        case 'd': {
                            if (optind >= argc) {
                                std::cout << "255 row too few args" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            std::cout << "-T -d argc is," << argc << std::endl;
                            std::cout << "-T -d optind is," << optind << std::endl;
                            if (!StrToInt(optarg, px1) || !StrToInt(argv[optind], py1)) {
                                std::cout << "262 row invalid command to input value" << std::endl;
                                return -1;
                            }
                            std::cout << "touch down " << px1 << " " << py1 << std::endl;
                            if (argv[optind +1] != nullptr) {
                                if (!StrToInt(argv[optind + 1], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            auto pointerEvent = MMI::PointerEvent::Create();
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(0);
                            item.SetGlobalX(px1);
                            item.SetGlobalY(py1);
                            pointerEvent->SetPointerId(0);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_DOWN);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
                            // pointerEvent->SetButtonId(buttonId);
                            std::cout << "tools 170" << std::endl;
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
                            keyEventI->touchOperate[0] = px1;
                            keyEventI->touchOperate[1] = py1;
                            //SimulateInput(keyEventI);
                            optind = optind + ONE_MORE_COMMAND;
                            break;
                        }
                        case 'u': {
                            if (optind >= argc) {
                                std::cout << "271 row too few args" << std::endl;
                                ShowUsage();
                                return -1;
                            }
                            std::cout << "-T -u argc is," << argc << std::endl;
                            std::cout << "-T -u optind is," << optind << std::endl;
                            if (!StrToInt(optarg, px1) || !StrToInt(argv[optind], py1)) {
                                std::cout << "invalid command to input value" << std::endl;
                                return -1;
                            }
                            std::cout << "touch up " << px1 << " " << py1 << std::endl;

                            if (argv[optind +1] != nullptr) {
                                if (!StrToInt(argv[optind + 1], delayTime)) {  //可选参数延时操作
                                delayTime = 0 ;
                                std::cout << "No delay operation is performed" << std::endl;
                                }else {
                                std::cout << "Delay "<<delayTime<<" time to execute the next one instruction" << std::endl;
                                }
                            }
                            keyEventI->touchOperate[0] = px1;
                            keyEventI->touchOperate[1] = py1;

                            auto pointerEvent = MMI::PointerEvent::Create();
                            std::cout << "tools 166" <<pointerEvent<<std::endl;
                            MMI::PointerEvent::PointerItem item;
                            item.SetPointerId(0);
                            item.SetGlobalX(px1);
                            item.SetGlobalY(py1);
                            pointerEvent->SetPointerId(0);
                            pointerEvent->AddPointerItem(item);
                            pointerEvent->SetPointerAction(MMI::PointerEvent::POINTER_ACTION_UP);
                            pointerEvent->SetSourceType(MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN);
                            // pointerEvent->SetButtonId(buttonId);
                            std::cout << "tools 170" << std::endl;
                            MMI::InputManager::GetInstance()->SimulateInputEvent(pointerEvent);
                            //SimulateInput(keyEventI);
                            optind =  optind + ONE_MORE_COMMAND;
                            break;   
                        }
                        default: {
                            std::cout << "invalid command to CV" << std::endl;
                            ShowUsage();
                            return -1;
                        }
                    }
                    /* sleep for a short time after every step to give the divice some time to react */
                    std::this_thread::sleep_for(std::chrono::milliseconds(SLEEPTIME));
                }
                break;
            }
            case '?': {
                ShowUsage();
                return ERR_OK;
            }
            default: {
                std::cout << "invalid command" << std::endl;
                ShowUsage();
                return -1;
            }
        }
    } else {
        std::cout << "too few args" << std::endl;
        ShowUsage();
        return -1;
    }
    return ERR_OK;
}


void InputManagerCommand::ShowUsage()
{
    std::cout << "Usage: injectevent <option> <command> <arg>..." << std::endl;                          // injectevent -mouse -m 7 8
    std::cout << "The option are:                                " << std::endl;
    std::cout << "-M   --mouse                                                  " << std::endl;
    std::cout << "  commands for  mouse:                            " << std::endl;
    std::cout << "  -m <dx> <dy>                    --move   <dx> <dy>               -move to relative position (dx,dy) " << std::endl;
    std::cout << "  -d <key>                        --down   key                     -press down a button,        " << std::endl;
    std::cout << "                                                                    0 is the left button,1 is the middle," << std::endl;
    std::cout << "                                                                    2 is the right" << std::endl;
    std::cout << "  -u <key>                        --up     <key>                   -release a button " << std::endl;
    std::cout << "  -l <key>                        --click  <key>                   -press the left button down,then raise" << std::endl;
    std::cout << "  -s <key>                        --scroll <key>                   -Positive values are sliding backwards "<<std::endl;
    std::cout << "                                                                    negative values are sliding forwards" << std::endl;

    std::cout << "-K   --keyboard                                                  " << std::endl;
    std::cout << "  commands for  keyboard:                                        " << std::endl;
    std::cout << "  -d <key>                        --down   <key>                   -press down a key combination," << std::endl;
    std::cout << "                                                                    1-14 is the number key,15-52 is the letterkey," << std::endl;
    std::cout << "                                                                    53-66 is the Fx, 67-88 is other" << std::endl;
    std::cout << "  -u <key>                        --up     <key>                   -release a key " << std::endl;
    

    std::cout << "-T   --touch                                                      " << std::endl;
    std::cout << "  commands for  touch:                                            " << std::endl;
    std::cout << "  -d <dx1> <dy1>                  --down   <dx1> <dy1>             -press down a position  dx1 dy1, " << std::endl;
    std::cout << "  -u <dx1> <dy1>                  --up     <dx1> <dy1>             -release a position dx1 dy1" << std::endl;
    std::cout << "  -m <dx1> <dy1> <dx2> <dy2>      --move   <dx1> <dy1> <dx2> <dy2> -move from position dx1 dy1 to dx2 dy2 " << std::endl;
   
    std::cout << "                                                                  " << std::endl;
    std::cout << "-?  --help                                                        " << std::endl;
}
} // namespace OHOS