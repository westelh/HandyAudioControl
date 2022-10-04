#include <stdexcept>
#include "MMDeviceClient.h"

namespace HandyAudioControl {

    UniqueCOMPtr<IMMDeviceEnumerator> GetIMMDeviceEnumerator() {
        IMMDeviceEnumerator* pEnumerator = nullptr;
        constexpr CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
        constexpr IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
        const auto hr = CoCreateInstance(
            CLSID_MMDeviceEnumerator, NULL,
            CLSCTX_ALL, IID_PPV_ARGS(&pEnumerator));
        
        CheckHResult(hr);
        pEnumerator->QueryInterface(IID_PPV_ARGS(&pEnumerator));
        return UniqueCOMPtr<IMMDeviceEnumerator>{std::move(pEnumerator)};
    }

    UniqueCOMPtr<IMMDeviceCollection> GetIMMDeviceCollection(UniqueCOMPtr<IMMDeviceEnumerator>& enumerator, EDataFlow dataFlow, DWORD dwStateMask) {
        IMMDeviceCollection* pDeviceCollection = nullptr;
        CheckHResult(enumerator->EnumAudioEndpoints(dataFlow, dwStateMask, &pDeviceCollection));
        return  UniqueCOMPtr<IMMDeviceCollection>{ std::move(pDeviceCollection) };
    }

    UINT GetIMMDeviceCount(UniqueCOMPtr<IMMDeviceCollection>& collection) {
        UINT count = 0;
        CheckHResult(collection->GetCount(&count));
        return count;
    }

    UniqueCOMPtr<IMMDevice> GetIMMDevice(UniqueCOMPtr<IMMDeviceCollection>& collection, UINT i) {
        IMMDevice* pDevice = nullptr;
        CheckHResult(collection->Item(i, &pDevice));
        return UniqueCOMPtr<IMMDevice>{std::move(pDevice)};
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

    std::wstring GetDeviceId(UniqueCOMPtr<IMMDevice>& device) {
        LPWSTR id = nullptr;
        CheckHResult(device->GetId(&id));
        std::wstring ret{ id };
        CoTaskMemFree(id);
        return id;
    }

    UniqueCOMPtr<IPropertyStore> GetDeviceProperty(UniqueCOMPtr<IMMDevice>& device) {
        IPropertyStore* prop = nullptr;
        CheckHResult(device->OpenPropertyStore(STGM_READ, &prop));
        return UniqueCOMPtr<IPropertyStore>{std::move(prop)};
    }

    std::wstring GetDeviceFriendlyName(UniqueCOMPtr<IPropertyStore>& prop) {
        if (prop)
        {
            PROPVARIANT varName;
            PropVariantInit(&varName);
            CheckHResult(prop->GetValue(PKEY_Device_FriendlyName, &varName));
            const auto name = std::wstring{ varName.pwszVal };
            PropVariantClear(&varName);
            return name;
        }
        else
        {
            throw std::invalid_argument{ "passed pointer is null" };
        }
    }
}

HandyAudioControl::MMDevice::MMDevice(UniqueCOMPtr<IMMDevice>&& p) : pDevice{ std::forward<UniqueCOMPtr<IMMDevice>>(p) }, pProperty{ } {
    IPropertyStore* prop = nullptr;
    CheckHResult(pDevice->OpenPropertyStore(STGM_READ, &prop));
    pProperty.reset(std::move(prop));
}

std::wstring HandyAudioControl::MMDevice::GetId() const {
    LPWSTR tempId;
    CheckHResult(this->pDevice->GetId(&tempId));
    std::wstring ret{ tempId };
    CoTaskMemFree(tempId);
    return ret;
}

std::wstring HandyAudioControl::MMDevice::GetFriendlyName() const {
    PROPVARIANT varName;
    PropVariantInit(&varName);
    CheckHResult(pProperty->GetValue(PKEY_Device_FriendlyName, &varName));
    const auto name = std::wstring{ varName.pwszVal };
    PropVariantClear(&varName);
    return name;
}
