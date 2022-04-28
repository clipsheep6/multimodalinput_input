/*
 * @Descripttion: 
 * @version: 
 * @Author: sueRimn
 * @Date: 2022-02-14 14:14:32
 * @LastEditors: sueRimn
 * @LastEditTime: 2022-02-14 14:41:03
 */
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

import { Callback } from './basic';

declare namespace inputDInput {

  export interface  DMouseLocation{
    globleX: number;
    globleY: number;
    dx: number;
    dy: number;
    displayId: number;
    logicalDisplayWidth: number;
    logicalDisplayHeight: number;
    logicalDisplayTopLeftX: number;
    logicalDisplayTopLeftY: number;

}
  /**
   * 隐藏鼠标
   * @param callback
   */
  function hideMouse(callback: AsyncCallback<boolean>): void;
  /**
   * 显示鼠标
   * @param callback
   */
  function showMouse(callback: AsyncCallback<boolean>): void;
  /**
   * 准备分布式
   * @param callback
   */
  function prepareRemoteInput(deviceId: string, callback: AsyncCallback<boolean>): void;
  /**
   * 取消准备分布式
   * @param callback
   */
  function unprepareRemoteInput(deviceId: string, callback: AsyncCallback<boolean>): void;
  /**
   * 获取设备列表
   * @param callback
   */
  function getVirtualDeviceIdListAsync(callback: AsyncCallback<Array<number>>): void;

  /**
   * 获取鼠标信息
   * @param callback
   */
  function getMouseLocation(callback: AsyncCallback<DMouseLocation>): void;
  /**
   * 开始分布式
   * @param callback
   */
  function startRemoteInput(deviceId: string, callback: AsyncCallback<boolean>): void;
  /**
   * 停止分布式
   * @param callback
   */
  function stopRemoteInput(deviceId: string, callback: AsyncCallback<boolean>): void;
  /**
   * 鼠标信息注入
   * @param callback
   */
  function simulateCrossLocation(mouseX: number, mouseY: number, callback: AsyncCallback<number> ): void;

}

export default inputDInput;