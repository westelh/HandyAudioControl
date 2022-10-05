#pragma once

#include <vector>
#include "framework.h"
#include "Utils.h"

namespace HandyAudioControl {
	std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask);

    std::wstring GetDeviceId(UniqueCOMPtr<IMMDevice>& device);

    UniqueCOMPtr<IPropertyStore> GetDeviceProperty(UniqueCOMPtr<IMMDevice>& device);

    std::wstring GetDeviceFriendlyName(UniqueCOMPtr<IPropertyStore>& prop);

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