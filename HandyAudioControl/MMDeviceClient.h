#pragma once

#include <vector>
#include "framework.h"
#include "Utils.h"

namespace HandyAudioControl {
	std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask);
}