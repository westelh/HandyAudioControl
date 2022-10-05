#pragma once

#include <vector>
#include "framework.h"
#include "Utils.h"

namespace HandyAudioControl {

	class MMDevice {
	public:
		static std::vector<MMDevice> Enumerate(EDataFlow dataFlow, DWORD dwStateMask);

		explicit MMDevice(UniqueCOMPtr<IMMDevice>&& p);

		MMDevice(MMDevice&& rhs) noexcept;

		~MMDevice() noexcept = default;

		std::wstring GetId() const;

		std::wstring GetFriendlyName() const;

	private:
		UniqueCOMPtr<IMMDevice> pDevice;

		UniqueCOMPtr<IPropertyStore> pProperty;
	};

}