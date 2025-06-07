#include<Windows.h>
#include<CommCtrl.h>
#include"resource.h"

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)\
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc, 0); // if DlgProc mark red, change x64 to x86  or add befor DlgProc (DLGPROC)
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

		// For WM_COMMAND
		// LOWORD(wParam) - who send message, HIWORD(wParam) - message code, what event happened to the sender
		// LOWERD(lParam) - HWND, who sender, HIWORD(lParam) - don't use, = 0
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
			if (FIRST_IPADDRESS(dwIPaddress) < 128) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 192) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
			else if (FIRST_IPADDRESS(dwIPaddress) < 224) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);
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