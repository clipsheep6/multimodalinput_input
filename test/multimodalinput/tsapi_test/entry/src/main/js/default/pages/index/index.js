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

import file from '@system.file'
import {Core, ExpectExtend, ReportExtend, InstrumentLog} from 'deccjsunit/index';
//const core = Core.getInstance()
//core.init()
//require('../../../test/list.test')
//core.execute()
export default {
    data: {
        title: ""
    },
    onInit() {
        this.title = this.$t('strings.test');
    },
    onShow() {
        console.info('onShow finish')
        const core = Core.getInstance()
        const expectExtend = new ExpectExtend({
            'id': 'extend'
        })
        core.addService('expect', expectExtend)
        core.init()
        const configService = core.getDefaultService('config')
        configService.setConfig(this)
        require('../../../test/list.test')
        core.execute()
    },
    onReady() {
    },
}