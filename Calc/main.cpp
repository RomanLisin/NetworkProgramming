#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int CountPrefixBits(DWORD mask);
void SetMaskAndPrefix(HWND hIPmask, HWND hEditPrefix, DWORD dwMask);
INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)\
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc, 0); // if DlgProc red, change x64 to x86  or add befor DlgProc (DLGPROC)
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		{
	case WM_INITDIALOG:
		HWND hPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hPrefix, UDM_SETRANGE, 0, MAKELPARAM(32, 0));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));
		}
		break;
	case WM_COMMAND:
	{
		HWND hIPaddress = GetDlgItem(hwnd, IDC_IPADDRESS);
		HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
		HWND hEditPrefix = GetDlgItem(hwnd, IDC_EDIT_PREFIX);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;


		switch (LOWORD(wParam))
		{

		case IDC_IPADDRESS:
		{
			SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
			if (FIRST_IPADDRESS(dwIPaddress) < 128)  dwIPmask = 0xFF000000;
			else if (FIRST_IPADDRESS(dwIPaddress) < 192)  dwIPmask = 0xFFFF0000;
			else if (FIRST_IPADDRESS(dwIPaddress) < 224) dwIPmask = 0xFFFFFF00;
			else dwIPmask = 0;
			SetMaskAndPrefix(hIPmask, hEditPrefix, dwIPmask);
		}
		break;
		case IDC_IPMASK:
		{
			if (dwIPmask != 0)
			SendMessage(hEditPrefix, WM_SETTEXT, 0, CountPrefixBits(dwIPmask));
		}
		break;
		case IDC_EDIT_PREFIX:
		{
			SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFFF0);
		}
		break;
		case IDOK:
		break;
		case IDCANCEL:
			EndDialog(hwnd, 0);
			break;
		}
	}
	break;
	case WM_CLOSE:
		EndDialog(hwnd, 0);
	}
	return FALSE;
}
int CountPrefixBits(DWORD mask)
{    
	if (mask != 0)
	{

	int count = 0;
	for (int i = 31; i >= 0; --i) {
		if ((mask >> i) & 1)
			++count;
		else
			break; // stop first zero
	}
	return count;
	}
	return 0;
}

void SetMaskAndPrefix(HWND hIPmask, HWND hEditPrefix, DWORD dwMask)
{
	// Установить маску в IP-контрол
	SendMessage(hIPmask, IPM_SETADDRESS, 0, dwMask);

	// Сконвертировать префикс (число единичных бит) в строку
	WCHAR szPrefix[4] = {};
	wsprintf(szPrefix, L"%d", CountPrefixBits(dwMask));

	// Установить текст в Edit-контрол
	SendMessage(hEditPrefix, WM_SETTEXT, 0, (LPARAM)szPrefix);
}