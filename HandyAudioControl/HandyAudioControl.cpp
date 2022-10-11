// HandyAudioControl.cpp : アプリケーションのエントリ ポイントを定義します。
//

#include <string>
#include <sstream>
#include "pch.h"
#include "HandyAudioControl.h"
#include "MMDeviceClient.h"
#include "PolicyConfigClient.h"
#include "Utils.h"

using namespace std::string_literals;
using namespace HandyAudioControl;

#define MAX_LOADSTRING 100

using namespace winrt;
using namespace Windows::UI;
using namespace Windows::UI::Composition;
using namespace Windows::UI::Xaml::Hosting;
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

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    _hInstance = hInstance;

    // The main window class name.
    const wchar_t szWindowClass[] = L"Win32DesktopApp";
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
        L"Windows c++ Win32 Desktop App",
        WS_OVERLAPPEDWINDOW | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
        NULL,
        NULL,
        hInstance,
        NULL
    );
    if (_hWnd == NULL)

    // XAML コントロールのホスティング
    using namespace winrt;
    using namespace Windows::UI::Xaml::Hosting;
    winrt::init_apartment(apartment_type::single_threaded);  // WinRTをSTAで初期化
    WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();    // XAMLフレームワークの初期化
    DesktopWindowXamlSource desktopSource;   // XAML コントロールのホスティング(XAML Islands)のメインクラス
    auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();   // ルートの取得
    check_hresult(interop->AttachToWindow(_hWnd));    // ウィンドウにアタッチ
    interop->get_WindowHandle(&hWndXamlIsland);  // ハンドルの取得
    SetWindowPos(hWndXamlIsland, 0, 0, 0, 640, 480, SWP_SHOWWINDOW);
    // Create the XAML content.
    Windows::UI::Xaml::Controls::StackPanel xamlContainer;
    xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::AntiqueWhite()});

    Windows::UI::Xaml::Controls::Button button;
    button.Content(box_value(L"Toggle!"));
    button.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
    button.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
    button.FontSize(48);
    button.Foreground(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::Black() } );

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


