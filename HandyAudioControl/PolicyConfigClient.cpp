#include "PolicyConfigClient.h"
#include "IPolicyConfig.h"
#include "Utils.h"

namespace HandyAudioControl {

	UniqueCOMPtr<IPolicyConfig> GetIPolicyConfig() {
		IPolicyConfig* pInstance = nullptr;
		const auto hr = CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pInstance));
		CheckHResult(hr);
		pInstance->QueryInterface(IID_PPV_ARGS(&pInstance));
		return UniqueCOMPtr<IPolicyConfig>{std::move(pInstance)};
	}
	
	bool SetDefaultAudioEndPoint(PCWSTR deviceId, ERole role) {
		auto pPolicyConfig = GetIPolicyConfig();
		pPolicyConfig->SetDefaultEndpoint(deviceId, role);
		return true;
	}
}