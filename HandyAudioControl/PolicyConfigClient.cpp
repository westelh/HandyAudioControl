#include "PolicyConfigClient.h"
#include "IPolicyConfig.h"
#include "Utils.h"

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