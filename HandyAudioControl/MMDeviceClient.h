#pragma once

#include <vector>
#include "mmdeviceapi.h"
#include "Utils.h"

namespace HandyAudioControl {
	std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask);
}