// HandyAudioControl.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "pch.h"
#include "HandyAudioControl.h"
#include "MMDeviceClient.h"
#include "PolicyConfigClient.h"
#include "Utils.h"
#include "Window.h"

#define MAX_LOADSTRING 100

using namespace std::string_literals;
using namespace HandyAudioControl;
using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Hosting;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Composition;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Numerics;

#define MAX_LOADSTRING 100

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

HWND _hWnd;
HWND _childhWnd;
HWND hWndXamlIsland = nullptr;   // ルートのハンドル
HINSTANCE _hInstance;

class CustomAsyncAction : public IAsyncAction {
public:
    CustomAsyncAction() = default;
};

void ToggleAudioEndPoint() noexcept {
    static unsigned int current = 0;
    try {
        HandyAudioControl::PolicyConfigClient client{};
        const auto devices = HandyAudioControl::MMDevice::Enumerate(EDataFlow::eRender, DEVICE_STATE_ACTIVE);
        if (devices.size() > 0)
        {
            auto&& next = devices.at(current++ % devices.size());
            const auto id = next.GetId();
            const auto name = next.GetFriendlyName();
            client.SetDefaultAudioEndPoint(id.c_str(), ERole::eCommunications);
            client.SetDefaultAudioEndPoint(id.c_str(), ERole::eMultimedia);
            client.SetDefaultAudioEndPoint(id.c_str(), ERole::eConsole);
            std::wstringstream ss;
            ss << L"Switched default audio endpoint to " << name << L"\n";
            OutputDebugString(ss.str().c_str());

        }
    }
    catch (const std::exception& e) {
        OutputDebugByteString(e.what());
    }
}

IAsyncAction OnButtonClick(Windows::Foundation::IInspectable const& sender, RoutedEventArgs const& e) {
    ToggleAudioEndPoint();
    return CustomAsyncAction{};
}

UIElement CreateXamlControl() {
    // Create the XAML content.
    Windows::UI::Xaml::Controls::StackPanel xamlContainer;
    xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::White() });

    Windows::UI::Xaml::Controls::Button button;
    button.Content(box_value(L"Toggle!"));
    button.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
    button.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
    button.FontSize(48);
    button.Foreground(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::Black() });
    button.Click(&OnButtonClick);

    xamlContainer.Children().Append(button);
    xamlContainer.UpdateLayout();

    return xamlContainer;
}

class MainWindow : public DesktopWindowT<MainWindow> {
public:
    MainWindow(HINSTANCE hInstance, int nCmdShow) noexcept
    {
        LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
        LoadStringW(hInstance, IDC_HANDYAUDIOCONTROL, szWindowClass, MAX_LOADSTRING);

        WNDCLASSEXW wcex = {};
        wcex.cbSize = sizeof(WNDCLASSEX);
        wcex.style = CS_HREDRAW | CS_VREDRAW;
        wcex.lpfnWndProc = WndProc;
        wcex.cbClsExtra = DLGWINDOWEXTRA;
        wcex.cbWndExtra = 0;
        wcex.hInstance = hInstance;
        wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HANDYAUDIOCONTROL));
        wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_HANDYAUDIOCONTROL);
        wcex.lpszClassName = szWindowClass;
        wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
        WINRT_VERIFY(RegisterClassEx(&wcex));
        WINRT_ASSERT(!GetHandle());

        const auto wnd = InitInstance(hInstance, nCmdShow);
        WINRT_ASSERT(wnd);
    }

    LRESULT MessageHandler(UINT const message, WPARAM const wParam, LPARAM const lParam) noexcept
    {
        HRESULT hr = S_OK;

        switch (message)
        {
            HANDLE_MSG(GetHandle(), WM_CREATE, OnCreate);
            HANDLE_MSG(GetHandle(), WM_COMMAND, OnCommand);
            HANDLE_MSG(GetHandle(), WM_DESTROY, OnDestroy);
            HANDLE_MSG(GetHandle(), WM_SIZE, OnResize);
        default:
            return base_type::MessageHandler(message, wParam, lParam);
        }

        return base_type::MessageHandler(message, wParam, lParam);
    }

