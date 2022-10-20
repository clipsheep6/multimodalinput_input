#pragma once

namespace Input {

    namespace EnumUtils {
        const char* InputEventTypeToString(int32_t eventType);
        const char* InputEventCodeToString(int32_t eventType, int32_t eventCode);
        const char* InputEventSynCodeToString(int32_t eventCode);
        const char* InputEventKeyCodeToString(int32_t eventCode);
        const char* InputEventRelCodeToString(int32_t eventCode);
        const char* InputEventAbsCodeToString(int32_t eventCode);
        const char* InputEventMscCodeToString(int32_t eventCode);
        const char* InputEventSwCodeToString(int32_t eventCode);
        const char* InputEventLedCodeToString(int32_t eventCode);
        const char* InputEventSndCodeToString(int32_t eventCode);
        const char* InputEventRepCodeToString(int32_t eventCode);
        const char* InputEventFfCodeToString(int32_t eventCode);
        const char* InputEventPwrCodeToString(int32_t eventCode);
        const char* InputEventFfstatusCodeToString(int32_t eventCode);
        const char* InputPropertyToString(int32_t property);
    };

}
