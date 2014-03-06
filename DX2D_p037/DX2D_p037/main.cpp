#include "main.h"


HINSTANCE hinst;
HDC hdc;			// デバイスコンテキストへのハンドル
TCHAR ch = ' ';		// 入力された文字
RECT rect;			// REctangle 構造体
PAINTSTRUCT ps;		// WM_PAINTで使用される
bool vkKeys[256];	// 仮想キー状態


const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World";
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;

const int test = 3012;

// ----------------------------------------------------------------------
// Windowsアプリケーションの開始点
// ----------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpcmdLine, int nCmdShow)
{
	MSG msg;

	// ウィンドウを生成
	if(!CreateMainWindow(hInstance, nCmdShow)){
		return false;
	}

	// 仮想キー状態の初期化
	for(int i = 0; i < 256; i++){
		vkKeys[i] = false;
	}

	// メッセージのメインループ
	int done = 0;
	while(!done){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			// 終了メッセージを検知
			if(msg.message == WM_QUIT){
				done = 1;
			}
			// ウィンドウイベントコールバック関数
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}


// ----------------------------------------------------------------------
// ウィンドウイベントコールバック関数
// ----------------------------------------------------------------------
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	short nVirtKey;	// 仮想キーのキーコード
	const short SHIFTED = (short)0x8000;
	TEXTMETRIC tm;
	DWORD chWidth = 20;
	DWORD chHeight = 20;

	switch(msg){
	case WM_CREATE:
		hdc = GetDC(hwnd);
		GetTextMetrics(hdc, &tm);
		ReleaseDC(hwnd, hdc);
		chWidth = tm.tmAveCharWidth;
		chHeight = tm.tmHeight;
		return 0;

	case WM_DESTROY:
		// Windowsにこのプログラムを終了させるように伝える
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:	// キーが押された場合
		vkKeys[wParam] = true;
		switch(wParam){
		case VK_SHIFT:		// Shiftキー
			nVirtKey = GetKeyState(VK_LSHIFT);	// 左Shiftキーの状態を取得
			if(nVirtKey & SHIFTED)				// 左Shiftキーの場合
				vkKeys[VK_LSHIFT] = true;
			nVirtKey = GetKeyState(VK_RSHIFT);
			if(nVirtKey & SHIFTED)
				vkKeys[VK_RSHIFT] = true;
			break;
		case VK_CONTROL:	// Ctrlキー
			nVirtKey = GetKeyState(VK_LCONTROL);
			if(nVirtKey & SHIFTED)				// 左Ctrlキーの場合
				vkKeys[VK_LCONTROL] = true;
			nVirtKey = GetKeyState(VK_RCONTROL);
			if(nVirtKey & SHIFTED)
				vkKeys[VK_RCONTROL] = true;
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
		break;

	case WM_KEYUP:
		vkKeys[wParam] = false;
		switch(wParam){
		case VK_SHIFT:
			nVirtKey = GetKeyState(VK_LSHIFT);
			if((nVirtKey & SHIFTED) == 0){
				vkKeys[VK_LSHIFT] = false;
			}
			nVirtKey = GetKeyState(VK_RSHIFT);
			if((nVirtKey & SHIFTED) == 0){
				vkKeys[VK_RSHIFT] = false;
			}
			break;
		}
		InvalidateRect(hwnd, NULL, TRUE);
		return 0;
		break;

	case WM_CHAR:	// キーボードから文字が入力された場合
		switch(wParam){
		case 0x08:	// バックスペース
		case 0x09:	// タブ
		case 0x0A:	// ラインフィード
		case 0x0D:	// キャリッジターン
		case 0x1B:	// ESC
			MessageBeep((UINT) -1);	// ビープ音を鳴らす。表示はしない
			return 0;
		default:	// 表示可能な文字
			ch = (TCHAR) wParam;	// 文字を取得
			// WM_PAINTを強制的に発生させる
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}

/*
	case WM_PAINT:	// ウィンドウを再描画する必要がある場合
		// デバイスコンテキストへのハンドルを取得
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);	// ウィンドウの短形を所得
		// 文字を表示
		TextOut(hdc, rect.right/2, rect.bottom/2, &ch, 1);
		EndPaint(hwnd, &ps);
		return 0;
*/

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		TextOut(hdc, 0, 0, &ch, 1);

		for(int r = 0; r < 16; r++){
			for(int c = 0; c < 16; c++){
				if(vkKeys[r*16+c]){
					SetBkMode(hdc, OPAQUE);
					TextOut(hdc, c*chWidth+chWidth*2, r*chHeight+chHeight*2, "T ", 2);
				}
				else{
					SetBkMode(hdc, TRANSPARENT);
					TextOut(hdc, c*chWidth+chWidth*2, r*chHeight+chHeight*2, "F ", 2);
				}
			}
		}

		EndPaint(hwnd, &ps);
		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

}


// ----------------------------------------------------------------------
// 
// ----------------------------------------------------------------------
bool CreateMainWindow(HINSTANCE hInstance, int nCmdShow)
{
	WNDCLASSEX wcx;
	HWND hwnd;

	wcx.cbSize = sizeof(wcx);					// 構造体のサイズ	
	wcx.style = CS_HREDRAW | CS_VREDRAW;		// ウィンドウサイズ変更時に再描画
	wcx.lpfnWndProc = WinProc;					// ウィンドウプロシージャを指す
	wcx.cbClsExtra = 0;							// 拡張クラスメモリなし
	wcx.cbWndExtra = 0;							// 拡張ウィンドウメモリなし
	wcx.hInstance = hInstance;					// インスタンスへのハンドル
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// 事前定義されている矢印カーソル
	wcx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	// 黒色の背景
	wcx.lpszMenuName = NULL;					// メニューリソースの名前
	wcx.lpszClassName = CLASS_NAME;				// ウィンドウクラスの名前
	wcx.hIconSm = NULL;							// 小さいアイコン

	// ウィンドウクラスを登録
	// エラー時に、RegisterClassExは0を戻す
	if(RegisterClassEx(&wcx) == 0){	// エラーの場合
		return false;		
	}

	// ウィンドウを作成
	hwnd = CreateWindow(
		CLASS_NAME,				// ウィンドウクラスの名前
		APP_TITLE,				// タイトルバーのテキスト
		WS_OVERLAPPEDWINDOW,	// ウィンドウのスタイル
		CW_USEDEFAULT,			// ウィンドウの水平位置のデフォルト
		CW_USEDEFAULT,			// ウィンドウの垂直位置のデフォルト
		WINDOW_WIDTH,			// ウィンドウの幅
		WINDOW_HEIGHT,			// ウィンドウの高さ
		(HWND)NULL,				// 親ウィンドウなし
		(HMENU)NULL,			// メニューなし
		hInstance,				// アプリケーションインスタンスへのハンドル
		(LPVOID)NULL			// ウィンドウパラメータなし
		);

	if(!hwnd){
		return false;
	}

	// ウィンドウを表示
	ShowWindow(hwnd, nCmdShow);
	// ウィンドウプロシージャにWM_PAINTメッセージを送る
	UpdateWindow(hwnd);
	return true;
}
