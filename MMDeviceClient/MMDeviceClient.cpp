#include "pch.h"
#include <limits>
#include <system_error>
#include "mmdeviceapi.h"
#include "MMDeviceClient.h"

// Get std::error_code with GetLastError() Win32 API
inline std::error_code GetLastErrorCode() noexcept {
    // Existing codes are enough small for int variables
    // https://learn.microsoft.com/ja-jp/windows/win32/debug/system-error-codes--0-499-
    return std::error_code(static_cast<int>(GetLastError()), std::system_category());
}

// Custom Deleter for COM Objects
template<class T>
struct COMRelease {
    void operator()(T* pT) {
        pT->Release();
    }
};

template<class T>
using UniqueCOMPtr = std::unique_ptr<T, COMRelease<T>>;

UniqueCOMPtr<IMMDeviceEnumerator> GetIMMDeviceEnumerator() {
    IMMDeviceEnumerator* pEnumerator = nullptr;
    constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
    constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator); 
    const auto hr = CoCreateInstance(
        CLSID_MMDeviceEnumerator, NULL,
        CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
    if (SUCCEEDED(hr))
    {
        if (pEnumerator)
        {
            pEnumerator->QueryInterface(IID_PPV_ARGS(&pEnumerator));
            return UniqueCOMPtr<IMMDeviceEnumerator>{std::move(pEnumerator)};
        }
        else
        {
            throw std::runtime_error{ "Even though api call has succeeded, object is null" };
        }
    }
    else
    {
        if (pEnumerator)
        {
            pEnumerator->Release();
        }
        throw GetLastErrorCode();
    }
}

UniqueCOMPtr<IMMDeviceCollection> GetIMMDeviceCollection(UniqueCOMPtr<IMMDeviceEnumerator>& enumerator, EDataFlow dataFlow, DWORD dwStateMask) {
    IMMDeviceCollection* pDeviceCollection = nullptr;
    const auto hr = enumerator->EnumAudioEndpoints(dataFlow, dwStateMask, &pDeviceCollection);
    if (SUCCEEDED(hr))
    {
        if (pDeviceCollection)
        {
            return UniqueCOMPtr<IMMDeviceCollection>{std::move(pDeviceCollection)};
        }
        else
        {
            throw std::runtime_error{ "Even though api call has succeeded, object is null" };
        }
    }
    else
    {
        if (pDeviceCollection)
        {
            pDeviceCollection->Release();
        }
        throw GetLastErrorCode();
    }
}

UINT GetIMMDeviceCount(UniqueCOMPtr<IMMDeviceCollection>& collection) {
    UINT count = 0;
    const auto hr = collection->GetCount(&count);
    if (SUCCEEDED(hr))
    {
        return count;
    }
    else
    {
        GetLastErrorCode();
    }
}

UniqueCOMPtr<IMMDevice> GetIMMDevice(UniqueCOMPtr<IMMDeviceCollection>& collection, UINT i) {
    IMMDevice* pDevice;
    const auto hr = collection->Item(i, &pDevice);
    if (SUCCEEDED(hr))
    {
        if (pDevice)
        {
            return UniqueCOMPtr<IMMDevice>{std::move(pDevice)};
        }
        else
        {
            throw std::runtime_error{ "Even though api call has succeeded, object is null" };
        }
    }
    else
    {
        if (pDevice)
        {
            pDevice->Release();
        }
        GetLastErrorCode();
    }
}

std::vector<UniqueCOMPtr<IMMDevice>> EnumerateAudioDevices(EDataFlow dataFlow, DWORD dwStateMask) {
    std::vector<UniqueCOMPtr<IMMDevice>> ret;
    auto enumerator = GetIMMDeviceEnumerator();
    auto collection = GetIMMDeviceCollection(enumerator, dataFlow, dwStateMask);
    auto n = GetIMMDeviceCount(collection);
    for (size_t i = 0; i < n; i++)
    {
        auto pDevice = GetIMMDevice(collection, i);
        ret.push_back(std::move(pDevice));
    }
    return ret;
}