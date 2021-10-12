/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "injection_event_dispatch.h"
#include "util.h"
#include "proto.h"

using namespace std;
using namespace OHOS::MMI;

namespace {
    static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = { LOG_CORE, MMI_LOG_DOMAIN, "InjectionEventDispatch" };
}

void InjectionEventDispatch::Init()
{
    InitConnnectFunction();
    InitManageFunction();
    InitDeviceInfo();
}

void InjectionEventDispatch::InitManageFunction()
{
    InjectFunctionMap funs[] = {
        {"drive", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnDrive, this)},
        {"device", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnDevice, this)},
        {"touch", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnTouchEvent, this)},
        {"aisensor", SEND_EVENT_TO_HDI, false, bind(&InjectionEventDispatch::OnAisensor, this)},
        {"aisensor-all", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnAisensorAll, this)},
        {"aisensor-each", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnAisensorEach, this)},
        {"hdi", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnHdi, this)},
        {"hdi-hot", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnHdiHot, this)},
        {"hdi-event", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnHdiEvent, this)},
        {"hdi-status", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnHdiStatus, this)},
        {"dump", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnDump, this)},
        {"exception", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnException, this)},
        {"list", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnList, this)},
        {"knuckle-all", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnKnuckleAll, this)},
        {"knuckle-each", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnKnuckleEach, this)},
        {"--help", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnHelp, this)},
#ifdef OHOS_BUILD_HDF
        {"json", SEND_EVENT_TO_HDI, true, bind(&InjectionEventDispatch::OnJson, this)},
#else
        {"sendevent", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnSendEvent, this)},
        {"json", SEND_EVENT_TO_DEVICE, false, bind(&InjectionEventDispatch::OnJson, this)},
#endif
    };

    for (auto& it : funs) {
        CHKC(RegistInjectEvent(it), EVENT_REG_FAIL);
    }
}

int32_t InjectionEventDispatch::OnJson()
{
    MMI_LOGI("Enter onJson function.");
    const string path = injectArgvs_.at(JSON_FILE_PATH_INDEX);
    std::ifstream reader(path);
    if (!reader) {
        MMI_LOGE("json file is empty!");
        return RET_ERR;
    }
    Json inputEventArrays;
    reader >> inputEventArrays;
    reader.close();
    
    int32_t ret = manageInjectDevice_.TransformJsonData(inputEventArrays);
    MMI_LOGI("Leave onJson function.");
    return ret;
}

bool InjectionEventDispatch::GetStartSocketPermission(string id)
{
    auto it = mapNeedStartSocket_.find(id);
    if (it == mapNeedStartSocket_.end()) {
        return false;
    }

    return it->second;
}

int32_t InjectionEventDispatch::GetSendEventType(string id)
{
    auto it = mapSendEventType_.find(id);
    if (it == mapSendEventType_.end()) {
        return RET_ERR;
    }

    return it->second;
}

string InjectionEventDispatch::GetFunId()
{
    return funId_;
}

void InjectionEventDispatch::OnConnected()
{
    MMI_LOGI("InjectionEventDispatch::OnConnected.");

    string id = GetFunId();
    auto fun = GetFun(id);
    if (!fun) {
        MMI_LOGE("event injection Unknown fuction id[%{public}s]", id.c_str());
        return;
    }

    auto ret = (*fun)();
    if (ret == RET_OK) {
        MMI_LOGI("injecte function success id=[%{public}s]", id.c_str());
    } else {
        MMI_LOGE("injecte function faild id=[%{public}s]", id.c_str());
    }

    return;
}

void InjectionEventDispatch::InitConnnectFunction()
{
    const ConnnectFun fun = bind(&InjectionEventDispatch::OnConnected, this);
    InjectToolClient::GetInstance()->SetFunction(fun);
}

bool InjectionEventDispatch::VirifyArgvs(const int32_t &argc, const vector<string> &argv)
{
    MMI_LOGT("enter");
    if (argc < ARGV_VALID || argv.at(ARGVS_TARGET_INDEX).empty()) {
        MMI_LOGE("Invaild Input Para, Plase Check the validity of the para! errCode:%{public}d", PARAM_INPUT_FAIL);
        return false;
    }

    bool result = false;
    for (auto it : mapFuns_) {
        string temp(argv.at(ARGVS_TARGET_INDEX));
        if (temp == it.first) {
            funId_ = temp;
            result = true;
            break;
        }
    }
    if (result) {
        injectArgvs_.clear();
        for (uint64_t i = 1; i < static_cast<uint64_t>(argc); i++) {
            injectArgvs_.push_back(argv[i]);
        }
        argvNum_ = argc - 1;
    }

    return result;
}

