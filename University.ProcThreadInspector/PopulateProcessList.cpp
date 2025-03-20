#include "PopulateProcessList.h"
#include <commctrl.h>

void PopulateProcessesList(HWND hList)
{
    ListView_DeleteAllItems(hList);

    DWORD processes[1024], needed;
    if (EnumProcesses(processes, sizeof(processes), &needed)) {
        int count = needed / sizeof(DWORD);
        LVITEM lvI;

        for (int i = 0; i < count; i++) {
            DWORD pid = processes[i];
            if (pid == 0) continue;

            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (hProcess) {
                wchar_t processName[MAX_PATH] = L"<Unknown>";
                GetModuleBaseName(hProcess, NULL, processName, MAX_PATH);

                DWORD threadCount = 0;
                HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnap != INVALID_HANDLE_VALUE) {
                    PROCESSENTRY32 pe;
                    pe.dwSize = sizeof(pe);
                    if (Process32First(hSnap, &pe)) {
                        do {
                            if (pe.th32ProcessID == pid) {
                                threadCount = pe.cntThreads;
                                break;
                            }
                        } while (Process32Next(hSnap, &pe));
                    }
                    CloseHandle(hSnap);
                }

                wchar_t pidStr[10], threadStr[10];
                swprintf(pidStr, 10, L"%d", pid);
                swprintf(threadStr, 10, L"%d", threadCount);

                lvI.mask = LVIF_TEXT;
                lvI.iItem = ListView_GetItemCount(hList);
                lvI.iSubItem = 0;
                lvI.pszText = pidStr;
                ListView_InsertItem(hList, &lvI);

                ListView_SetItemText(hList, lvI.iItem, 1, processName);
                ListView_SetItemText(hList, lvI.iItem, 2, threadStr);

                CloseHandle(hProcess);
            }
        }
    }
}

void PopulateThreadsList(HWND hList, DWORD processId)
{
    ListView_DeleteAllItems(hList);

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;

    THREADENTRY32 te;
    te.dwSize = sizeof(te);
    LVITEM lvI;

    if (Thread32First(hSnap, &te)) {
        do {
            if (te.th32OwnerProcessID == processId) {
                wchar_t tidStr[10], pidStr[10];
                swprintf(tidStr, 10, L"%d", te.th32ThreadID);
                swprintf(pidStr, 10, L"%d", te.th32OwnerProcessID);

                lvI.mask = LVIF_TEXT;
                lvI.iItem = ListView_GetItemCount(hList);
                lvI.iSubItem = 0;
                lvI.pszText = tidStr;
                ListView_InsertItem(hList, &lvI);

                ListView_SetItemText(hList, lvI.iItem, 1, pidStr);
            }
        } while (Thread32Next(hSnap, &te));
    }

    CloseHandle(hSnap);
}

void InitProcessListView(HWND hWndListView) {
    LVCOLUMN lvc;
    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.cx = 80; lvc.pszText = (LPWSTR)L"PID";
    ListView_InsertColumn(hWndListView, 0, &lvc);

    lvc.cx = 180; lvc.pszText = (LPWSTR)L"Имя процесса";
    ListView_InsertColumn(hWndListView, 1, &lvc);

    lvc.cx = 100; lvc.pszText = (LPWSTR)L"Потоков";
    ListView_InsertColumn(hWndListView, 2, &lvc);
}

void InitThreadListView(HWND hWndListView) {
    LVCOLUMN lvc;
    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    lvc.cx = 100; lvc.pszText = (LPWSTR)L"TID";
    ListView_InsertColumn(hWndListView, 0, &lvc);

    lvc.cx = 100; lvc.pszText = (LPWSTR)L"PID процесса";
    ListView_InsertColumn(hWndListView, 1, &lvc);
}

