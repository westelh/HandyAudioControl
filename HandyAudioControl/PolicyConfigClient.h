#pragma once

#ifdef POLICYCONFIGCLIENT_EXPORTS
#define	POLICYCONFIGCLIENT_API __declspec(dllexport)
#else
#define POLICYCONFIGCLIENT_API __declspec(dllimport)
#endif // POLICYCONFIGCLIENT_EXPORTS

#include "framework.h"
#include "IPolicyConfig.h"
#include "Utils.h"

namespace HandyAudioControl {

	class PolicyConfigClient {
	private:
		UniqueCOMPtr<IPolicyConfig> instance;

	public:
		PolicyConfigClient();

		~PolicyConfigClient() noexcept = default;

		void SetDefaultAudioEndPoint(std::wstring deviceId, ERole role);
	};

}