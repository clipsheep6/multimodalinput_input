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

describe('tsApi_back_test', function () {

    it('inputEventClient::multi_input_tsApi_back_001: mouse', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_mouse_middle_click.json'
        }

        const eventHandle = (event) => {
            var currentTime = new Date().getTime();
            console.log(`currentTime:` + currentTime);

            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back_mouse: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_002, keyboard:"Esc', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_003, touchPad', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_touchPad_three_finger_move_to_left.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);

            expect(typeof (event.uuid) == 'string').assertEqual(true);
            expect(event.uuid.length == 36).assertEqual(true);
            expect(event.occurredTime).assertEqual(event.occurredTime);
            expect(event.sourceDevice).assertEqual('9');
            expect(event.inputDeviceId).assertEqual('16437');
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }
        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(4000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_004, remoteControl', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_remoteControl_key_back_158.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_005, aiSensor', 0, function () {
        let injectEvent = {
            eventFileName: aiVoiceEvent,
            jsonEvent: '1113 1'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_006, not exist call back handler', 0, function () {
        console.log(`inputEventClient::back: start`);
        const registerResult = inputEventClient.on(WINDOW_ID, 'back', 32323);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient::multi_input_tsApi_back_007, not exist register function', 0, function () {
        const eventHandle = (event) => {
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 2423, eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient::multi_input_tsApi_back_008, register other ui standard function', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_down.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient002:next_dataVerify`);
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
            expect(event.type).assertNotEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient002:dataVerify end`);
        }
        const registerResult = inputEventClient.on(WINDOW_ID, 'next', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'next', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_009, unregister not exist call back handler', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'back', 12321);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
            console.log(`unregisterResultFirst:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_010, unregister not exist register function', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, '12', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            console.log(`unregisterResultFirst:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_011, inject space', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: ' '
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:back_dataVerify`);
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
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_012, repeat register', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResultFirst = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
            const registerResultSecond = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
            expect(registerResultSecond).assertEqual(MMI_STANDARD_EVENT_EXIST);
            console.log(`registerResultSecond:` + registerResultSecond);
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_back_013, repeat unregister', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            if (unregisterResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
                const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
                expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
                console.log(`unregisterResultSecond:` + unregisterResultSecond);
            }
        }
    })
    it('inputEventClient::multi_input_tsApi_back_014, unregister without register', 0, function () {
        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);
        }

        const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
        expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
        console.log(`unregisterResult:` + unregisterResult);
    })
    it('inputEventClient::multi_input_tsApi_back_015, performance check', 0, function () {

        console.log(`inputEventClient::back_test: start`)
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_Esc.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:back_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_BACK);

            attributeAverageTimeCheck(event.uuid);
            attributeAverageTimeCheck(event.occurredTime);
            attributeAverageTimeCheck(event.sourceDevice);
            attributeAverageTimeCheck(event.inputDeviceId);
            attributeAverageTimeCheck(event.type);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'back', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:back: register success`);
            console.log(`inject_back START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(2000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_back END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'back', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`inputEventClient::back_test: unregister success`);
        }
        registerAverageTimeCheck(inputEventClient.on(WINDOW_ID, 'showMenu', eventHandle), inputEventClient.off(WINDOW_ID, 'showMenu', eventHandle));
        console.log(`inputEventClient::back_test: end`);
    })
})