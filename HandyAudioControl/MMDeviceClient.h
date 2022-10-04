#pragma once

#include <vector>
#include "framework.h"
#include "Utils.h"

namespace HandyAudioControl {
	std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask);

    std::wstring GetDeviceId(UniqueCOMPtr<IMMDevice>& device);

    UniqueCOMPtr<IPropertyStore> GetDeviceProperty(UniqueCOMPtr<IMMDevice>& device);

    std::wstring GetDeviceFriendlyName(UniqueCOMPtr<IPropertyStore>& prop);
}