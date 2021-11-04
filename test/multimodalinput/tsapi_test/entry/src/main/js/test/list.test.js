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
import {beforeAll, afterAll, expect} from 'deccjsunit/index';
import {sleep, traditionalEvent, API_CONFIG_SUCCESS, WINDOW_ID} from './common.js'

beforeAll(() => {
    console.log(`inputEventClient::beforeAll start`);
    let injectEvent = {
        eventFileName: traditionalEvent,
        jsonEvent: 'start'
    }
    let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
    sleep(1000);
    expect(ret).assertEqual(API_CONFIG_SUCCESS);
    console.log(`inputEventClient::beforeAll end`);
})

//require('./showMenu.test.js');
//require('./send.test.js');
//require('./copy.test.js');
//require('./paste.test.js');
//require('./cut.test.js');
//require('./undo.test.js');
//require('./refuse.test.js');
//require('./cancel.test.js');
//require('./enter.test.js');
//require('./previous.test.js');
//require('./next.test.js');
require('./back.test.js');
//require('./print.test.js');
//require('./answer.test.js');
//require('./refresh.test.js');
//require('./hangup.test.js');
//require('./telephoneControl.test.js');
//require('./play.test.js');
//require('./pause.test.js');
//require('./mediaControl.test.js');

afterAll(() => {
    console.log(`inputEventClient::afterAll start`);
    let injectEvent = {
        eventFileName: traditionalEvent,
        jsonEvent: 'end'
    }
    let ret = inputEventClient.setInjectFile(WINDOW_ID, injectEvent);
    sleep(1000);
    expect(ret).assertEqual(API_CONFIG_SUCCESS);
    console.log(`inputEventClient::afterAll end`);
})