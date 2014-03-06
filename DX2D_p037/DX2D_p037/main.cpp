#include "main.h"


HINSTANCE hinst;
HDC hdc;			// �f�o�C�X�R���e�L�X�g�ւ̃n���h��
TCHAR ch = ' ';		// ���͂��ꂽ����
RECT rect;			// REctangle �\����
PAINTSTRUCT ps;		// WM_PAINT�Ŏg�p�����
bool vkKeys[256];	// ���z�L�[���


const char CLASS_NAME[] = "WinMain";
const char APP_TITLE[] = "Hello World";
const int WINDOW_WIDTH = 400;
const int WINDOW_HEIGHT = 400;

const int test = 3012;

// ----------------------------------------------------------------------
// Windows�A�v���P�[�V�����̊J�n�_
// ----------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpcmdLine, int nCmdShow)
{
	MSG msg;

	// �E�B���h�E�𐶐�
	if(!CreateMainWindow(hInstance, nCmdShow)){
		return false;
	}

	// ���z�L�[��Ԃ̏�����
	for(int i = 0; i < 256; i++){
		vkKeys[i] = false;
	}

	// ���b�Z�[�W�̃��C�����[�v
	int done = 0;
	while(!done){
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)){
			// �I�����b�Z�[�W�����m
			if(msg.message == WM_QUIT){
				done = 1;
			}
			// �E�B���h�E�C�x���g�R�[���o�b�N�֐�
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return msg.wParam;
}


// ----------------------------------------------------------------------
// �E�B���h�E�C�x���g�R�[���o�b�N�֐�
// ----------------------------------------------------------------------
LRESULT WINAPI WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	short nVirtKey;	// ���z�L�[�̃L�[�R�[�h
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
		// Windows�ɂ��̃v���O�������I��������悤�ɓ`����
		PostQuitMessage(0);
		return 0;

	case WM_KEYDOWN:	// �L�[�������ꂽ�ꍇ
		vkKeys[wParam] = true;
		switch(wParam){
		case VK_SHIFT:		// Shift�L�[
			nVirtKey = GetKeyState(VK_LSHIFT);	// ��Shift�L�[�̏�Ԃ��擾
			if(nVirtKey & SHIFTED)				// ��Shift�L�[�̏ꍇ
				vkKeys[VK_LSHIFT] = true;
			nVirtKey = GetKeyState(VK_RSHIFT);
			if(nVirtKey & SHIFTED)
				vkKeys[VK_RSHIFT] = true;
			break;
		case VK_CONTROL:	// Ctrl�L�[
			nVirtKey = GetKeyState(VK_LCONTROL);
			if(nVirtKey & SHIFTED)				// ��Ctrl�L�[�̏ꍇ
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

	case WM_CHAR:	// �L�[�{�[�h���當�������͂��ꂽ�ꍇ
		switch(wParam){
		case 0x08:	// �o�b�N�X�y�[�X
		case 0x09:	// �^�u
		case 0x0A:	// ���C���t�B�[�h
		case 0x0D:	// �L�����b�W�^�[��
		case 0x1B:	// ESC
			MessageBeep((UINT) -1);	// �r�[�v����炷�B�\���͂��Ȃ�
			return 0;
		default:	// �\���\�ȕ���
			ch = (TCHAR) wParam;	// �������擾
			// WM_PAINT�������I�ɔ���������
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;
		}

/*
	case WM_PAINT:	// �E�B���h�E���ĕ`�悷��K�v������ꍇ
		// �f�o�C�X�R���e�L�X�g�ւ̃n���h�����擾
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);	// �E�B���h�E�̒Z�`������
		// ������\��
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

	wcx.cbSize = sizeof(wcx);					// �\���̂̃T�C�Y	
	wcx.style = CS_HREDRAW | CS_VREDRAW;		// �E�B���h�E�T�C�Y�ύX���ɍĕ`��
	wcx.lpfnWndProc = WinProc;					// �E�B���h�E�v���V�[�W�����w��
	wcx.cbClsExtra = 0;							// �g���N���X�������Ȃ�
	wcx.cbWndExtra = 0;							// �g���E�B���h�E�������Ȃ�
	wcx.hInstance = hInstance;					// �C���X�^���X�ւ̃n���h��
	wcx.hIcon = NULL;
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);	// ���O��`����Ă�����J�[�\��
	wcx.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);	// ���F�̔w�i
	wcx.lpszMenuName = NULL;					// ���j���[���\�[�X�̖��O
	wcx.lpszClassName = CLASS_NAME;				// �E�B���h�E�N���X�̖��O
	wcx.hIconSm = NULL;							// �������A�C�R��

	// �E�B���h�E�N���X��o�^
	// �G���[���ɁARegisterClassEx��0��߂�
	if(RegisterClassEx(&wcx) == 0){	// �G���[�̏ꍇ
		return false;		
	}

	// �E�B���h�E���쐬
	hwnd = CreateWindow(
		CLASS_NAME,				// �E�B���h�E�N���X�̖��O
		APP_TITLE,				// �^�C�g���o�[�̃e�L�X�g
		WS_OVERLAPPEDWINDOW,	// �E�B���h�E�̃X�^�C��
		CW_USEDEFAULT,			// �E�B���h�E�̐����ʒu�̃f�t�H���g
		CW_USEDEFAULT,			// �E�B���h�E�̐����ʒu�̃f�t�H���g
		WINDOW_WIDTH,			// �E�B���h�E�̕�
		WINDOW_HEIGHT,			// �E�B���h�E�̍���
		(HWND)NULL,				// �e�E�B���h�E�Ȃ�
		(HMENU)NULL,			// ���j���[�Ȃ�
		hInstance,				// �A�v���P�[�V�����C���X�^���X�ւ̃n���h��
		(LPVOID)NULL			// �E�B���h�E�p�����[�^�Ȃ�
		);

	if(!hwnd){
		return false;
	}

	// �E�B���h�E��\��
	ShowWindow(hwnd, nCmdShow);
	// �E�B���h�E�v���V�[�W����WM_PAINT���b�Z�[�W�𑗂�
	UpdateWindow(hwnd);
	return true;
}
