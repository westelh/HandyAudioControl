#include "pch.h"
#include "PolicyConfigClient.h"
#include "IPolicyConfig.h"
#include "Utils.h"

namespace HandyAudioControl {

	UniqueCOMPtr<IPolicyConfig> GetIPolicyConfig() {
		IPolicyConfig* pInstance = nullptr;
		CheckHResult(CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pInstance)));
		CheckHResult(pInstance->QueryInterface(IID_PPV_ARGS(&pInstance)));
		return UniqueCOMPtr<IPolicyConfig>{std::move(pInstance)};
	}
	
	bool SetDefaultAudioEndPoint(PCWSTR deviceId, ERole role) {
		auto pPolicyConfig = GetIPolicyConfig();
		pPolicyConfig->SetDefaultEndpoint(deviceId, role);
		return true;
	}
}

HandyAudioControl::PolicyConfigClient::PolicyConfigClient() : instance{ } {
	IPolicyConfig* pInstance = nullptr;
	CheckHResult(CoCreateInstance(__uuidof(CPolicyConfigClient), nullptr, CLSCTX_ALL, IID_PPV_ARGS(&pInstance)));
	CheckHResult(pInstance->QueryInterface(IID_PPV_ARGS(&pInstance)));
	instance.reset(pInstance);
}

void HandyAudioControl::PolicyConfigClient::SetDefaultAudioEndPoint(std::wstring deviceId, ERole role) {
	CheckHResult(instance->SetDefaultEndpoint(deviceId.c_str(), role));
}