bool InjectionEventDispatch::StartSocket()
{
    MMI_LOGT("enter");
    return InjectToolClient::GetInstance()->Start();
}

bool InjectionEventDispatch::SendMsg(NetPacket ckt)
{
    return InjectToolClient::GetInstance()->SendMsg(ckt);
}

bool InjectionEventDispatch::StartLog()
{
    string udsPath = GetEnv("UDS_PATH");
    if (udsPath.empty()) {
        udsPath = DEF_UDS_PATH;
    }
#ifdef OHOS_BUILD_MMI_DEBUG
    string log = GetEnv("MI_LOG");
    if (log.empty()) {
        log = DEF_LOG_CONFIG;
    }
    if (!LogManager::GetInstance().Init(DEF_LOG_CONFIG)) {
        printf("LOG object init error\n");
        return false;
    }
    if (!LogManager::GetInstance().Start()) {
        printf("LOG object start error\n");
        return false;
    }
#endif // OHOS_BUILD_MMI_DEBUG
    return true;
}

void InjectionEventDispatch::Run()
{
    MMI_LOGT("enter");
    string id = GetFunId();
    auto fun = GetFun(id);
    if (!fun) {
        MMI_LOGE("event injection Unknown fuction id[%{public}s]", id.c_str());
        return;
    }
    bool needStartSocket = GetStartSocketPermission(id);
    int32_t ret = RET_ERR;
    if (needStartSocket) {
        if (!StartSocket()) {
            MMI_LOGE("inject tools start socket error.");
            return;
        }
        this_thread::sleep_for(chrono::seconds(CLIENT_SLEEP_TIME));
    } else {
        ret = (*fun)();
        if (ret == RET_OK) {
            MMI_LOGI("injecte function success id=[%{public}s]", id.c_str());
        } else {
            MMI_LOGE("injecte function faild id=[%{public}s]", id.c_str());
        }
    }
    this_thread::sleep_for(chrono::seconds(1));
}

int32_t InjectionEventDispatch::ExecuteFunction(string funId)
{
    if (funId.empty()) {
        return RET_ERR;
    }
    auto fun = GetFun(funId);
    if (!fun) {
        MMI_LOGE("event injection Unknown fuction id[%{public}s]", funId.c_str());
        return false;
    }
    int32_t ret = RET_ERR;
    MMI_LOGI("Inject tools into function: [%{public}s]", funId.c_str());
    ret = (*fun)();
    if (ret == RET_OK) {
        MMI_LOGI("injecte function success id=[%{public}s]", funId.c_str());
    } else {
        MMI_LOGE("injecte function faild id=[%{public}s]", funId.c_str());
    }

    return ret;
}

