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

#ifndef AUDIO_POLICY_MANAGER_STUB_H
#define AUDIO_POLICY_MANAGER_STUB_H

#include "audio_policy_base.h"

namespace OHOS {
namespace AudioStandard {
class AudioPolicyManagerStub : public IRemoteStub<IAudioPolicy> {
public:
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel &data,
                                MessageParcel &reply, MessageOption &option) override;
    bool IsPermissionValid();

private:
    void SetStreamVolumeInternal(MessageParcel &data, MessageParcel &reply);
    void SetRingerModeInternal(MessageParcel &data, MessageParcel &reply);
    void GetRingerModeInternal(MessageParcel &data);
    void GetStreamVolumeInternal(MessageParcel &data, MessageParcel &reply);
    void SetStreamMuteInternal(MessageParcel &data, MessageParcel &reply);
    void GetStreamMuteInternal(MessageParcel &data, MessageParcel &reply);
    void IsStreamActiveInternal(MessageParcel &data, MessageParcel &reply);
    void SetDeviceActiveInternal(MessageParcel &data, MessageParcel &reply);
    void IsDeviceActiveInternal(MessageParcel &data, MessageParcel &reply);
    void SetRingerModeCallbackInternal(MessageParcel &data, MessageParcel &reply);
    void UnsetRingerModeCallbackInternal(MessageParcel &data, MessageParcel &reply);
    void SetInterruptCallbackInternal(MessageParcel &data, MessageParcel &reply);
    void UnsetInterruptCallbackInternal(MessageParcel &data, MessageParcel &reply);
    void ActivateInterruptInternal(MessageParcel &data, MessageParcel &reply);
    void DeactivateInterruptInternal(MessageParcel &data, MessageParcel &reply);
    void GetStreamInFocusInternal(MessageParcel &reply);
    void ReadAudioInterruptParams(MessageParcel &data, AudioInterrupt &audioInterrupt);
    void SetVolumeKeyEventCallbackInternal(MessageParcel &data, MessageParcel &reply);
};
} // AudioStandard
} // namespace OHOS
#endif // AUDIO_POLICY_MANAGER_STUB_H
