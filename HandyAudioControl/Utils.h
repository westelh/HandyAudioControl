#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <string_view>
#include "windef.h"

#ifdef _DEBUG
#include "debugapi.h"
#endif // !_DEBUG

namespace HandyAudioControl {

	template<class T>
	struct COMReleaser{
		void operator()(T* pT) const noexcept {
			pT->Release();
		}
	};

	template<class T>
	using UniqueCOMPtr = std::unique_ptr<T, COMReleaser<T>>;

	// Get std::error_code with GetLastError() Win32 API
	inline std::error_code GetLastErrorCode() noexcept {
		// Existing codes are enough small for int variables
		// https://learn.microsoft.com/ja-jp/windows/win32/debug/system-error-codes--0-499-
		return std::error_code(static_cast<int>(GetLastError()), std::system_category());
	}

	// Boiler plate for hresult check
	inline void CheckHResult(HRESULT code) {
		if (!SUCCEEDED(code))
		{
			throw GetLastErrorCode();
		}
	}

	// intended to be used with classic string
	inline std::wstring WideCharFromLegacyByteArray(const std::string_view from) {
		// required size for wide char buffer
		const auto size = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from.data(), from.size(), nullptr, 0);
		std::wstring buffer(size, 0);
		// convert byte array string using system ansi code page
		const auto ret = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, from.data(), from.size(), buffer.data(), buffer.size());
		if (!ret)
		{
			throw GetLastErrorCode();
		}
		return buffer;
	}

	// utf8 array to wide chars
	inline std::wstring WideCharFromU8(const std::u8string_view from) {
		// required size for wide char buffer
		const auto size = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, reinterpret_cast<const char*>(from.data()), from.size(), nullptr, 0);
		std::wstring buffer(size, 0);
		// convert byte array string using utf8 code page
		const auto ret = MultiByteToWideChar(CP_UTF8, MB_PRECOMPOSED, reinterpret_cast<const char*>(from.data()), from.size(), buffer.data(), buffer.size());
		if (!ret)
		{
			throw GetLastErrorCode();
		}
		return buffer;
	}

	inline void OutputDebugByteString(const char* msg) {
		OutputDebugString(WideCharFromLegacyByteArray(msg).c_str());
	}
}