LRESULT CALLBACK WindowProc(HWND hWnd, UINT messageCode, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;
    wchar_t greeting[] = L"Hello World in Win32!";
    RECT rcClient;

    switch (messageCode)
    {
    case WM_PAINT:
        if (hWnd == _hWnd)
        {
            hdc = BeginPaint(hWnd, &ps);
            TextOut(hdc, 300, 5, greeting, wcslen(greeting));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
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

//// グローバル変数:
//HINSTANCE hInst;                                // 現在のインターフェイス
//WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
//WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名
//
//// このコード モジュールに含まれる関数の宣言を転送します:
//ATOM                MyRegisterClass(HINSTANCE hInstance);
//BOOL                InitInstance(HINSTANCE, int);
//LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
//INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
//
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR    lpCmdLine,
//    _In_ int       nCmdShow)
//{
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//    // グローバル文字列を初期化する
//    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
//    LoadStringW(hInstance, IDC_HANDYAUDIOCONTROL, szWindowClass, MAX_LOADSTRING);
//    MyRegisterClass(hInstance);
//
//    auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
//    if (!SUCCEEDED(hr))
//    {
//        OutputDebugString(L"COM Initialization has failed");
//        return 1;
//    }
//
//    // アプリケーション初期化の実行:
//    if (!InitInstance(hInstance, nCmdShow))
//    {
//        return FALSE;
//    }
//
//    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HANDYAUDIOCONTROL));
//        
//    MSG msg;
//
//
//
//	// メイン メッセージ ループ:
//	while (GetMessage(&msg, nullptr, 0, 0))
//	{
//		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
//		{
//			TranslateMessage(&msg);
//			DispatchMessage(&msg);
//		}
//	}
//
//    CoUninitialize();
//    return (int) msg.wParam;
//}
//
//
//
////
////  関数: MyRegisterClass()
////
////  目的: ウィンドウ クラスを登録します。
////
//ATOM MyRegisterClass(HINSTANCE hInstance)
//{
//    WNDCLASSEXW wcex;
//
//    wcex.cbSize = sizeof(WNDCLASSEX);
//
//    wcex.style          = CS_HREDRAW | CS_VREDRAW;
//    wcex.lpfnWndProc    = WndProc;
//    wcex.cbClsExtra     = 0;
//    wcex.cbWndExtra     = 0;
//    wcex.hInstance      = hInstance;
//    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HANDYAUDIOCONTROL));
//    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
//    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
//    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HANDYAUDIOCONTROL);
//    wcex.lpszClassName  = szWindowClass;
//    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
//
//    return RegisterClassExW(&wcex);
//}
//
////
////   関数: InitInstance(HINSTANCE, int)
////
////   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
////
////   コメント:
////
////        この関数で、グローバル変数でインスタンス ハンドルを保存し、
////        メイン プログラム ウィンドウを作成および表示します。
////
//BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//{
//   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する
//
//   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
//      0, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);
//
//   if (!hWnd)
//   {
//      return FALSE;
//   }
//   Z
//   // XAML コントロールのホスティング
//   using namespace winrt;
//   using namespace Windows::UI::Xaml::Hosting;
//   winrt::init_apartment(apartment_type::single_threaded);  // WinRTをSTAで初期化
//   WindowsXamlManager winxamlmanager = WindowsXamlManager::InitializeForCurrentThread();    // XAMLフレームワークの初期化
//   DesktopWindowXamlSource desktopSource;   // XAML コントロールのホスティング(XAML Islands)のメインクラス
//   auto interop = desktopSource.as<IDesktopWindowXamlSourceNative>();   // ルートの取得
//   check_hresult(interop->AttachToWindow(hWnd));    // ウィンドウにアタッチ
//   HWND hWndXamlIsland = nullptr;   // ルートのハンドル
//   interop->get_WindowHandle(&hWndXamlIsland);  // ハンドルの取得
//   SetWindowPos(hWndXamlIsland, 0, 0, 0, 640, 480, SWP_SHOWWINDOW);
//   // Create the XAML content.
//   Windows::UI::Xaml::Controls::StackPanel xamlContainer;
//   xamlContainer.Background(Windows::UI::Xaml::Media::SolidColorBrush{ Windows::UI::Colors::Red() });
//
//   Windows::UI::Xaml::Controls::TextBlock tb;
//   tb.Text(L"Hello World from Xaml Islands!");
//   tb.VerticalAlignment(Windows::UI::Xaml::VerticalAlignment::Center);
//   tb.HorizontalAlignment(Windows::UI::Xaml::HorizontalAlignment::Center);
//   tb.FontSize(48);
//
//   xamlContainer.Children().Append(tb);
//   xamlContainer.UpdateLayout();
//   desktopSource.Content(xamlContainer);
//
//   // End XAML Island section.
//
//   ShowWindow(hWnd, nCmdShow);
//   UpdateWindow(hWnd);
//
//   return TRUE;
//}
//
////
////  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
////
////  目的: メイン ウィンドウのメッセージを処理します。
////
////  WM_COMMAND  - アプリケーション メニューの処理
////  WM_PAINT    - メイン ウィンドウを描画する
////  WM_DESTROY  - 中止メッセージを表示して戻る
////
////
//HWND _childhWnd;
//DWORD current = 0;
//LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    RECT rcClient;
//    switch (message)
//    {
//    case WM_COMMAND:
//        {
//            int wmId = LOWORD(wParam);
//            // 選択されたメニューの解析:
//            switch (wmId)
//            {
//            case BN_CLICKED:
//                try {   
//                HandyAudioControl::PolicyConfigClient client{};
//                    const auto devices = HandyAudioControl::MMDevice::Enumerate(EDataFlow::eRender, DEVICE_STATE_ACTIVE);
//                    if (devices.size() > 0)
//                    {
//                        auto&& next = devices.at(current++ % devices.size());
//                        const auto id = next.GetId();
//                        const auto name = next.GetFriendlyName();
//                        client.SetDefaultAudioEndPoint(id.c_str(), ERole::eCommunications);
//                        client.SetDefaultAudioEndPoint(id.c_str(), ERole::eMultimedia);
//                        client.SetDefaultAudioEndPoint(id.c_str(), ERole::eConsole);
//                        std::wstringstream ss;
//                        ss << L"Switched default audio endpoint to " << name << L"\n";
//                        OutputDebugString(ss.str().c_str());
//
//                    }
//                }
//                catch (const std::exception& e) {
//                    OutputDebugByteString(e.what());
//                }
//                break;
//
//            // (メニュー項目) [ヘルプ] の [バージョン情報] で使用
//            case IDM_ABOUT:
//                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
//                break;
//
//            // (メニュー項目) [ファイル] の [終了] で使用
//            case IDM_EXIT:
//                SendMessage(hWnd, WM_CLOSE, 0, 0);
//                break;
//
//            default:
//                return DefWindowProc(hWnd, message, wParam, lParam);
//            }
//        }
//        break;
//
//    case WM_PAINT:
//        {
//            PAINTSTRUCT ps;
//            HDC hdc = BeginPaint(hWnd, &ps);
//            // TODO: HDC を使用する描画コードをここに追加してください...
//            EndPaint(hWnd, &ps);
//        }
//        break;
//
//    // ウィンドウまたはアプリケーションを終了する必要があることを示すシグナル
//    case WM_CLOSE:
//        DestroyWindow(hWnd);
//        break;
//
//    // ウィンドウが破棄されるときに送信
//    case WM_DESTROY:
//        PostQuitMessage(0);
//        break;
//
//        // Create main window
//    case WM_CREATE:
//        _childhWnd = CreateWindowEx(0, L"ChildWClass", NULL, WS_CHILD | WS_BORDER, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);
//        return 0;
//
//        // Main window changed size
//    case WM_SIZE:
//        // Get the dimensions of the main window's client
//        // area, and enumerate the child windows. Pass the
//        // dimensions to the child windows during enumeration.
//        GetClientRect(hWnd, &rcClient);
//        MoveWindow(_childhWnd, 200, 200, 400, 500, TRUE);
//        ShowWindow(_childhWnd, SW_SHOW);
//
//        return 0;
//
//        // Process other messages.
//
//
//    default:
//        return DefWindowProc(hWnd, message, wParam, lParam);
//    }
//    return 0;
//}

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