private:
    wil::unique_hwnd hWndXamlIsland = nullptr;
    //winrt::MyApp::MainUserControl m_mainUserControl = nullptr;
    //winrt::SampleLibraryCS::CustomUserControl m_managedControl = nullptr;
    winrt::Windows::UI::Xaml::Controls::Button::Click_revoker m_xamlBt1ClickEventRevoker;

    HWND InitInstance(HINSTANCE hInstance, int nCmdShow)
    {
        hInst = hInstance; // Store instance handle in our global variable
        HWND hMainWnd = CreateWindow(
            szWindowClass,
            szTitle,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
            nullptr, nullptr, hInstance, this);
        THROW_LAST_ERROR_IF(hMainWnd == 0);

        ShowWindow(hMainWnd, nCmdShow);
        UpdateWindow(hMainWnd);
        SetFocus(hMainWnd);
        return hMainWnd;
    }

    bool OnCreate(HWND, LPCREATESTRUCT)
    {
        //DEVICE_SCALE_FACTOR scaleFactor = {};
        //winrt::check_hresult(GetScaleFactorForMonitor(MonitorFromWindow(GetHandle(), 0), &scaleFactor));
        //const auto dpi = static_cast<int>(scaleFactor) / 100.0f;

        //winrt::Contoso::Interop::IMainFormInterop interop{ nullptr };
        //m_mainUserControl = winrt::MyApp::MainUserControl(interop);

        auto manager = winrt::Windows::UI::Xaml::Hosting::WindowsXamlManager::InitializeForCurrentThread();
        hWndXamlIsland = wil::unique_hwnd(CreateDesktopWindowsXamlSource(WS_TABSTOP, CreateXamlControl()));
        return true;
    }

    void OnCommand(HWND, int id, HWND hwndCtl, UINT codeNotify)
    {
        switch (id)
        {
            //case IDM_ABOUT:
            //    DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), GetHandle(), About);
            //    break;
            //case IDM_EXIT:
            //    PostQuitMessage(0);
            //    break;
            //case IDM_ButtonID1:
            //case IDM_ButtonID2:
            //    if (m_mainUserControl)
            //    {
            //        const auto string = (id == IDM_ButtonID1) ? winrt::hstring(L"Native button 1") : winrt::hstring(L"Native button 2");
            //        //m_mainUserControl.MyProperty(string);
            //    }
            //    break;
        }
    }

	void OnDestroy(HWND hwnd)
	{
		if (m_xamlBt1ClickEventRevoker)
		{
			m_xamlBt1ClickEventRevoker.revoke();
		}

		base_type::OnDestroy(hwnd);
	}

	void OnResize(HWND, UINT state, int cx, int cy)
	{
		SetWindowPos(hWndXamlIsland.get(), 0, 0, 0, cx, cy, SWP_SHOWWINDOW);

		//const auto newHeight = cy;
		//const auto newWidth = cx;
		//const auto islandHeight = newHeight - (ButtonHeight * 2) - ButtonMargin;
		//const auto islandWidth = newWidth - (ButtonMargin * 2);
		//SetWindowPos(m_hButton1.get(), 0, ButtonWidth * 2, ButtonMargin, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
		//SetWindowPos(m_hWndXamlButton1.get(), m_hButton1.get(), newWidth - (ButtonWidth * 2), ButtonMargin, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
		//SetWindowPos(m_hWndXamlIsland.get(), m_hWndXamlButton1.get(), 0, XamlIslandMargin, islandWidth, islandHeight, SWP_SHOWWINDOW);
		//SetWindowPos(m_hButton2.get(), m_hWndXamlIsland.get(), (ButtonMargin + newWidth - ButtonWidth) / 2, newHeight - ButtonMargin - ButtonHeight, ButtonWidth, ButtonHeight, SWP_SHOWWINDOW);
	}

	void OnXamlButtonClick(winrt::Windows::Foundation::IInspectable const& sender, winrt::Windows::UI::Xaml::RoutedEventArgs const&)
	{
		//m_mainUserControl.MyProperty(winrt::hstring(L"Xaml K Button 1"));
	}

};

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) try
{
    winrt::init_apartment(winrt::apartment_type::single_threaded);
    MainWindow window{hInstance, nCmdShow};
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HANDYAUDIOCONTROL));
    int retValue = window.MessageLoop(hAccelTable);
    return retValue;                                            
}
CATCH_RETURN();

LRESULT CommandProc(HWND hWnd, UINT messageCode, WPARAM wParam, LPARAM lParam) {
	int wmId = LOWORD(wParam);
	// 選択されたメニューの解析:
	switch (wmId)
	{
		// (メニュー項目) [ヘルプ] の [バージョン情報] で使用
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		break;

		// (メニュー項目) [ファイル] の [終了] で使用
	case IDM_EXIT:
		SendMessage(hWnd, WM_CLOSE, 0, 0);
		break;

	default:
        break;
	}
    return DefWindowProc(hWnd, messageCode, wParam, lParam);
}


LRESULT CALLBACK WindowProc(HWND hWnd, UINT messageCode, WPARAM wParam, LPARAM lParam)
{
    RECT rcClient;

	switch (messageCode)
	{
	case WM_PAINT:
		if (hWnd == _hWnd)
		{
		}
		break;

	case WM_COMMAND:
        return CommandProc(hWnd, messageCode, wParam, lParam);
	    break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

		// ウィンドウまたはアプリケーションを終了する必要があることを示すシグナル
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;

		// Create main window
	case WM_CREATE:
		_childhWnd = CreateWindowEx(0, L"ChildWClass", NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, _hInstance, NULL);
		return 0;

		// Main window changed size
	case WM_SIZE:
		// Get the dimensions of the main window's client
		// area, and enumerate the child windows. Pass the
		// dimensions to the child windows during enumeration.
		GetClientRect(hWnd, &rcClient);
		MoveWindow(hWndXamlIsland, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, TRUE);
		MoveWindow(_childhWnd, 200, 200, 400, 500, TRUE);
		ShowWindow(_childhWnd, SW_SHOW);

		return 0;

		// Process other messages.

	default:
		return DefWindowProc(hWnd, messageCode, wParam, lParam);
		break;
	}

    return 0;
}

// バージョン情報ボックスのメッセージ ハンドラーです。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
