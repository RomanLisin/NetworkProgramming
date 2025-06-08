#define _CRT_SECURE_NO_WARNINGS
#include<Windows.h>
#include<string>
#include<CommCtrl.h>
#include"resource.h"
#define CHECK_BIT(var, pos) ((var) & (1<<(pos)))

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int MaskToPrefix(DWORD mask);
DWORD PrefixToMask(int prefix);
char* NumberIPnetOrHost(DWORD mask);
DWORD NetworkAddress(DWORD ip, int prefix);
DWORD BroadcastAddress(DWORD ip, int prefix);
LPSTR IpToString(DWORD ip);
void FillTextControl(HWND hTxtCntrl, DWORD dwIPaddr, int pref, DWORD dwIPmask, DWORD dwIPnet);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInst, LPSTR lpCmdLine, INT nCmdShow)
{
	DialogBoxParam(hInstance, MAKEINTRESOURCE(IDD_DIALOG_MAIN), NULL, DlgProc, 0); // if DlgProc mark red, change x64 to x86  or add befor DlgProc (DLGPROC)
	return 0;
}

BOOL CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		HWND hPrefix = GetDlgItem(hwnd, IDC_SPIN_PREFIX);
		SendMessage(hPrefix, UDM_SETRANGE, 0, MAKELPARAM(32, 0));
		//https://learn.microsoft.com/en-us/windows/win32/controls/udm-setrange
		SetFocus(GetDlgItem(hwnd, IDC_IPADDRESS));
	}
	break;

	case WM_NOTIFY:
	{
		NMUPDOWN* pNMUpDown = (NMUPDOWN*)lParam;
		if (pNMUpDown->hdr.idFrom == IDC_SPIN_PREFIX)
		{
			// get current value of prefix
			int prefix = SendMessage(pNMUpDown->hdr.hwndFrom, UDM_GETPOS, 0, 0);
			prefix += pNMUpDown->iDelta; // учитываем изменение

			if (prefix < 0) prefix = 0;
			if (prefix > 32) prefix = 32; 

			DWORD mask = PrefixToMask(prefix);

			// set new mask
			HWND hIPmask = GetDlgItem(hwnd, IDC_IPMASK);
			SendMessage(hIPmask, IPM_SETADDRESS, 0, mask);

			// upgrade value in edit control
			SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

			return TRUE;
		}
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
		HWND hTextControl = GetDlgItem(hwnd, IDC_STATIC_INFO);
		DWORD dwIPaddress = 0;
		DWORD dwIPmask = 0;
		DWORD dwIPnetwork = 0;
		

		switch (LOWORD(wParam))
		{
		case IDC_IPADDRESS:
			if (HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				if (FIRST_IPADDRESS(dwIPaddress) < 128) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFF000000);
				else if (FIRST_IPADDRESS(dwIPaddress) < 192) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFF0000);
				else if (FIRST_IPADDRESS(dwIPaddress) < 224) SendMessage(hIPmask, IPM_SETADDRESS, 0, 0xFFFFFF00);

				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				int prefix = MaskToPrefix(dwIPmask);
				SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);
				dwIPnetwork = NetworkAddress(dwIPaddress, prefix);
				
				FillTextControl(hTextControl, dwIPaddress, prefix, dwIPmask, dwIPnetwork);
			}
		break;
		case IDC_IPMASK:
			if(HIWORD(wParam) == EN_CHANGE)
			{
				SendMessage(hIPaddress, IPM_GETADDRESS, 0, (LPARAM)&dwIPaddress);
				SendMessage(hIPmask, IPM_GETADDRESS, 0, (LPARAM)&dwIPmask);
				int prefix = MaskToPrefix(dwIPmask);
				SetDlgItemInt(hwnd, IDC_EDIT_PREFIX, prefix, FALSE);

				FillTextControl(hTextControl, dwIPaddress, prefix, dwIPmask, dwIPnetwork);
			}
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

int MaskToPrefix(DWORD mask)
{
	int prefix = 0;
	for (int i = 31; i >= 0; i--)
	{
		if (CHECK_BIT(mask,i))
		/*if((mask >> i) & 1 )*/
		{
			prefix++;
		}
		else break;
	}
	return prefix;
}

char* NumberIPnetOrHost(DWORD mask, BOOL host)
{
	char strNum[256] = { 0 };
	int zeroBits = 0;
	for (int i = 31; i >= 0; i--)
	{
		if(!((mask >> i) & 1))
		{
			zeroBits++;
		}
	}

	int numIP = (zeroBits == 0) ? 1 : (1 << zeroBits); // 2^zeroBits
	if (host)
	{
		int prefix = MaskToPrefix(mask);
		if (prefix == 32) numIP = 1;
		else if (prefix == 31) numIP = 2;
		else numIP = numIP - 2;
	}
	sprintf(strNum,"%d",numIP);
	return strNum;
}

DWORD PrefixToMask(int prefix)
{
	return (0xFFFFFFFF << (32 - prefix));
}

DWORD NetworkAddress(DWORD ip, int prefix)
{
	DWORD mask = PrefixToMask(prefix);
	return ip & mask;
}

DWORD BroadcastAddress(DWORD ip, int prefix)
{
	DWORD mask = PrefixToMask(prefix);
	return ip | ~mask;
}

LPSTR IpToString(DWORD ip)
{
	BYTE* ipBytes = (BYTE*)&ip;
	char ipStr[32];
	sprintf(ipStr, "%d.%d.%d.%d", ipBytes[3], ipBytes[2], ipBytes[1], ipBytes[0]);
	return (LPSTR)ipStr;
}

void FillTextControl(HWND hTxtCntrl, DWORD dwIPaddr, int pref, DWORD dwIPmask, DWORD dwIPnet)
{

	struct { const char* text; char textIp[32]; } strings[] = 
	{
		{"Адрес сети: ", ""},
		{"Широковещательный адрес: ", ""},
		{"Количество IP-aдресов в этой сети: ", ""},
		{"Количество узлов текущей сети: ", ""}
	};
	
	SendMessageA(hTxtCntrl, WM_SETTEXT, 0, (LPARAM)"Info:");
	char buff[256];
	SendMessageA(hTxtCntrl, WM_GETTEXT, (WPARAM)256, (LPARAM)buff);
	strcat(buff, "\n");

	for (int i = 0; i <= 3; i++)
	{

		int variant = i%4;
		SendMessageA(hTxtCntrl, WM_GETTEXT, (WPARAM)256, (LPARAM)buff);
		strcat(buff, "\n");
		strcat(buff, strings[i].text);
		switch (variant)
		{
			case 0:
				strcat(buff, "\t\t\t\t");
				strcat(buff, IpToString(dwIPnet));
				break;
			case 1:
				strcat(buff, "\t\t");
				strcat(buff, IpToString(BroadcastAddress(dwIPaddr, pref)));
				break;
			case 2:
				strcat(buff, "\t");
				strcat(buff, NumberIPnetOrHost(dwIPmask, 0));
				break;
			case 3:
				strcat(buff, "\t");
				strcat(buff, NumberIPnetOrHost(dwIPmask, 1));
				break;
		}
		SendMessageA(hTxtCntrl, WM_SETTEXT, 0, (LPARAM)(LPSTR)buff);
	}
}