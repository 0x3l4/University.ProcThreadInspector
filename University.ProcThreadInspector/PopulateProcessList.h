#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>

void PopulateProcessesList(HWND hList);
void PopulateThreadsList(HWND hList, DWORD processId);
void InitProcessListView(HWND hWndListView);
void InitThreadListView(HWND hWndListView);