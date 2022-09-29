#include "PolicyConfigClient.h"
#include "IPolicyConfig.h"

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
	bool SetDefaultAudioEndPoint(PCWSTR deviceId, ERole role) {
		IPolicyConfig* pPolicyConfig = nullptr;

		const auto rCreation = CreateIPolicyConfig(&pPolicyConfig);
		if (!SUCCEEDED(rCreation))
		{
			SafeRelease(&pPolicyConfig);
			return false;
		}

		const auto rQuery = QueryIPolicyConfig(pPolicyConfig);
		if (!SUCCEEDED(rQuery))
		{
			SafeRelease(&pPolicyConfig);
		}

		pPolicyConfig->SetDefaultEndpoint(deviceId, role);
		SafeRelease(&pPolicyConfig);
		return true;
	}
}