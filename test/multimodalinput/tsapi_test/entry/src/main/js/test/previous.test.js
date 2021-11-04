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
// @ts-ignore
import inputEventClient from '@ohos.multimodalInput.inputEventClient';
import {describe, it, expect} from 'deccjsunit/index';
import {sleep, attributeAverageTimeCheck, registerAverageTimeCheck, traditionalEvent, aiVoiceEvent,
    MMI_STANDARD_EVENT_INVALID_PARAMETER,
    MMI_STANDARD_EVENT_SUCCESS, MMI_STANDARD_EVENT_EXIST, MMI_STANDARD_EVENT_NOT_EXIST, API_CONFIG_SUCCESS, WINDOW_ID,
    EventCallback} from './common.js'

describe('tsApi_previous_test', function () {
    it('inputEventClient001::multi_input_tsApi_previous_001: mouse', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_mouse_wheel_up.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient001:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('2');
            expect(event.inputDeviceId).assertEqual('3');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient001:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient001:previous_mouse-02: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient002::multi_input_tsApi_previous_002, keyboard:pageUp', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_pageUp.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient002:Onprevious_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient002:dataVerify end`);

        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient002:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient003::multi_input_tsApi_previous_003, keyboard:up', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient002:Onprevious_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient002:dataVerify end`);

        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient002:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient004::multi_input_tsApi_previous_004, keyboard:left', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_left.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient002:Onprevious_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient002:dataVerify end`);

        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient002:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient005::multi_input_tsApi_previous_005, remoteControl:volumeUp', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_remoteControl_key_volumeUp_115.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient003:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('7');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient003:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient003:previous: register success`);
            console.log(`inject_Onsend START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient005::multi_input_tsApi_previous_006, remoteControl:channelUp', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_remoteControl_key_channelUp_402.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient003:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('7');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient003:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient003:previous: register success`);
            console.log(`inject_Onsend START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient007::multi_input_tsApi_previous_007, touchPad', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_touchPad_three_finger_move_to_left.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient005:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('9');
            expect(event.inputDeviceId).assertEqual('33365');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient005:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient005:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient008::multi_input_tsApi_previous_008, aiSensor', 0, function () {
        let injectEvent = {
            eventFileName: aiVoiceEvent,
            jsonEvent: '1111 1'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient006:previous dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('6');
            expect(event.inputDeviceId).assertEqual('31');
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient006:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient006:previous register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient009::multi_input_tsApi_previous_009, not exist call back handler', 0, function () {
        console.log(`inputEventClient007::previous start`);
        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', 32323);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient010::multi_input_tsApi_previous_010, not exist register function', 0, function () {

        const eventHandle = (event) => {
            console.log(`inputEventClient008:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient008:dataVerify end`);
        }


        const registerResult = inputEventClient.on(WINDOW_ID, 2423, eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient011::multi_input_tsApi_previous_011, register other ui standard function', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_enter.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient010:enter_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertNotEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient010:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'enter', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient010:previous: register success`);
            console.log(`inject_enter START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_enter END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'enter', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient012::multi_input_tsApi_previous_012, unregister not exist call back handler', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient012:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient012:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient011:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'previous', 12321);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
            console.log(`unregisterResultFirst:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient013::multi_input_tsApi_previous_013, unregister not exist register function', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient012:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient012:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient012:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, '10', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            console.log(`unregisterResultFirst:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient014::multi_input_tsApi_previous_014, inject space', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: ' '
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient013:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual('');
            expect(event.occurredTime).assertEqual('');
            expect(event.sourceDevice).assertEqual('');
            expect(event.inputDeviceId).assertEqual('');
            expect(event.type).assertEqual('');
            console.log(`inputEventClient013:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient013:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient015::multi_input_tsApi_previous_015, repeat register', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient014:Onprevious_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient014:dataVerify end`);

        }

        const registerResultFirst = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
            const registerResultSecond = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
            expect(registerResultSecond).assertEqual(MMI_STANDARD_EVENT_EXIST);
            console.log(`registerResultSecond:` + registerResultSecond);
            console.log(`inputEventClient014:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient016::multi_input_tsApi_previous_016, repeat unregister', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient015:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);
            console.log(`inputEventClient015:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient015:previous: register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            if (unregisterResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
                const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
                expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
                console.log(`unregisterResultSecond:` + unregisterResultSecond);
            }
        }
    })
    it('inputEventClient017::multi_input_tsApi_previous_017, unregister without register', 0, function () {
        const eventHandle = (event) => {
            console.log(`inputEventClient:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);
        }

        const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
        expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
        console.log(`unregisterResult:` + unregisterResult);
    })
    it('inputEventClient018::multi_input_tsApi_previous_018, performance check', 0, function () {

        console.log(`inputEventClient::previous start`)
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_up.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:previous_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('1');
            expect(event.inputDeviceId).assertEqual('4');
            expect(event.type).assertEqual(EventCallback.ON_PREVIOUS);

            attributeAverageTimeCheck(event.uuid);
            attributeAverageTimeCheck(event.occurredTime);
            attributeAverageTimeCheck(event.sourceDevice);
            attributeAverageTimeCheck(event.inputDeviceId);
            attributeAverageTimeCheck(event.type);
            console.log(`inputEventClient:previous dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'previous', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:previous register success`);
            console.log(`inject_previous START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_previous END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'previous', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`inputEventClient::previous unregister success`);
        }
        registerAverageTimeCheck(inputEventClient.on(WINDOW_ID, 'previous', eventHandle), inputEventClient.off(WINDOW_ID, 'previous', eventHandle));
        console.log(`inputEventClient::previous_test end`);
    })
})