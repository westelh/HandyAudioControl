#pragma once

#ifdef POLICYCONFIGCLIENT_EXPORTS
#define	POLICYCONFIGCLIENT_API __declspec(dllexport)
#else
#define POLICYCONFIGCLIENT_API __declspec(dllimport)
#endif // POLICYCONFIGCLIENT_EXPORTS

#include "mmdeviceapi.h"

namespace HandyAudioControl {
	bool SetDefaultAudioEndPoint(PCWSTR deviceId, ERole role);
}