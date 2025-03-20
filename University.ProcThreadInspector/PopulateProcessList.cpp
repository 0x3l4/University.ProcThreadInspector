#include "PopulateProcessList.h"
#include <commctrl.h>

// Функция заполняет список процессов в предоставленном окне списка (list view).
void PopulateProcessesList(HWND hList)
{
    // Сначала удаляем все существующие элементы из списка
    ListView_DeleteAllItems(hList);

    // Массив для хранения идентификаторов процессов
    DWORD processes[1024];
    DWORD needed;  // Переменная для хранения количества байтов, необходимых для получения всех идентификаторов процессов

    // Получаем массив идентификаторов активных процессов
    if (EnumProcesses(processes, sizeof(processes), &needed))
    {
        // Вычисляем количество найденных процессов
        int count = needed / sizeof(DWORD);

        // Структура для добавления новых элементов в список
        LVITEM lvI;

        // Проходим по каждому процессу
        for (int i = 0; i < count; i++)
        {
            DWORD pid = processes[i];  // Идентификатор процесса
            if (pid == 0) continue;   // Пропускаем нулевые значения

            // Открываем процесс для получения дополнительной информации
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
            if (hProcess)
            {
                // По умолчанию имя процесса неизвестно
                wchar_t processName[MAX_PATH] = L"<Unknown>";

                // Получаем имя исполняемого файла процесса
                GetModuleBaseName(hProcess, NULL, processName, MAX_PATH);

                // Подсчитываем количество потоков в процессе
                DWORD threadCount = 0;
                HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnap != INVALID_HANDLE_VALUE)
                {
                    PROCESSENTRY32 pe;
                    pe.dwSize = sizeof(pe);
                    if (Process32First(hSnap, &pe))
                    {
                        do {
                            if (pe.th32ProcessID == pid) {
                                threadCount = pe.cntThreads;
                                break;
                            }
                        } while (Process32Next(hSnap, &pe));
                    }
                    CloseHandle(hSnap);
                }

                // Преобразовываем идентификаторы в строки
                wchar_t pidStr[10], threadStr[10];
                swprintf(pidStr, 10, L"%d", pid);
                swprintf(threadStr, 10, L"%d", threadCount);

                // Готовим структуру для вставки нового элемента в список
                lvI.mask = LVIF_TEXT;
                lvI.iItem = ListView_GetItemCount(hList);  // Новый элемент будет последним
                lvI.iSubItem = 0;                         // Первая колонка (идентификатор процесса)
                lvI.pszText = pidStr;

                // Вставляем новый элемент в список
                ListView_InsertItem(hList, &lvI);

                // Добавляем дополнительные столбцы с именем процесса и количеством потоков
                ListView_SetItemText(hList, lvI.iItem, 1, processName);
                ListView_SetItemText(hList, lvI.iItem, 2, threadStr);

                // Закрываем дескриптор процесса
                CloseHandle(hProcess);
            }
        }
    }
}

// Функция заполняет список потоков для указанного процесса в предоставленное окно списка.
void PopulateThreadsList(HWND hList, DWORD processId)
{
    // Очищаем существующий список
    ListView_DeleteAllItems(hList);

    // Создаём снимок всех потоков системы
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hSnap == INVALID_HANDLE_VALUE) return;  // Если снимок не получен, прерываем выполнение

    // Структура для получения информации о потоке
    THREADENTRY32 te;
    te.dwSize = sizeof(te);

    // Структура для добавления нового элемента в список
    LVITEM lvI;

    // Проходим по каждому потоку в снимке
    if (Thread32First(hSnap, &te))
    {
        do {
            if (te.th32OwnerProcessID == processId)  // Фильтруем потоки по указанному процессу
            {
                // Преобразовываем идентификаторы потока и процесса в строки
                wchar_t tidStr[10], pidStr[10];
                swprintf(tidStr, 10, L"%d", te.th32ThreadID);
                swprintf(pidStr, 10, L"%d", te.th32OwnerProcessID);

                // Готовим структуру для вставки нового элемента в список
                lvI.mask = LVIF_TEXT;
                lvI.iItem = ListView_GetItemCount(hList);  // Новый элемент будет последним
                lvI.iSubItem = 0;                         // Первая колонка (идентификатор потока)
                lvI.pszText = tidStr;

                // Вставляем новый элемент в список
                ListView_InsertItem(hList, &lvI);

                // Добавляем дополнительный столбец с идентификатором процесса
                ListView_SetItemText(hList, lvI.iItem, 1, pidStr);
            }
        } while (Thread32Next(hSnap, &te));  // Переходим к следующему потоку
    }

    // Закрываем дескриптор снимка
    CloseHandle(hSnap);
}

// Функция инициализирует список процессов, добавляя необходимые колонки.
void InitProcessListView(HWND hWndListView)
{
    // Включаем режим выделения всей строки
    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

    // Определяем параметры колонок
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;  // Маска для настройки текста, ширины и индекса колонки

    // Колонка с идентификаторами процессов
    lvc.cx = 80;           // Ширина колонки
    lvc.pszText = (LPWSTR)L"PID";  // Название колонки
    ListView_InsertColumn(hWndListView, 0, &lvc);  // Вставка первой колонки

    // Колонка с именами процессов
    lvc.cx = 180;          // Ширина колонки
    lvc.pszText = (LPWSTR)L"Имя процесса";  // Название колонки
    ListView_InsertColumn(hWndListView, 1, &lvc);  // Вставка второй колонки

    // Колонка с количеством потоков
    lvc.cx = 100;          // Ширина колонки
    lvc.pszText = (LPWSTR)L"Потоков";  // Название колонки
    ListView_InsertColumn(hWndListView, 2, &lvc);  // Вставка третьей колонки
}

// Функция инициализирует список потоков, добавляя необходимые колонки.
void InitThreadListView(HWND hWndListView)
{
    // Включаем режим выделения всей строки
    ListView_SetExtendedListViewStyle(hWndListView, LVS_EX_FULLROWSELECT);

    // Определяем параметры колонок
    LVCOLUMN lvc;
    lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;  // Маска для настройки текста, ширины и индекса колонки

    // Колонка с идентификаторами потоков
    lvc.cx = 100;           // Ширина колонки
    lvc.pszText = (LPWSTR)L"TID";  // Название колонки
    ListView_InsertColumn(hWndListView, 0, &lvc);  // Вставка первой колонки

    // Колонка с идентификаторами процессов
    lvc.cx = 100;           // Ширина колонки
    lvc.pszText = (LPWSTR)L"PID процесса";  // Название колонки
    ListView_InsertColumn(hWndListView, 1, &lvc);  // Вставка второй колонки
}
