#pragma once
#include<Windows.h>
#include<stdio.h>
#include<atomic>


LPSTR FormatLastError(DWORD dwMessageID);
VOID PrintLastError(DWORD dwMessageID);

extern std::atomic<bool> g_running; // extern нужен если возникает ошибка  LNK1169
BOOL WINAPI ConsoleClosed(DWORD signal);