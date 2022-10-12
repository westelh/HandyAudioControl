#include "Window.h"

namespace {
    WPARAM GetKeyFromReason(winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason reason)
    {
        static const WPARAM invalidKey = (WPARAM)-1;
        auto key = invalidKey;
        if (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Last || reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First)
        {
            key = VK_TAB;
        }
        else if (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Left)
        {
            key = VK_LEFT;
        }
        else if (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Right)
        {
            key = VK_RIGHT;
        }
        else if (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Up)
        {
            key = VK_UP;
        }
        else if (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Down)
        {
            key = VK_DOWN;
        }
        return key;
    }

    winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason GetReasonFromKey(WPARAM key)
    {
        constexpr auto static invalidReason = static_cast<winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason>(-1);
        auto reason = invalidReason;
        if (key == VK_TAB)
        {
            byte keyboardState[256] = {};
            WINRT_VERIFY(::GetKeyboardState(keyboardState));
            reason = (keyboardState[VK_SHIFT] & 0x80) ?
                winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Last :
                winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First;
        }
        else if (key == VK_LEFT)
        {
            reason = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Left;
        }
        else if (key == VK_RIGHT)
        {
            reason = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Right;
        }
        else if (key == VK_UP)
        {
            reason = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Up;
        }
        else if (key == VK_DOWN)
        {
            reason = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Down;
        }
        return reason;
    }


}

bool DesktopWindow::NavigateFocus(MSG* msg)
{
    if (const auto nextFocusedIsland = GetNextFocusedIsland(msg))
    {
        WINRT_VERIFY(!nextFocusedIsland.HasFocus());
        const auto previousFocusedWindow = ::GetFocus();
        RECT rect = {};
        WINRT_VERIFY(::GetWindowRect(previousFocusedWindow, &rect));
        const auto nativeIsland = nextFocusedIsland.as<IDesktopWindowXamlSourceNative>();
        HWND islandWnd{};
        winrt::check_hresult(nativeIsland->get_WindowHandle(&islandWnd));
        POINT pt = { rect.left, rect.top };
        SIZE size = { rect.right - rect.left, rect.bottom - rect.top };
        ::ScreenToClient(islandWnd, &pt);
        const auto hintRect = winrt::Windows::Foundation::Rect({ static_cast<float>(pt.x), static_cast<float>(pt.y), static_cast<float>(size.cx), static_cast<float>(size.cy) });
        const auto reason = GetReasonFromKey(msg->wParam);
        const auto request = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationRequest(reason, hintRect);
        m_lastFocusRequestId = request.CorrelationId();
        const auto result = nextFocusedIsland.NavigateFocus(request);
        return result.WasFocusMoved();
    }
    else
    {
        const bool islandIsFocused = GetFocusedIsland() != nullptr;
        byte keyboardState[256] = {};
        WINRT_VERIFY(::GetKeyboardState(keyboardState));
        const bool isMenuModifier = keyboardState[VK_MENU] & 0x80;
        if (islandIsFocused && !isMenuModifier)
        {
            return false;
        }
        const bool isDialogMessage = !!IsDialogMessage(hwnd.get(), msg);
        return isDialogMessage;
    }
}

winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource DesktopWindow::GetFocusedIsland()
{
    for (auto& xamlSource : m_xamlSources)
    {
        if (xamlSource.HasFocus())
        {
            return xamlSource;
        }
    }
    return nullptr;
}

winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource DesktopWindow::GetNextFocusedIsland(const MSG* msg)
{
    constexpr auto static invalidReason = static_cast<winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason>(-1);
    if (msg->message == WM_KEYDOWN)
    {
        const auto key = msg->wParam;
        auto reason = GetReasonFromKey(key);
        if (reason != invalidReason)
        {
            const BOOL previous =
                (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First ||
                    reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Down ||
                    reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Right) ? false : true;

            const auto currentFocusedWindow = ::GetFocus();
            const auto nextElement = GetNextDlgTabItem(hwnd.get(), currentFocusedWindow, previous);
            for (auto& xamlSource : m_xamlSources)
            {
                HWND islandWnd{};
                winrt::check_hresult(xamlSource.as<IDesktopWindowXamlSourceNative>()->get_WindowHandle(&islandWnd));
                if (nextElement == islandWnd)
                {
                    return xamlSource;
                }
            }
        }
    }

    return nullptr;
}