int32_t InjectionEventDispatch::OnDevice()
{
    if (injectArgvs_.size() != DEVICE_ARGV_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    int32_t ret = RET_ERR;
    parse_.injectToHdf_ = false;

    string devicePath = injectArgvs_.at(DEVICE_PATH_INDEX);
    string commandPath = injectArgvs_.at(COMMAND_PATH_INDEX);
    if (commandPath.empty()) {
        return RET_ERR;
    }

    ret = parse_.getDriverFile(devicePath);
    if (ret == RET_ERR) {
        MMI_LOGE("execut getDriverFile faild.");
        return RET_ERR;
    }
    ret = parse_.getCommandFile(commandPath);
    if (ret == RET_ERR) {
        MMI_LOGE("execut getCommandFile faild.");
        return RET_ERR;
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnDrive()
{
    if (injectArgvs_.size() != DRIVE_ARGV_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    parse_.injectToHdf_ = false;
    int32_t ret = RET_ERR;
    string devicePath = injectArgvs_[DEVICE_PATH_INDEX];
    string commandPath = injectArgvs_[COMMAND_PATH_INDEX];
    if (devicePath.empty()) {
        return RET_ERR;
    }
    ret = parse_.getDriverFile(devicePath);
    vector<InputEvent> inputEventArray;
    if (commandPath.empty()) {
        return RET_ERR;
    }
    ret = parse_.AnalysisCommandFile(commandPath, inputEventArray);
    if (ret == RET_ERR) {
        MMI_LOGE("execut AnalysisCommandFile faild.");
    }
    for (InputEvent inputEvent : inputEventArray) {
        ret = parse_.SendCommand(inputEvent);
        if (ret == RET_OK) {
            MMI_LOGI("inject to drive successed.");
        } else {
            MMI_LOGD("inject to drive faild.");
        }
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnAisensor()
{
    printf("into function: OnAisensor()\n");
    int32_t exRet = RET_ERR;

    if (argvNum_ < AI_SENDOR_MIN_ARGV_NUMS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    string flag = injectArgvs_[AI_SENSOR_TARGET_INDEX];
    if (flag == "all") {
        exRet = ExecuteFunction("aisensor-all");
    } else if (flag == "each") {
        exRet = ExecuteFunction("aisensor-each");
    } else {
        // nothing to do.
    }

    return exRet;
}

int32_t InjectionEventDispatch::OnAisensorOne(uint32_t code, uint32_t value)
{
    timeval time;
    RawInputEvent rawEvent = {};
    int32_t msgType = MSG_TYPE_DEVICE_INFO;
    gettimeofday(&time, 0);
    rawEvent.ev_type = INPUT_DEVICE_CAP_AI_SENSOR;
    rawEvent.ev_code = code;
    rawEvent.ev_value = value;
    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
    NetPacket cktAi(ID_MSG_SENIOR_INPUT_FUNC);
    cktAi << msgType << rawEvent;
    if (SendMsg(cktAi)) {
        return RET_OK;
    } else {
        MMI_LOGE("Send AI Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }
}

int32_t InjectionEventDispatch::OnKnuckleOne(uint32_t code, uint32_t value)
{
    timeval time;
    RawInputEvent rawEvent = {};
    int32_t msgType = MSG_TYPE_DEVICE_INFO;
    gettimeofday(&time, 0);
    rawEvent.ev_type = INPUT_DEVICE_CAP_AI_SENSOR;
    rawEvent.ev_code = code;
    rawEvent.ev_value = value; 
    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
    NetPacket cktKnuckle(ID_MSG_SENIOR_INPUT_FUNC);
    cktKnuckle << msgType << rawEvent;
    if (SendMsg(cktKnuckle)) {
        return RET_OK;
    } else {
        MMI_LOGE("Send Knuckle Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }
}

int32_t InjectionEventDispatch::OnAisensorEach()
{
    if (argvNum_ != AI_EACH_ARGV_INVALID) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    const std::string& inputString = injectArgvs_[AI_SENSOR_CODE_INDEX];
    bool ret = std::all_of(inputString.begin(), inputString.end(), [](char c) {
        return isdigit(c);
        });
    if (!ret) {
        MMI_LOGE("Invaild Input Para, Plase Check the validity of the para! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    int32_t msgType = MSG_TYPE_DEVICE_INIT;
    int32_t devIndex = 0;
    int32_t devType = INPUT_DEVICE_CAP_AISENSOR;

    NetPacket cktAiInit(ID_MSG_SENIOR_INPUT_FUNC);
    cktAiInit << msgType << devIndex << devType;
    SendMsg(cktAiInit);

    timeval time;
    RawInputEvent rawEvent = {};
    gettimeofday(&time, 0);
    msgType = MSG_TYPE_DEVICE_INFO;
    rawEvent.ev_type = INPUT_DEVICE_CAP_AI_SENSOR;
    rawEvent.ev_code = static_cast<uint32_t>(stoi(injectArgvs_[AI_SENSOR_CODE_INDEX]));
    rawEvent.ev_value = static_cast<uint32_t>(stoi(injectArgvs_[AI_SENSOR_VALUE_INDEX]));
    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
    NetPacket cktAi(ID_MSG_SENIOR_INPUT_FUNC);
    cktAi << msgType << rawEvent;
    if (SendMsg(cktAi)) {
        return RET_OK;
    } else {
        MMI_LOGE("Send AI Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InjectionEventDispatch::OnKnuckleEach()
{
    if (argvNum_ != AI_EACH_ARGV_INVALID) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    const std::string& inputString = injectArgvs_[AI_SENSOR_CODE_INDEX];
    bool ret = std::all_of(inputString.begin(), inputString.end(), [](char c) {
        return isdigit(c);
        });
    if (!ret) {
        MMI_LOGE("Invaild Input Para, Plase Check the validity of the para! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    int32_t msgType = MSG_TYPE_DEVICE_INIT;
    int32_t devIndex = 0;
    int32_t devType = INPUT_DEVICE_CAP_KNUCKLE;

    NetPacket cktAiInit(ID_MSG_SENIOR_INPUT_FUNC);
    cktAiInit << msgType << devIndex << devType;
    SendMsg(cktAiInit);

    timeval time;
    RawInputEvent rawEvent = {};
    gettimeofday(&time, 0);
    msgType = MSG_TYPE_DEVICE_INFO;
    rawEvent.ev_type = INPUT_DEVICE_CAP_AI_SENSOR;
    rawEvent.ev_code = static_cast<uint32_t>(stoi(injectArgvs_[AI_SENSOR_CODE_INDEX]));
    rawEvent.ev_value = static_cast<uint32_t>(stoi(injectArgvs_[AI_SENSOR_VALUE_INDEX]));
    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
    NetPacket cktKnuckle(ID_MSG_SENIOR_INPUT_FUNC);
    cktKnuckle << msgType << rawEvent;
    if (SendMsg(cktKnuckle)) {
        return RET_OK;
    } else {
        MMI_LOGE("Send AI Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }
    return RET_OK;
}

int32_t InjectionEventDispatch::OnHelp()
{
    InjectionToolsHelpFunc helpFunc;
    string ret = helpFunc.GetHelpText();
    printf("%s", ret.c_str());

    return RET_OK;
}

int32_t InjectionEventDispatch::OnAisensorAll()
{
    uint16_t cycleNum = 0;
    if (argvNum_ == AI_SENSOR_DEFAULT_NUMS) {
        cycleNum = AI_SENSOR_DEFAULT_CYCLE_NUMS;
    } else if (argvNum_ == AI_SENSOR_CYCLE_NUMS) {
        cycleNum = static_cast<uint16_t>(stoi(injectArgvs_[AI_SENSOR_CYCLE_INDEX]));
    } else {
        // nothing to do.
    }

    ProcessAiSensorInfoByCycleNum(cycleNum);

    return RET_OK;
}

int32_t InjectionEventDispatch::OnKnuckleAll()
{
    uint16_t cycleNum = 0;
    if (argvNum_ == AI_SENSOR_DEFAULT_NUMS) {
        cycleNum = AI_SENSOR_DEFAULT_CYCLE_NUMS;
    } else if (argvNum_ == AI_SENSOR_CYCLE_NUMS) {
        cycleNum = static_cast<uint16_t>(stoi(injectArgvs_[AI_SENSOR_CYCLE_INDEX]));
    } else {
        // nothing to do.
    }

    ProcessKnuckleInfoByCycleNum(cycleNum);

    return RET_OK;
}

int32_t InjectionEventDispatch::OnHdi()
{
    MMI_LOGI("into Onhdi function.");
    if ((injectArgvs_.size() < HDI_MIN_ARGV_NUMS) || (injectArgvs_.size() > HDI_MAX_ARGV_NUMS)) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    parse_.injectToHdf_ = true;
    string hdiFunctionId = "hdi-" + injectArgvs_[HDF_TARGET_INDEX];
    int32_t ret = ExecuteFunction(hdiFunctionId);

    return ret;
}

int32_t InjectionEventDispatch::OnHdiStatus()
{
    printf("into function: OnHdiStatus()\n");
    if (injectArgvs_.size() != HDI_STATUS_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    int32_t sendType = static_cast<int32_t>(SHOW_DEVICE_INFO);
    NetPacket cktHdi(ID_MSG_HDI_INJECT);
    cktHdi << sendType;
    if (!(SendMsg(cktHdi))) {
        MMI_LOGE("hdi hot plug to server errot");
        return RET_OK;
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnHdiHot()
{
    if (injectArgvs_.size() != HDI_HOT_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    string deviceStatusText = injectArgvs_[HDI_STATUS_INDEX];
    int32_t status = GetDeviceStatus(deviceStatusText);
    if (status == RET_ERR) {
        MMI_LOGE("OnHdiHot status error ,status:%{public}d", status);
        return RET_ERR;
    }

    string deviceNameText = injectArgvs_.at(HDI_DEVICE_NAME_INDEX);
    int32_t index = GetDeviceIndex(deviceNameText);
    printf("OnHdiHot index  = %d\n", index);
    if (index == RET_ERR) {
        MMI_LOGE("OnHdiHot index error ,index:%{public}d", index);
        return RET_ERR;
    }

    int32_t sendType = static_cast<int32_t>(SET_HOT_PLUGS);
    uint32_t devIndex = static_cast<uint32_t>(index);
    uint32_t devSatatus = static_cast<uint32_t>(status);
    NetPacket cktHdi(ID_MSG_HDI_INJECT);
    cktHdi << sendType << devIndex << devSatatus;
    if (!(SendMsg(cktHdi))) {
        MMI_LOGE("hdi hot plug to server error.");
        return RET_OK;
    }
    printf("On hdi hot SendMsg......\n");
    return RET_OK;
}

int32_t InjectionEventDispatch::GetDeviceIndex(const string& deviceNameText)
{
    if (deviceNameText.empty()) {
        return RET_ERR;
    }
    vector<DeviceInformation>::iterator iter;
    for (iter = allDevices.begin(); iter != allDevices.end(); iter++) {
        if (deviceNameText == iter->chipName) {
            return iter->devIndex;
        }
    }

    return RET_ERR;
}

int32_t InjectionEventDispatch::GetDeviceStatus(const string &deviceStatusText)
{
    if (deviceStatusText.empty()) {
        return RET_ERR;
    }
    if (deviceStatusText == "add") {
        return HDI_ADD;
    } else if (deviceStatusText == "remove") {
        return HDI_REMOVE;
    } else {
        return RET_ERR;
    }

    return RET_ERR;
}

int32_t InjectionEventDispatch::OnHdiEvent()
{
    if (injectArgvs_.size() != HDI_EVENT_ARGV_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    string commandPath = injectArgvs_[HDI_EVENT_COMMAND_INDEX];
    vector<InputEvent> inputEventArray;
    int32_t ret = parse_.AnalysisCommandFile(commandPath, inputEventArray);
    if (ret == RET_ERR) {
        MMI_LOGE("OnHdiEvent AnalysisCommandFile faild.");
        return ret;
    }
    for (InputEvent inputEvent : inputEventArray) {
        parse_.SendCommand(inputEvent);
    }

    return RET_OK;
}

#ifndef OHOS_BUILD_HDF
int32_t InjectionEventDispatch::OnSendEvent()
{
    if (injectArgvs_.size() != SEND_EVENT_ARGV_COUNTS) {
        MMI_LOGE("Wrong number of input parameters! errCode:%d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }

    string deviceNode = injectArgvs_[SEND_EVENT_DEV_NODE_INDEX];
    if (deviceNode.empty()) {
        MMI_LOGE("device node:%s is not exit.", deviceNode.c_str());
        return RET_ERR;
    }
    timeval tm;
    gettimeofday(&tm, 0);
    input_event event = {};
    event.input_event_sec = tm.tv_sec;
    event.input_event_usec = tm.tv_usec;
    event.type = static_cast<int32_t>(std::stoi(injectArgvs_[SEND_EVENT_TYPE_INDEX]));
    event.code = static_cast<uint16_t>(std::stoi(injectArgvs_[SEND_EVENT_CODE_INDEX]));
    event.value = static_cast<int32_t>(std::stoi(injectArgvs_[SEND_EVENT_VALUE_INDEX]));

    int32_t fd = open(deviceNode.c_str(), O_RDWR);
    if (fd < 0) {
        MMI_LOGE("open device node:%s faild.", deviceNode.c_str());
        return RET_ERR;
    }
    write(fd, &event, sizeof(event));
    if (fd > 0) {
        close(fd);
    }
    return RET_OK;
}
#endif

void InjectionEventDispatch::InitDeviceInfo()
{
    DeviceInformation deviceInfoArray[] = {
        {HDI_REMOVE, INPUT_DEVICE_POINTER_INDEX, HDF_MOUSE_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "mouse"},
        {HDI_REMOVE, INPUT_DEVICE_KEYBOARD_INDEX, HDF_KEYBOARD_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "keyboard"},
        {HDI_REMOVE, INPUT_DEVICE_TOUCH_INDEX, HDF_TOUCH_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "touch"},
        {HDI_REMOVE, INPUT_DEVICE_TABLET_TOOL_INDEX, HDF_TABLET_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "pen"},
        {HDI_REMOVE, INPUT_DEVICE_TABLET_PAD_INDEX, HDF_TABLET_PAD_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "pad"},
        {HDI_REMOVE, INPUT_DEVICE_FINGER_INDEX, HDF_TOUCH_FINGER_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "finger"},
        {HDI_REMOVE, INPUT_DEVICE_SWITCH_INDEX, HDF_SWITCH_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "knob"},
        {HDI_REMOVE, INPUT_DEVICE_TRACKPAD5_INDEX, HDF_TRACK_PAD_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "trackPad"},
        {HDI_REMOVE, INPUT_DEVICE_JOYSTICK_INDEX, HDF_JOYSTICK_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "joyStick"},
        {HDI_REMOVE, INPUT_DEVICE_GAMEPAD_INDEX, HDF_GAMEPAD_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "gamePad"},
        {HDI_REMOVE, INPUT_DEVICE_TOUCH_PAD, HDF_TOUCH_PAD_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS, "touchPad"},
        {HDI_REMOVE, INPUT_DEVICE_REMOTE_CONTROL, HDF_TRACK_BALL_DEV_TYPE, HDF_DEVICE_FD_DEFAULT_STATUS,
         "remoteControl"},
    };

    int32_t counts = sizeof(deviceInfoArray) / sizeof(DeviceInformation);
    allDevices.insert(allDevices.begin(), deviceInfoArray, deviceInfoArray + counts);
}

int32_t InjectionEventDispatch::GetDevTypeByIndex(int32_t devIndex)
{
    vector<DeviceInformation>::iterator iter;
    for (iter = allDevices.begin(); iter != allDevices.end(); iter++) {
        if (devIndex == iter->devIndex) {
            return iter->devType;
        }
    }

    return RET_ERR;
}

int32_t InjectionEventDispatch::GetDevIndexByType(int32_t devType)
{
    vector<DeviceInformation>::iterator iter;
    for (iter = allDevices.begin(); iter != allDevices.end(); iter++) {
        if (devType == iter->devType) {
            return iter->devIndex;
        }
    }

    return RET_ERR;
}

int32_t InjectionEventDispatch::OnDump()
{
    NetPacket cktDump(ID_MSG_ON_DUMP);
    ID_MSG idMsg = ID_MSG_ON_DUMP;
    cktDump << idMsg;
    if (!(SendMsg(cktDump))) {
        MMI_LOGE("Send hdi dump fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnList()
{
    if (injectArgvs_.size() < HDI_ALL_ARGV_INVALID) {
        return RET_ERR;
    }
    NetPacket cktList(ID_MSG_ON_LIST);
    struct timeval time;
    RawInputEvent rawEvent = {};
    gettimeofday(&time, 0);

    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);
    rawEvent.ev_type = static_cast<uint32_t>(INJECTION_LIST);
    rawEvent.ev_code = static_cast<uint32_t>(std::stoi(injectArgvs_[TEST_ON_LIST_CODE_INDEX]));
    rawEvent.ev_value = static_cast<uint32_t>(std::stoi(injectArgvs_[TEST_ON_LIST_VALUE_INDEX]));

    cktList << rawEvent;
    if (!(SendMsg(cktList))) {
        MMI_LOGE("Send list Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnException()
{
    if (injectArgvs_.size() < HDI_ALL_ARGV_INVALID) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    NetPacket cktException(ID_MSG_ON_EXCEPTION);
    struct timeval time;
    RawInputEvent rawEvent = {};
    gettimeofday(&time, 0);
    rawEvent.ev_type = static_cast<uint32_t>(INJECTION_EXCEPTION);
    rawEvent.ev_code = static_cast<uint32_t>(std::stoi(injectArgvs_[TEST_EXCEPTION_CODE_INDEX]));
    rawEvent.ev_value = static_cast<uint32_t>(std::stoi(injectArgvs_[TEST_EXCEPTION_VALUE_INDEX]));
    rawEvent.stamp = static_cast<uint32_t>(time.tv_usec);

    cktException << rawEvent;
    if (!(SendMsg(cktException))) {
        MMI_LOGE("Send AI Msg fail! errCode:%{public}d", MSG_SEND_FAIL);
        return RET_ERR;
    }

    return RET_OK;
}

int32_t InjectionEventDispatch::OnTouchEvent()
{
    printf("into function: OnTouchEvent()\n");
    if (injectArgvs_.size() > TOUCH_ARGV_INVALID) {
        MMI_LOGE("Wrong number of input parameters! errCode:%{public}d", PARAM_INPUT_FAIL);
        return RET_ERR;
    }
    parse_.injectToHdf_ = false;
    string touchDeviceNode = "/dev/input/event0";
    int32_t ret = touchEvent_.SetDeviceHandle(touchDeviceNode);
    ret = touchEvent_.GetTouchFile(injectArgvs_[TOUCH_FILE_INDEX]);
    if (ret == RET_OK) {
        MMI_LOGI("execute touchevent success.");
    } else {
        MMI_LOGI("execute touchevent faild.");
    }

    return ret;
}

void OHOS::MMI::InjectionEventDispatch::ProcessAiSensorInfoByCycleNum(uint16_t cycleNum)
{
    static const vector<ID_MSG> aiSensorAllowProcCodes {
        ID_MSG_ON_SHOW_MENU,
        ID_MSG_ON_SEND,
        ID_MSG_ON_COPY,
        ID_MSG_ON_PASTE,
        ID_MSG_ON_CUT,
        ID_MSG_ON_UNDO,
        ID_MSG_ON_REFRESH,
        ID_MSG_ON_CANCEL,
        ID_MSG_ON_ENTER,
        ID_MSG_ON_PREVIOUS,
        ID_MSG_ON_NEXT,
        ID_MSG_ON_BACK,
        ID_MSG_ON_PRINT,
        ID_MSG_ON_PLAY,
        ID_MSG_ON_PAUSE,
        ID_MSG_ON_SCREEN_SHOT,
        ID_MSG_ON_SCREEN_SPLIT,
        ID_MSG_ON_START_SCREEN_RECORD,
        ID_MSG_ON_STOP_SCREEN_RECORD,
        ID_MSG_ON_GOTO_DESKTOP,
        ID_MSG_ON_RECENT,
        ID_MSG_ON_SHOW_NOTIFICATION,
        ID_MSG_ON_LOCK_SCREEN,
        ID_MSG_ON_SEARCH,
        ID_MSG_ON_CLOSEPAGE,
        ID_MSG_ON_LAUNCH_VOICE_ASSISTANT,
        ID_MSG_ON_MUTE,
        ID_MSG_ON_ANSWER,
        ID_MSG_ON_REFUSE,
        ID_MSG_ON_HANG_UP,
        ID_MSG_ON_START_DRAG,
        ID_MSG_ON_MEDIA_CONTROL,
        ID_MSG_ON_TELEPHONE_CONTROL
    };

    int32_t msgType = MSG_TYPE_DEVICE_INIT;
    int32_t devIndex = 0;
    int32_t devType = INPUT_DEVICE_CAP_AISENSOR;

    NetPacket cktAiInit(ID_MSG_SENIOR_INPUT_FUNC);
    cktAiInit << msgType << devIndex << devType;
    SendMsg(cktAiInit);

    for (uint32_t item = 0; item < cycleNum; item++) {
        for (auto i : aiSensorAllowProcCodes) {
            OnAisensorOne(i, item);
        }
    }
}

void OHOS::MMI::InjectionEventDispatch::ProcessKnuckleInfoByCycleNum(uint16_t cycleNum)
{
    static const vector<ID_MSG> aiSensorAllowProcCodes = {
        ID_MSG_ON_SCREEN_SHOT,
        ID_MSG_ON_SCREEN_SPLIT,
        ID_MSG_ON_START_SCREEN_RECORD,
        ID_MSG_ON_STOP_SCREEN_RECORD,
    };

    int32_t msgType = MSG_TYPE_DEVICE_INIT;
    int32_t devIndex = 0;
    int32_t devType = INPUT_DEVICE_CAP_KNUCKLE;

    NetPacket cktAiInit(ID_MSG_SENIOR_INPUT_FUNC);
    cktAiInit << msgType << devIndex << devType;
    SendMsg(cktAiInit);

    for (uint32_t item = 0; item < cycleNum; item++) {
        for (auto i : aiSensorAllowProcCodes) {
            OnKnuckleOne(i, item);
        }
    }
}
