#include "PolicyConfigClient.h"
#include "IPolicyConfig.h"
#include "Utils.h"

namespace {
	HRESULT CreateIPolicyConfig(IPolicyConfig** ppInstance) noexcept {
		const auto hr = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, IID_PPV_ARGS(ppInstance));
		return hr;
	}

	HRESULT QueryIPolicyConfig(IPolicyConfig* pInstance) noexcept {
		if (pInstance) 
		{
			return pInstance->QueryInterface(IID_PPV_ARGS(&pInstance));
		}
		else 
		{
			return E_POINTER;
		}
	}

	template <class T> void SafeRelease(T** ppT)
	{
		if (*ppT)
		{
			(*ppT)->Release();
			*ppT = nullptr;
		}
	}
}

namespace HandyAudioControl {

	UniqueCOMPtr<IPolicyConfig> GetIPolicyConfig() {
		IPolicyConfig* pInstance;
		const auto hr = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pInstance));
		if (SUCCEEDED(hr))
		{
			if (pInstance)
			{
				pInstance->QueryInterface(IID_PPV_ARGS(&pInstance));
				return UniqueCOMPtr<IPolicyConfig>{std::move(pInstance)};
			}
			else
			{
				throw std::runtime_error{ "Even though api call has succeeded, object is null" };
			}
		}
		else
		{
			if (pInstance)
			{
				pInstance->Release();
			}
			throw GetLastErrorCode();
		}
	}
	
	bool SetDefaultAudioEndPoint(PCWSTR deviceId, ERole role) {
		auto pPolicyConfig = GetIPolicyConfig();
		pPolicyConfig->SetDefaultEndpoint(deviceId, role);
		return true;
	}
}