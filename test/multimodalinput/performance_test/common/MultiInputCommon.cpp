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
 /**
 * @addtogroup multiInput
 * @{
 *
 * @brief Defines multiInput-related tests, including case data and functions for auto test,
 *
 * @since 1.0
 * @version 1.0
 */
#include "MultiInputCommon.h"


OHOS::MMI::MultiInputCommon::MultiInputCommon()
{
}

OHOS::MMI::MultiInputCommon::~MultiInputCommon()
{
}

void OHOS::MMI::MultiInputCommon::ClientOne()
{
    system("hosmmi-client 0 0 480 480 0 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientTwo()
{
    system("hosmmi-client 0 0 480 480 0 0 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientThree()
{
    system("./hosmmi_client 0 0 0 0 0 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientFour()
{
    system("./hosmmi_client 500 500 20000 20000 1 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientFive()
{
    system("./hosmmi_client 0 0 35000 35000 1 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientSix()
{
    system("./hosmmi_client 20500 500 20000 20000 0 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientSeven()
{
    system("./hosmmi_client 10000 10000 30000 30000 1 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::ClientEight()
{
    system("./hosmmi_client 500 500 20000 20000 1 1 &");
    return;
}

void OHOS::MMI::MultiInputCommon::WriteDeviceIni(const std::string &iniFilePath)
{
    if (iniFilePath.empty()) {
        return;
    }
    std::string deviceFile = iniFilePath + "/event.ini";

    std::ofstream fileWriter(deviceFile, std::ios_base::trunc);
    fileWriter.close();

    std::ofstream fout;
    if (fout.bad()) {
        return;
    }
    fout.open(deviceFile, std::ios::app);

    fout << "/dev/input/event2" << std::endl;
    fout << "/dev/input/event2" << std::endl;
    fout << "/dev/input/event3" << std::endl;
    fout << "/dev/input/event4" << std::endl;
    fout << "/dev/input/event5" << std::endl;
    fout << "/dev/input/event6" << std::endl;
    fout << "/dev/input/event7" << std::endl;
    fout << "/dev/input/event8" << std::endl;
    fout << "/dev/input/event9" << std::endl;
    fout << "/dev/input/event10" << std::endl;
    fout << "/dev/input/event11" << std::endl;
    fout << "/dev/input/event12" << std::endl;
    fout << "/dev/input/event13" << std::endl;
    fout << "/dev/input/event14" << std::endl;
    fout << "/dev/input/event15" << std::endl;
    fout << "/dev/input/event16" << std::endl;
    fout << "/dev/input/event17" << std::endl;
    fout << "/dev/input/event18" << std::endl;
    fout << "/dev/input/event19" << std::endl;
    fout << "/dev/input/event20" << std::endl;
    fout << "/dev/input/event21" << std::endl;
    fout << "/dev/input/event22" << std::endl;
    fout << "/dev/input/event23" << std::endl;
    fout << "/dev/input/event24" << std::endl;
    fout << "/dev/input/event25" << std::endl;
    fout << "/dev/input/event26" << std::endl;
    fout << "/dev/input/event27" << std::endl;
    fout << "/dev/input/event28" << std::endl;
    fout << "/dev/input/event29" << std::endl;
    fout << "/dev/input/event30" << std::endl;
    fout << "/dev/input/event31" << std::endl;
    fout << "/dev/input/event256" << std::endl;
    fout << "/dev/input/event257" << std::endl;
    fout << "/dev/input/event258" << std::endl;
    fout << "/dev/input/event259" << std::endl;
    fout << "/dev/input/event260" << std::endl;
    fout << "/dev/input/event261" << std::endl;
    fout << "/dev/input/event262" << std::endl;
    fout << "/dev/input/event263" << std::endl;
    fout << "/dev/input/event264" << std::endl;
    fout << "/dev/input/event264" << std::endl;
    fout << "/dev/input/event264" << std::endl;
    fout << "/dev/input/event264" << std::endl;
    fout << "/dev/input/event265" << std::endl;
    fout << "/dev/input/event266" << std::endl;
    fout << "/dev/input/event267" << std::endl;
    fout << "/dev/input/event268" << std::endl;
    fout << "/dev/input/event269" << std::endl;
    fout << "/dev/input/event270" << std::endl;
    fout << "/dev/input/event271" << std::endl;
    fout << "/dev/input/event272" << std::endl;
    fout << "/dev/input/event273" << std::endl;
    fout << "/dev/input/event274" << std::endl;
    fout << "/dev/input/event275" << std::endl;
    fout << "/dev/input/event276" << std::endl;
    fout << "/dev/input/event277" << std::endl;
    fout << "/dev/input/event278" << std::endl;
    fout << "/dev/input/event279" << std::endl;
    fout.close();

    return;
}

void OHOS::MMI::MultiInputCommon::InJectionEvent(const std::string &eventValue, const std::string &deviceMsg)
{
    std::string injectionCommand = "";
    if (eventValue == "") return;
    //AI
    if (deviceMsg.find("Voice")!= deviceMsg.npos) { 
        injectionCommand = "hosmmi-event-injection aisensor-each " + eventValue;
        system(injectionCommand.data());
    } 
    //injectionCommand = "hosmmi-event-injection knuckle-each " + eventValue;
    else if (deviceMsg.find("Phalangeal")!= deviceMsg.npos) { 
        system(injectionCommand.data());
    } else if (eventValue.find(".json")!= eventValue.npos) {
        std::string injectionCommand = "hosmmi-event-injection json /data/json/"+eventValue;
        system(injectionCommand.data());
        sleep(5);
    } else {
        std::string commandFile = "/data/event/event_s.ini";
        std::ofstream file;
        if (file.bad()) return;
        file.open(commandFile, std::ios::app);
//        system("echo "" > /data/event/event_s.ini");
//        file << eventValue << std::endl;
        file << eventValue << std::endl;
        file.close();
        if (eventValue.find("41") == 0 || eventValue.find("42") == 0 || eventValue.find("43") == 0 ) {
            injectionCommand = "hosmmi-event-injection touch /data/event/event_s.ini";
            sleep(5);
        } else {
            injectionCommand = "hosmmi-event-injection hdi event /data/event/event_s.ini";
        }
        system("cat /data/event/event_s.ini");
        std::cout<< injectionCommand <<std::endl;
        system(injectionCommand.data());
        std::ofstream file_writer(commandFile, std::ios_base::trunc);
        file_writer.close();
    }
    return;
}

std::string OHOS::MMI::MultiInputCommon::GetProcessByName(const std::string &name)
{
    FILE* fp;
    std::string buffer;
    char buf[PROCESS_SEEK_LENGTH] = "";
    char cmd[PROCESS_SEEK_LENGTH] = {'\0'};
    std::string procName = name;
    sprintf_s(cmd, sizeof(cmd), "pidof %s", procName.data());
    if ((fp = popen(cmd, "r")) != nullptr) {
        if (fgets(buf, PROCESS_SEEK_LENGTH, fp) != nullptr) {
            buffer = buf;
        }
    }
    pclose(fp);

    return buffer;
}

void OHOS::MMI::MultiInputCommon::InitClient(const std::string &scene)
{
    std::string processName = "hosmmi-client";
    std::string closeProcess = "kill -9 " + (GetProcessByName(processName));
    system(closeProcess.data());
    processName = "./hosmmi_client";
    closeProcess = "kill -9 " + (GetProcessByName(processName));
    system(closeProcess.data());
    sleep(1);
    std::string iniFilePath = "/data/event/";
    mkdir(iniFilePath.data(), FILE_FOUND_JURISDICTION);
    WriteDeviceIni(iniFilePath);
    AssignmentResult(scene);
    StartProcess(scene);
    return;
}

void OHOS::MMI::MultiInputCommon::AssignmentResult(const std::string &scene)
{
    if (scene == "Scene_C") {
        valueServer = "[surfaceId1]|surfaceId1|0|[surfaceId1,fd1,abilityId1]";
        valueClient = "[surfaceId1,fd1,abilityId1]";
    } else if (scene == "Scene_A") {
        valueServer = "[]|0|0|[0,fd1,abilityId1]";
        valueClient = "[0,fd1,abilityId1]";
    } else if (scene == "Scene_B") {
        valueServer = "[]|0|0|[0,fd1,abilityId1]";
        valueClient = "[0,fd1,abilityId1]";
    } else if (scene == "Scene_H") {
        valueServer = "[surfaceId1]|surfaceId1|0|[surfaceId1,fd1,abilityId1]";
        valueClient = "[surfaceId1,fd1,abilityId1]";
    } else if (scene == "Scene_R") {
        valueServer = "[surfaceId1]|surfaceId1|0|[surfaceId1,fd1,abilityId1]";
        valueClient = "[surfaceId1,fd1,abilityId1]";
    } else if (scene == "Scene_D") {
        valueServer = "[surfaceId1]|surfaceId1|0|[surfaceId1,fd1,abilityId1]";
        valueClient = "[surfaceId1,fd1,abilityId1]";
    } else {
    }
    return;
}

void OHOS::MMI::MultiInputCommon::StartProcess(const std::string &scene)
{
    SplitComplexity(scene);
    if ((scene == "Scene_A") || (scene == "Scene_B")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientThree, this);
        clientOne.detach();
    } else if (scene == "Scene_C") {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        clientOne.detach();
    } else if ((scene == "Scene_D") || (scene == "Scene_R") || (scene == "Scene_H")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientTwo, this);
        clientOne.detach();
    } else if (scene == "Scene_F") {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientTwo, this);
        clientOne.detach();
        clientTwo.detach();
    } else if ((scene == "Scene_I") || (scene == "Scene_J") || (scene == "Scene_S") || (scene == "Scene_K")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientSix, this);
        clientOne.detach();
        clientTwo.detach();
    } else if (scene == "Scene_M") {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientFive, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientEight, this);
        clientOne.detach();
        clientTwo.detach();
    } else if ((scene == "Scene_L") || (scene == "Scene_V")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientFive, this);
        clientOne.detach();
        clientTwo.detach();
    } else if ((scene == "Scene_N") || (scene == "Scene_U") || (scene == "Scene_E")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientFour, this);
        clientOne.detach();
        clientTwo.detach();
    } else {
    }
    return;
}

void OHOS::MMI::MultiInputCommon::SplitComplexity(const std::string &scene)
{
    if ((scene == "Scene_W") || (scene == "Scene_O") || (scene == "Scene_P") || (scene == "Scene_Q") ||
        (scene == "Scene_T")) {
        std::thread clientOne(&OHOS::MMI::MultiInputCommon::ClientOne, this);
        std::thread clientTwo(&OHOS::MMI::MultiInputCommon::ClientSeven, this);
        clientOne.detach();
        clientTwo.detach();
    }
    return;
}
