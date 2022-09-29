#pragma once

#ifdef MMDEVICECLIENT_EXPORTS
#define	MMDEVICECLIENT_API __declspec(dllexport)
#else
#define MMDEVICECLIENT_API __declspec(dllimport)
#endif // MMDEVICECLIENT_EXPORTS

#include <memory>
#include <vector>
#include "mmdeviceapi.h"

class Win32ApiException;
