#pragma once

#include "pch.h"

class DesktopWindow {
private:
    wil::unique_hwnd hwnd;
    winrt::guid m_lastFocusRequestId;
    bool FilterMessage(const MSG* msg);
    std::vector<winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource::TakeFocusRequested_revoker> m_takeFocusEventRevokers;
    std::vector<winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource> m_xamlSources;
    void OnTakeFocusRequested(winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource const& sender, winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args);
    bool NavigateFocus(MSG* msg);
    winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource GetFocusedIsland();
    winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource GetNextFocusedIsland(const MSG* msg);

protected:

    HWND GetHandle();

	// WM_NCCCREATE におけるウィンドウハンドルの関連付け
    static void OnNCCreate(HWND window, LPARAM lparam) noexcept {
        const auto cs = reinterpret_cast<CREATESTRUCT*>(lparam);	// params for the new window
        const auto that = static_cast<DesktopWindow*>(cs->lpCreateParams);
        that->hwnd.reset(window); // take ownership of the window
        // ウィンドウがクラスを指定
        const auto result = SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(that));
        // THROW_LAST_ERROR_IF(result == 0);
    }

    HWND CreateDesktopWindowsXamlSource(DWORD extraStyles, const winrt::Windows::UI::Xaml::UIElement& content);

    void ClearXamlIslands();

public:
    int MessageLoop(HACCEL accelerators);
};

template<class T>
class DesktopWindowT: public DesktopWindow {
protected:
	using base_type = DesktopWindowT<T>;

    static LRESULT __stdcall WndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) noexcept
    {
        if (message == WM_NCCREATE)
        {
            OnNCCreate(window, lparam);
        }
        else if (message == WM_NCDESTROY)
        {
            // DestopWindowとの関連付けを解除
            SetWindowLongPtrW(window, GWLP_USERDATA, 0);
        }
        else if (auto that = reinterpret_cast<T*>(GetWindowLongPtrW(window, GWLP_USERDATA)))
        {
            return that->MessageHandler(message, wparam, lparam);
        }

        return DefWindowProcW(window, message, wparam, lparam);
    }

    LRESULT MessageHandler(UINT message, WPARAM wParam, LPARAM lParam) noexcept
    {
        switch (message)
        {
            HANDLE_MSG(GetHandle(), WM_DESTROY, OnDestroy);
            HANDLE_MSG(GetHandle(), WM_ACTIVATE, OnActivate);
            HANDLE_MSG(GetHandle(), WM_SETFOCUS, OnSetFocus);
        }
        return DefWindowProcW(GetHandle(), message, wParam, lParam);
    }

    void OnDestroy(HWND)
    {
        ClearXamlIslands();
        PostQuitMessage(0);
    }

private:

    void OnActivate(HWND, UINT state, HWND hwndActDeact, BOOL fMinimized)
    {
        if (state == WA_INACTIVE)
        {
            //m_hwndLastFocus = GetFocus();
        }
    }

    void OnSetFocus(HWND, HWND hwndOldFocus)
    {
        //if (m_hwndLastFocus)
        //{
        //    SetFocus(m_hwndLastFocus);
        //}
    }
};
