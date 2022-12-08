/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#include "input_display_bind_helper.h"

#include <list>
#include <iostream>
#include <sstream>
#include <fstream>

#include "mmi_log.h"

namespace OHOS {
namespace MMI {
class BindInfo
{
public:
	int32_t GetInputDeviceId() const { return inputDeviceId_; }
	std::string GetInputDeviceName() const { return inputDeviceName_; }
	int32_t GetDisplayId() const { return displayId_; }
	std::string GetDisplayName() const { return displayName_; }
	bool IsUnbind() const {	return ((inputDeviceId_ == -1) || (displayId_ == -1)); }
	bool InputDeviceNotBind() const { return (inputDeviceId_ == -1);	}
	bool DisplayNotBind() const { return (displayId_ == -1); }
	bool AddInputDevice(int32_t deviceId, const std::string &deviceName);
	void RemoveInputDevice();
	bool AddDisplay(int32_t id, const std::string &name);
	void RemoveDisplay();
	std::string GetDesc() const;
	friend bool operator<(const BindInfo &l, const BindInfo &r);
	friend std::ostream& operator<<(std::ostream &os, const BindInfo &r);
	friend std::istream& operator>>(std::istream &is, BindInfo &r);
private:
	int32_t inputDeviceId_ { -1 };
	std::string inputDeviceName_;
	int32_t displayId_ { -1 };
	std::string displayName_;
};

class BindInfos {
public:
	bool Add(const BindInfo &info);
	void UnbindInputDevice(int32_t deviceId);
	void UnbindDisplay(int32_t displayId);
	BindInfo GetUnbindInputDevice(const std::string &displayName);
	BindInfo GetUnbindDisplay(const std::string &inputDeviceName);    
	std::string GetDisplayNameByInputDevice(const std::string &name);
	std::string GetInputDeviceByDisplayName(const std::string &name);
    std::string GetDesc() const;
	const std::list<BindInfo> &GetInfos() const { return infos_; }
	friend std::ostream& operator<<(std::ostream &os, const BindInfos &r);
	friend std::istream& operator>>(std::istream &is, BindInfos &r);
private:
    BindInfo GetUnbindInputDevice();
    BindInfo GetUnbindInfo();
    BindInfo GetUnbindDisplay();
    std::list<BindInfo> infos_;
};

bool BindInfo::AddInputDevice(int32_t deviceId, const std::string &deviceName)
{
    if ((inputDeviceId_ != -1) || !inputDeviceName_.empty()) {
        return false;
    }
    inputDeviceId_ = deviceId;
    inputDeviceName_ = deviceName;
    return true;
}

void BindInfo::RemoveInputDevice() {
    inputDeviceId_ = -1;
    inputDeviceName_.clear();
}
bool BindInfo::AddDisplay(int32_t id, const std::string &name) {
    if ((displayId_ != -1) || !displayName_.empty()) {
        return false;
    }
    displayId_ = id;
    displayName_ = name;
    return true;
}
void BindInfo::RemoveDisplay() {
    displayId_ = -1;
    displayName_.clear();
}
std::string BindInfo::GetDesc() const {
    std::ostringstream oss;
    oss << "InputDevice(id:" << inputDeviceId_ << ",name:" << inputDeviceName_ << "),Display(id:" << displayId_ << ",name:" << displayName_ << ")";
    return oss.str();
}

bool operator<(const BindInfo &l, const BindInfo &r) {
	if (l.inputDeviceId_ < r.inputDeviceId_) {
		return true;
	}
	return (l.displayId_ < r.displayId_);	
}

std::ostream& operator<<(std::ostream &os, const BindInfo &r) {
	os << r.inputDeviceName_ << "<=>" << r.displayName_ << std::endl;
	return os;
}

std::istream& operator>>(std::istream &is, BindInfo &r) {
	std::string line;
	std::getline(is, line);
	const std::string delim = "<=>";
	std::string::size_type pos = line.find(delim);
	if (pos == std::string::npos) {
		return is;
	}
	r.inputDeviceName_ = line.substr(0, pos);
	r.displayName_ = line.substr(pos + delim.length());
	r.inputDeviceId_ = 0;
	r.displayId_ = 0;
	return is;
}

std::string BindInfos::GetDesc() const {
	int32_t index = 0;
	std::ostringstream oss;
	for (auto &info : infos_) {
		oss << "index:" << index << "," << info.GetDesc() << std::endl;
	}
	return oss.str();
}

std::string BindInfos::GetBindDisplayNameByInputDevice(int32_t inputDeviceId) const {
	for (const auto &item : infos_) {
		if (item.GetInputDeviceId() == inputDeviceId) {
			if (!item.IsUnbind()) {
				return item.GetDisplayName();
			}
		}
	}
	return {};
}

std::string BindInfos::GetDisplayNameByInputDevice(const std::string &name) {
	for (const auto &item : infos_) {
		if (item.GetInputDeviceName() == name) {
			return item.GetDisplayName();
		}
	}
	return "";
}

std::string BindInfos::GetInputDeviceByDisplayName(const std::string &name) {
	for (const auto &item : infos_) {
		if (item.GetDisplayName() == name) {
			return item.GetInputDeviceName();
		}
	}
	return "";
}

bool BindInfos::Add(const BindInfo &info) {
	auto it = infos_.begin();
	for (; it != infos_.end(); ++it) {
		if (info < *it) {
			break;
		}
	}
	auto it2 = infos_.emplace(it, std::move(info));
	if (it2 == infos_.end()) {
		printf("duplicate %s\n", info.GetDesc().c_str());
	}
	return true;
}

void BindInfos::UnbindInputDevice(int32_t deviceId) {
	auto it = infos_.begin();
	for (; it != infos_.end(); ++it) {
		if (it->GetInputDeviceId() == deviceId) {
			it->RemoveInputDevice();
			if (it->IsUnbind()) {
				infos_.erase(it);
			}
			return;
		}
	} 
}

void BindInfos::UnbindDisplay(int32_t displayId) {
	auto it = infos_.begin();
	for (; it != infos_.end(); ++it) {
		if (it->GetDisplayId() == displayId) {
			it->RemoveDisplay();
			if (it->IsUnbind()) {
				infos_.erase(it);
			}
			return;
		}
	} 
}

BindInfo BindInfos::GetUnbindInfo() {
	auto it = infos_.begin();
	while (it != infos_.end()) {
		if (it->IsUnbind()) {
			auto info = std::move(*it);
			infos_.erase(it);
			return info;
		}
		++it;
	}
	return BindInfo();
}

BindInfo BindInfos::GetUnbindInputDevice() {
	auto it = infos_.begin();
	while (it != infos_.end()) {
		if (it->InputDeviceNotBind()) {
			auto info = std::move(*it);
			infos_.erase(it);
			return info;
		}
		++it;
	}
	return BindInfo();
}

BindInfo BindInfos::GetUnbindInputDevice(const std::string &displayName) {
	auto it = infos_.begin();
	while (it != infos_.end()) {
		if (it->InputDeviceNotBind()) {
			if (it->GetDisplayName() == displayName) {
				auto info = std::move(*it);
				infos_.erase(it);
				return info;
			}
		}
		++it;
	}
	return GetUnbindInputDevice();
}

BindInfo BindInfos::GetUnbindDisplay() {
	auto it = infos_.begin();
	while (it != infos_.end()) {
		if (it->DisplayNotBind()) {
			auto info = std::move(*it);
			infos_.erase(it);
			return info;
		}
		++it;
	}
	return BindInfo();
}

BindInfo BindInfos::GetUnbindDisplay(const std::string &inputDeviceName) {
	auto it = infos_.begin();
	while (it != infos_.end()) {
		if (it->DisplayNotBind()) {
			if (it->GetInputDeviceName() == inputDeviceName) {
				auto info = std::move(*it);
				infos_.erase(it);
				return info;
			}
		}
		++it;
	}
	return GetUnbindDisplay();
}

std::ostream& operator<<(std::ostream &os, const BindInfos &r) {
	const auto &infos = r.GetInfos();
	for (const auto &info : infos) {
		if (!info.IsUnbind()) {
			os << info;
		}
	}
	return os;
}

std::istream& operator>>(std::istream &is, BindInfos &r) {
	int32_t index = -1;
	while (!is.eof()) {
		BindInfo info;
		is >> info;
		if (info.IsUnbind()) {
			break;
		}
		r.Add(info);
	}
	return is;
}

InputDisplayBindHelper::InputDisplayBindHelper(const std::string bindCfgFile)
	: fileName_(bindCfgFile),
	infos_(std::make_shared<BindInfos>()),
	configFileInfos_(std::make_shared<BindInfos>())
{}

std::string InputDisplayBindHelper::GetDiplayNameByInputDeviceId(int32_t inputDeviceId) const
{
	if (infos_ == nullptr) {
		return {};
	}
	return infos_->GetDisplayNameByInputDeviceId(inputDeviceId);
}

void InputDisplayBindHelper::AddInputDevice(int32_t id, const std::string &name) {
	auto displayName = configFileInfos_->GetDisplayNameByInputDevice(name);
	BindInfo info = infos_->GetUnbindInputDevice(displayName);
	info.AddInputDevice(id, name);
	infos_->Add(info);
	Store();
}

void InputDisplayBindHelper::RemoveInputDevice(int32_t id) {
	infos_->UnbindInputDevice(id);
}

bool InputDisplayBindHelper::IsDisplayAdd(int32_t id, const std::string &name)
{
	const auto &infos = infos_->GetInfos();
	for (const auto &info : infos)	 {
		if ((info.GetDisplayName() == name) && (info.GetDisplayId() == id)) {
			return true;
		}
	}
	return false;
}

std::set<std::pair<int32_t, std::string>> InputDisplayBindHelper::GetDisplayIdNames() const
{
	std::set<std::pair<int32_t, std::string>> idNames;
	const auto &infos = infos_->GetInfos();
	for (const auto &info : infos)	 {
		if (info.GetDisplayId() != -1) {
			idNames.insert(std::make_pair(info.GetDisplayId(), info.GetDisplayName()));
		}
	}
	return idNames;
}

void InputDisplayBindHelper::AddDisplay(int32_t id, const std::string &name) {
	auto inputDeviceName = configFileInfos_->GetInputDeviceByDisplayName(name);
	BindInfo info = infos_->GetUnbindDisplay(inputDeviceName);
	info.AddDisplay(id, name);
	infos_->Add(info);
	Store();
}

void InputDisplayBindHelper::RemoveDisplay(int32_t id) {
	infos_->UnbindDisplay(id);
}

void InputDisplayBindHelper::Store()
{
	std::ofstream ofs(fileName_.c_str()); 
	if (!ofs) {
		printf("open file fail.%s\n", fileName_.c_str());
		return;
	}
	ofs << infos_;
	ofs.close();
}

void InputDisplayBindHelper::Load()
{
	std::ifstream ifs(fileName_.c_str()); 
	if (!ifs) {
		printf("open file fail.%s\n", fileName_.c_str());
		return;
	}
	ifs >> *configFileInfos_;
	ifs.close();
}

std::string InputDisplayBindHelper::Dumps() const
{
	if (infos_ == nullptr) {
		return {};
	}
	std::ostringstream oss;
	oss << *infos_;
	return oss.str();
}
} //namespace MMI
} // namespace OHOS