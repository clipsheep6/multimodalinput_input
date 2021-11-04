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

describe('tsApi_copy_test', function () {
    it('inputEventClient::multi_input_tsApi_copy_001, keyboard:ctrl+c', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_002, aiSensor', 0, function () {
        let injectEvent = {
            eventFileName: aiVoiceEvent,
            jsonEvent: '1103 1'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient:copy_dataVerify`);
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
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })

    it('inputEventClient::multi_input_tsApi_copy_003, not exist call back handler', 0, function () {
        console.log(`inputEventClient::copy start`);
        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', 32323);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient::multi_input_tsApi_copy_004, not exist register function', 0, function () {

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }
        const registerResult = inputEventClient.on(WINDOW_ID, 2423, eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
        console.log(`registerResult:` + registerResult);
    })
    it('inputEventClient::multi_input_tsApi_copy_005, register other ui standard function', 0, function () {
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_enter.json'
        }
        const eventHandle = (event) => {
            console.log(`inputEventClient:Oncopy_dataVerify`);
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
            expect(event.type).assertNotEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'send', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_send START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_send END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            const unregisterResultSecond = inputEventClient.on(WINDOW_ID, 'send', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_006, unregister not exist call back handler', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }


        const eventHandle = (event) => {

            console.log(`inputEventClient:copy_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'copy', 12321);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_INVALID_PARAMETER);
            console.log(`unregisterResultFirst:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS)
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_007, unregister not exist register function', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, '2', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
            console.log(`unregisterResult:` + unregisterResultFirst);
            const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_SUCCESS)
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_008, inject space', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: ' '
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy_dataVerify`);
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

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_009, repeat register', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResultFirst = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
            const registerResultSecond = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
            expect(registerResultSecond).assertEqual(MMI_STANDARD_EVENT_EXIST);
            console.log(`registerResultSecond:` + registerResultSecond);
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`unregisterResult:` + unregisterResult);
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_010, repeat unregister', 0, function () {

        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }

        const eventHandle = (event) => {

            console.log(`inputEventClient:copy_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResultFirst = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResultFirst).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            if (unregisterResultFirst == MMI_STANDARD_EVENT_SUCCESS) {
                const unregisterResultSecond = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
                expect(unregisterResultSecond).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
                console.log(`unregisterResultSecond:` + unregisterResultSecond);
            }
        }
    })
    it('inputEventClient::multi_input_tsApi_copy_011, unregister without register', 0, function () {
        const eventHandle = (event) => {
            console.log(`inputEventClient:copy_dataVerify`);
            console.log(`event.uuid:` + event.uuid);
            console.log(`event.occurredTime:` + event.occurredTime);
            console.log(`event.sourceDevice:` + event.sourceDevice);
            console.log(`event.inputDeviceId:` + event.inputDeviceId);
            console.log(`event.type:` + event.type);
        }

        const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
        expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_NOT_EXIST);
        console.log(`unregisterResult:` + unregisterResult);
    })
    it('inputEventClient::multi_input_tsApi_copy_012, performance check', 0, function () {

        console.log(`inputEventClient::copy: start`)
        let injectEvent = {
            eventFileName: traditionalEvent,
            jsonEvent: 'inject_event_keyboard_key_ctrl_c.json'
        }

        const eventHandle = (event) => {
            console.log(`inputEventClient:copy_dataVerify`);
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
            expect(event.type).assertEqual(EventCallback.ON_COPY);

            attributeAverageTimeCheck(event.uuid);
            attributeAverageTimeCheck(event.occurredTime);
            attributeAverageTimeCheck(event.sourceDevice);
            attributeAverageTimeCheck(event.inputDeviceId);
            attributeAverageTimeCheck(event.type);
            console.log(`inputEventClient:dataVerify end`);
        }

        const registerResult = inputEventClient.on(WINDOW_ID, 'copy', eventHandle);
        expect(registerResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
        if (registerResult == MMI_STANDARD_EVENT_SUCCESS) {
            console.log(`inputEventClient:copy: register success`);
            console.log(`inject_copy START`);
            let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
            sleep(1000);
            expect(ret).assertEqual(API_CONFIG_SUCCESS);
            console.log(`inject_copy END`);
            const unregisterResult = inputEventClient.off(WINDOW_ID, 'copy', eventHandle);
            expect(unregisterResult).assertEqual(MMI_STANDARD_EVENT_SUCCESS);
            console.log(`inputEventClient::copy: unregister success`);
        }
        registerAverageTimeCheck(inputEventClient.on(WINDOW_ID, 'copy', eventHandle), inputEventClient.off(WINDOW_ID, 'copy', eventHandle));
    })
})