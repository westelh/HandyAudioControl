﻿// HandyAudioControl.cpp : アプリケーションのエントリ ポイントを定義します。
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

// グローバル変数:
HINSTANCE hInst;                                // 現在のインターフェイス
WCHAR szTitle[MAX_LOADSTRING];                  // タイトル バーのテキスト
WCHAR szWindowClass[MAX_LOADSTRING];            // メイン ウィンドウ クラス名

// このコード モジュールに含まれる関数の宣言を転送します:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // グローバル文字列を初期化する
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_HANDYAUDIOCONTROL, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // アプリケーション初期化の実行:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_HANDYAUDIOCONTROL));
        
    MSG msg;

    auto hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (!SUCCEEDED(hr))
    {
        OutputDebugString(L"COM Initialization has failed");
        return 1;
    }

    try {
        const auto devices = HandyAudioControl::MMDevice::Enumerate(EDataFlow::eRender, DEVICE_STATE_ACTIVE);
        const auto n = devices.size();
        const auto nstring = std::to_wstring(n);
        std::wstringstream ss{};
        ss << L"Found " << nstring << L" devices\n";
        OutputDebugString(ss.str().c_str());
        ss.clear();

        for (size_t i = 0; i < n; i++)
        {
            DWORD state = 0;
            const auto id = devices.at(i).GetId();
            const auto name = devices.at(i).GetFriendlyName();
            ss << id << L" " << name << L"\n";
            OutputDebugString(ss.str().c_str());
            ss.clear();
        }
    }
    catch (std::exception e) {
        OutputDebugByteString(e.what());
        return 1;
    }

	// メイン メッセージ ループ:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

    CoUninitialize();
    return (int) msg.wParam;
}



//
//  関数: MyRegisterClass()
//
//  目的: ウィンドウ クラスを登録します。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_HANDYAUDIOCONTROL));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_HANDYAUDIOCONTROL);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   関数: InitInstance(HINSTANCE, int)
//
//   目的: インスタンス ハンドルを保存して、メイン ウィンドウを作成します
//
//   コメント:
//
//        この関数で、グローバル変数でインスタンス ハンドルを保存し、
//        メイン プログラム ウィンドウを作成および表示します。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // グローバル変数にインスタンス ハンドルを格納する

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }


   HWND hwndButton = CreateWindow(
       L"BUTTON",  // Predefined class; Unicode assumed 
       L"Toggle!",      // Button text 
       WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
       10,         // x position 
       10,         // y position 
       100,        // Button width
       100,        // Button height
       hWnd,     // Parent window
       NULL,       // No menu.
       (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
       NULL);      // Pointer not needed.

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  関数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: メイン ウィンドウのメッセージを処理します。
//
//  WM_COMMAND  - アプリケーション メニューの処理
//  WM_PAINT    - メイン ウィンドウを描画する
//  WM_DESTROY  - 中止メッセージを表示して戻る
//
//
DWORD current = 0;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 選択されたメニューの解析:
            switch (wmId)
            {
            case BN_CLICKED:
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
                break;

            // (メニュー項目) [ヘルプ] の [バージョン情報] で使用
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;

            // (メニュー項目) [ファイル] の [終了] で使用
            case IDM_EXIT:
                SendMessage(hWnd, WM_CLOSE, 0, 0);
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: HDC を使用する描画コードをここに追加してください...
            EndPaint(hWnd, &ps);
        }
        break;

    // ウィンドウまたはアプリケーションを終了する必要があることを示すシグナル
    case WM_CLOSE:
        DestroyWindow(hWnd);
        break;

    // ウィンドウが破棄されるときに送信
    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
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
