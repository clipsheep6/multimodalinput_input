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
import {expect} from 'deccjsunit/index';

function sleep(numberMillis) {
    var now = new Date();
    var exitTime = now.getTime() + numberMillis;
    while (true) {
        now = new Date();
        if (now.getTime() > exitTime)
        return;
    }
}

function attributeAverageTimeCheck(ApiCall) {
    var arr = new Array();
    for (let i = 0; i < 1000; i++) {
        var startTime = new Date().getTime();
        ApiCall;
        var endTime = new Date().getTime();
        var timeResult = endTime - startTime;
        arr.push(timeResult);
    }
    console.log(`inputEventClient::runTimes = ` + arr.length + 'times');
    var summary = 0;
    for (let j = 0; j < arr.length; j++) {
        summary += arr[j];
    }
    console.log(`inputEventClient::averageExeTime = ` + summary.toFixed(2) + 'us');
    expect(parseFloat(summary.toFixed(2)) < 10.00).assertEqual(true);
}

function registerAverageTimeCheck(onCall, offCall) {
    var onArr = new Array();
    var offArr = new Array();
    for (let item = 0; item < 1000; item++) {
        var onStartTime = new Date().getTime();
        const registerResult = onCall;
        var onEndTime = new Date().getTime();
        var onTimeResult = onEndTime - onStartTime;
        onArr.push(onTimeResult);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            var offStartTime = new Date().getTime();
            const unregisterResult = offCall;
            var offEndTime = new Date().getTime();
            var offTimeResult = offEndTime - offStartTime;
            offArr.push(offTimeResult);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    }
    console.log(`inputEventClient::onRunTimes = ` + onArr.length + 'times');
    var onSummary = 0;
    for (let onIndex = 0; onIndex < onArr.length; onIndex++) {
        onSummary += onArr[onIndex];
    }
    console.log(`inputEventClient::onAverageExeTime = ` + onSummary.toFixed(2) + 'us');
    expect(parseFloat(onSummary.toFixed(2)) < 10.00).assertEqual(true);

    console.log(`inputEventClient::offRunTimes = ` + offArr.length + 'times');
    var offSummary = 0;
    for (let offIndex = 0; offIndex < offArr.length; offIndex++) {
        offSummary += offArr[offIndex];
    }
    console.log(`inputEventClient::offAverageExeTime = ` + offSummary.toFixed(2) + 'us');
    expect(parseFloat(offSummary.toFixed(2)) < 10.00).assertEqual(true);
}

const traditionalEvent = "traditionalEvent.ini";
const aiVoiceEvent = "aiVoiceEvent.ini";
const MMI_STANDARD_EVENT_INVALID_PARAMETER = -1;
const MMI_STANDARD_EVENT_SUCCESS = 1;
const MMI_STANDARD_EVENT_EXIST = 2;
const MMI_STANDARD_EVENT_NOT_EXIST = 3;
const API_CONFIG_SUCCESS = 0;
const WINDOW_ID = 123;
const EventCallback = {
    ON_SHOW_MENU: 0,
    ON_SEND: 1,
    ON_COPY: 2,
    ON_PASTE: 3,
    ON_CUT: 4,
    ON_UNDO: 5,
    ON_REFRESH: 6,
    ON_CANCEL: 8,
    ON_ENTER: 9,
    ON_PREVIOUS: 10,
    ON_NEXT: 11,
    ON_BACK: 12,
    ON_PRINT: 13,
    ON_ANSWER: 14,
    ON_REFUSE: 15,
    ON_HANGUP: 16,
    ON_TELEPHONE_CONTROL: 17,
    ON_PLAY: 18,
    ON_PAUSE: 19,
    ON_MEDIA_CONTROL: 20
}

export {
    sleep,
    attributeAverageTimeCheck,
    registerAverageTimeCheck,
    traditionalEvent,
    aiVoiceEvent,
    MMI_STANDARD_EVENT_INVALID_PARAMETER,
    MMI_STANDARD_EVENT_SUCCESS,
    MMI_STANDARD_EVENT_EXIST,
    MMI_STANDARD_EVENT_NOT_EXIST,
    API_CONFIG_SUCCESS,
    WINDOW_ID,
    EventCallback,
}
