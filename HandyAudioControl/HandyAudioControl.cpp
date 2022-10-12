// HandyAudioControl.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include "pch.h"
#include "HandyAudioControl.h"
#include "MMDeviceClient.h"
#include "PolicyConfigClient.h"
#include "Utils.h"

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

IAsyncAction OnButtonClick(IInspectable const& sender, RoutedEventArgs const& e) {
    ToggleAudioEndPoint();
    return CustomAsyncAction{};
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    _hInstance = hInstance;

    // The main window class name.
    const wchar_t szWindowClass[] = L"HandyAudioControl";
    WNDCLASSEX windowClass = { };

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.lpszClassName = szWindowClass;
    windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClass.hIconSm = LoadIcon(windowClass.hInstance, IDI_APPLICATION);

    if (RegisterClassEx(&windowClass) == NULL)
    {
        MessageBox(NULL, L"Windows registration failed!", L"Error", NULL);
        return 0;
    }

    _hWnd = CreateWindow(
        szWindowClass,
        L"Handy Audio Control",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (_hWnd == NULL)
    {
        OutputDebugByteString("Failed to create window");
        return 1;
    }

    // XAML コントロールのホスティング
    winrt::init_apartment(apartment_type::single_threaded);  // WinRTをSTAで初期化
    WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();    // XAMLフレームワークの初期化
    DesktopWindowXamlSource desktopSource;   // XAML コントロールのホスティング(XAML Islands)のメインクラス
    auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();   // ルートの取得
    check_hresult(interop->AttachToWindow(_hWnd));    // ウィンドウにアタッチ
    interop->get_WindowHandle(&hWndXamlIsland);  // ハンドルの取得
    SetWindowPos(hWndXamlIsland, 0, 0, 0, 640, 480, SWP_SHOWWINDOW);
    // Create the XAML content.
    Windows::UI::Xaml::Controls::StackPanel xamlContainer;
    xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::White()});

    Windows::UI::Xaml::Controls::Button button;
    button.Content(box_value(L"Toggle!"));
    button.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
    button.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
    button.FontSize(48);
    button.Foreground(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::Black() } );
    button.Click(&OnButtonClick);

    xamlContainer.Children().Append(button);
    xamlContainer.UpdateLayout();
    desktopSource.Content(xamlContainer);

    ShowWindow(_hWnd, nCmdShow);
    UpdateWindow(_hWnd);

    //Message loop:
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

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
