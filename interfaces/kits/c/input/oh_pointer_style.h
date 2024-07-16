/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OH_POINTER_STYLE_H
#define OH_POINTER_STYLE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    /**
     * Default
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    DEFAULT,

    /**
     * East arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    EAST,

    /**
     * West arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    WEST,

    /**
     * South arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    SOUTH,

    /**
     * North arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH,

    /**
     * East-west arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    WEST_EAST,

    /**
     * North-south arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH_SOUTH,

    /**
     * North-east arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH_EAST,

    /**
     * North-west arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH_WEST,

    /**
     * South-east arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    SOUTH_EAST,

    /**
     * South-west arrow
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    SOUTH_WEST,

    /**
     * Northeast and southwest adjustment
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH_EAST_SOUTH_WEST,

    /**
     * Northwest and southeast adjustment
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    NORTH_WEST_SOUTH_EAST,

    /**
     * Cross (accurate selection)
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    CROSS,

    /**
     * Copy
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    CURSOR_COPY,

    /**
     * Forbid
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    CURSOR_FORBID,

    /**
     * Sucker
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    COLOR_SUCKER,

    /**
     * Grabbing hand
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    HAND_GRABBING,

    /**
     * Opening hand
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    HAND_OPEN,

    /**
     * Hand-shaped pointer
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    HAND_POINTING,

    /**
     * Help
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    HELP,

    /**
     * Move
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MOVE,

    /**
     * Left and right resizing
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    RESIZE_LEFT_RIGHT,

    /**
     * Up and down resizing
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    RESIZE_UP_DOWN,

    /**
     * Screenshot crosshair
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    SCREENSHOT_CHOOSE,

    /**
     * Screenshot
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    SCREENSHOT_CURSOR,

    /**
     * Text selection
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    TEXT_CURSOR,

    /**
     * Zoom in
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    ZOOM_IN,

    /**
     * Zoom out
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    ZOOM_OUT,

    /**
     * Scrolling east
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_EAST,

    /**
     * Scrolling west
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_WEST,

    /**
     * Scrolling south
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_SOUTH,

    /**
     * Scrolling north
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_NORTH,

    /**
     * Scrolling north and south
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_NORTH_SOUTH,

    /**
     * Scrolling northeast
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_NORTH_EAST,

    /**
     * Scrolling northwest
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_NORTH_WEST,

    /**
     * Scrolling southeast
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_SOUTH_EAST,

    /**
     * Scrolling southwest
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_SOUTH_WEST,

    /**
     * Moving as a cone in four directions
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    MIDDLE_BTN_NORTH_SOUTH_WEST_EAST,

    /**
     * Horizontal text selection
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    HORIZONTAL_TEXT_CURSOR,

    /**
     * Precise selection
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    CURSOR_CROSS,

    /**
     * Cursor with circle style
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    CURSOR_CIRCLE,

    /**
     * Loading state with dynamic cursor
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    /**
     * Loading state with dynamic cursor
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @atomicservice
     * @since 12
     */
    LOADING,

    /**
     * Running state with dynamic cursor
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @since 12
     */
    /**
     * Running state with dynamic cursor
     *
     * @syscap SystemCapability.MultimodalInput.Input.Core
     * @atomicservice
     * @since 12
     */
    RUNNING
} Input_PointerStyle;

#ifdef __cplusplus
}
#endif
/** @} */
#endif