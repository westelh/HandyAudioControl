#pragma once

#include <vector>
#include "mmdeviceapi.h"
#include "Utils.h"

namespace HandyAudioControl {

	UniqueCOMPtr<IMMDeviceEnumerator> GetIMMDeviceEnumerator();

	UniqueCOMPtr<IMMDeviceCollection> GetIMMDeviceCollection(UniqueCOMPtr<IMMDeviceEnumerator>& enumerator, EDataFlow dataFlow, DWORD dwStateMask);

	UINT GetIMMDeviceCount(UniqueCOMPtr<IMMDeviceCollection>& collection);

	UniqueCOMPtr<IMMDevice> GetIMMDevice(UniqueCOMPtr<IMMDeviceCollection>& collection, UINT i);

	std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask);
}