bool DesktopWindow::FilterMessage(const MSG* msg)
{
	// When multiple child windows are present it is needed to pre dispatch messages to all
    // DesktopWindowXamlSource instances so keyboard accelerators and
    // keyboard focus work correctly.
	for (auto& xamlSource : m_xamlSources)
	{
		BOOL xamlSourceProcessedMessage = FALSE;
		winrt::check_hresult(xamlSource.as<IDesktopWindowXamlSourceNative2>()->PreTranslateMessage(msg, &xamlSourceProcessedMessage));
		if (xamlSourceProcessedMessage != FALSE)
		{
			return true;
		}
	}

	return false;
}

void DesktopWindow::OnTakeFocusRequested(winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource const& sender, winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSourceTakeFocusRequestedEventArgs const& args)
{
    if (args.Request().CorrelationId() != m_lastFocusRequestId)
    {
        const auto reason = args.Request().Reason();
        const BOOL previous =
            (reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::First ||
                reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Down ||
                reason == winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Right) ? false : true;

        const auto nativeXamlSource = sender.as<IDesktopWindowXamlSourceNative>();
        HWND senderHwnd = nullptr;
        winrt::check_hresult(nativeXamlSource->get_WindowHandle(&senderHwnd));

        MSG msg = {};
        msg.hwnd = senderHwnd;
        msg.message = WM_KEYDOWN;
        msg.wParam = GetKeyFromReason(reason);
        if (!NavigateFocus(&msg))
        {
            const auto nextElement = ::GetNextDlgTabItem(hwnd.get(), senderHwnd, previous);
            ::SetFocus(nextElement);
        }
    }
    else
    {
        const auto request = winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationRequest(winrt::Windows::UI::Xaml::Hosting::XamlSourceFocusNavigationReason::Restore);
        m_lastFocusRequestId = request.CorrelationId();
        sender.NavigateFocus(request);
    }
}

HWND DesktopWindow::GetHandle()
{
	return hwnd.get();
}

HWND DesktopWindow::CreateDesktopWindowsXamlSource(DWORD extraStyles, const winrt::Windows::UI::Xaml::UIElement& content)
{
    winrt::Windows::UI::Xaml::Hosting::DesktopWindowXamlSource desktopSource;

    auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();
    // Parent the DesktopWindowXamlSource object to current window
    winrt::check_hresult(interop->AttachToWindow(hwnd.get()));
    HWND xamlSourceWindow{}; // Lifetime controlled desktopSource
    winrt::check_hresult(interop->get_WindowHandle(&xamlSourceWindow));
    const DWORD style = GetWindowLongW(xamlSourceWindow, GWL_STYLE) | extraStyles;
    SetWindowLongW(xamlSourceWindow, GWL_STYLE, style);
    
    desktopSource.Content(content);

    m_takeFocusEventRevokers.push_back(desktopSource.TakeFocusRequested(winrt::auto_revoke, { this, &DesktopWindow::OnTakeFocusRequested }));
    m_xamlSources.push_back(desktopSource);

    return xamlSourceWindow;
}

void DesktopWindow::ClearXamlIslands()
{
    for (auto& takeFocusRevoker : m_takeFocusEventRevokers)
    {
        takeFocusRevoker.revoke();
    }
    m_takeFocusEventRevokers.clear();

    for (auto& xamlSource : m_xamlSources)
    {
        xamlSource.Close();
    }
    m_xamlSources.clear();
}

int DesktopWindow::MessageLoop(HACCEL accelerators)
{
    MSG msg = {};
    while (GetMessageW(&msg, nullptr, 0, 0))
    {
        const bool processedMessage = FilterMessage(&msg);
        if (!processedMessage && !TranslateAcceleratorW(msg.hwnd, accelerators, &msg))
        {
            if (!NavigateFocus(&msg))
            {
                TranslateMessage(&msg);
                DispatchMessageW(&msg);
            }
        }
    }

    return static_cast<int>(msg.wParam);
}
