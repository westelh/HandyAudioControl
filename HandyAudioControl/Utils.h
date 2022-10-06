#pragma once

#include <string>
#include <memory>
#include <system_error>
#include <string_view>

#ifdef _DEBUG
#include "debugapi.h"
#endif // !_DEBUG

inline std::wstring UnicodeInWideChars(std::string_view from) {
	std::wstring ret(from.size(), 0);
	size_t charCount;
	mbstowcs_s(&charCount, ret.data(), ret.size(), from.data(), _TRUNCATE);
	return ret;
}

inline void OutputDebugString(const std::exception& e) noexcept {
#ifdef UNICODE
#endif // UNICODE

	
	return;
}



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
}