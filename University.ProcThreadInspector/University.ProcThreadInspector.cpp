#include <Windows.h>
#include <TlHelp32.h>
#include "framework.h"
#include "University.ProcThreadInspector.h"
#include "PopulateProcessList.h"
#include <commctrl.h>

// Определение макроса для максимальной длины строки загрузки (заголовка окна)
#define MAX_LOADSTRING 100

// Глобальная переменная экземпляра приложения
HINSTANCE hInst;
// Массив символов для хранения заголовка окна
WCHAR szTitle[MAX_LOADSTRING];
// Массив символов для хранения имени класса окна
WCHAR szWindowClass[MAX_LOADSTRING];

// Функция регистрации класса окна
ATOM                MyRegisterClass(HINSTANCE hInstance);
// Функция инициализации экземпляра приложения
BOOL                InitInstance(HINSTANCE, int);
// Функция обработки сообщений окна
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// Функция обработки сообщений диалогового окна "О программе"
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// Дескриптор окна списка процессов
HWND hListProcesses;
// Дескриптор окна списка потоков
HWND hListThreads; 

// Главная точка входа для приложений Windows
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{

	UNREFERENCED_PARAMETER(hPrevInstance); // Игнорируем неиспользуемый параметр
	UNREFERENCED_PARAMETER(lpCmdLine); // Игнорируем командную строку

	// Загружаем строку заголовка окна из ресурсов
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	// Загружаем имя класса окна из ресурсов
	LoadStringW(hInstance, IDC_UNIVERSITYPROCTHREADINSPECTOR, szWindowClass, MAX_LOADSTRING);
	// Регистрируем класс окна
	MyRegisterClass(hInstance);

	// Инициализируем приложение
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE; // Если инициализация не удалась, выходим с ошибкой
	}

	// Загружаем таблицу ускорителей (горячих клавиш)
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_UNIVERSITYPROCTHREADINSPECTOR));

	// Структура для хранения сообщений
	MSG msg;

	// Основной цикл обработки сообщений
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		// Проверяем, есть ли горячие клавиши для данного сообщения
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg); // Переводим виртуальные коды клавиш в символьные сообщения
			DispatchMessage(&msg); // Отправляем сообщение соответствующей процедуре окна
		}
	}

	// Возвращаем код завершения приложения
	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	// Определяем структуру для описания класса окна
	WNDCLASSEXW wcex;

	// Устанавливаем размер структуры
	wcex.cbSize = sizeof(WNDCLASSEX);

	// Задаём стиль класса окна
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	// Указываем функцию обратного вызова для обработки сообщений окна
	wcex.lpfnWndProc = WndProc;
	// Дополнительное пространство для класса окна (здесь не нужно)
	wcex.cbClsExtra = 0;
	// Дополнительное пространство для каждого окна (здесь не нужно)
	wcex.cbWndExtra = 0;
	// Присваиваем текущий экземпляр приложения
	wcex.hInstance = hInstance;
	// Загружаем большой значок для окна
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_UNIVERSITYPROCTHREADINSPECTOR));
	// Загружаем стандартный курсор для окна
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	// Устанавливаем цвет фона окна
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	// Присваиваем меню для окна
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_UNIVERSITYPROCTHREADINSPECTOR);
	// Присваиваем имя класса окна
	wcex.lpszClassName = szWindowClass;
	// Загружаем маленький значок для окна
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Регистрируем класс окна
	return RegisterClassExW(&wcex);
}

// Функция инициализирует первый экземпляр окна приложения
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	// Сохраняем экземпляр приложения в глобальной переменной
	hInst = hInstance;

	// Создаем главное окно приложения
	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	// Проверяем успешность создания окна
	if (!hWnd)
	{
		return FALSE; // Создание окна не удалось
	}

	// Отображаем окно с заданным параметром отображения
	ShowWindow(hWnd, nCmdShow);
	// Обновляем клиентскую область окна
	UpdateWindow(hWnd);

	return TRUE; // Успешная инициализация
}

// Основная функция обработки сообщений окна
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE: // Сообщение отправляется при создании окна
		// Создаем дочернее окно-список процессов
		hListProcesses = CreateWindowW(WC_LISTVIEW, NULL,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
			20, 20, 350, 500,
			hWnd, (HMENU)IDC_LIST_PROCESSES, NULL, NULL);

		// Создаем дочернее окно-список потоков
		hListThreads = CreateWindowW(WC_LISTVIEW, NULL,
			WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SINGLESEL,
			400, 20, 200, 500,
			hWnd, (HMENU)IDC_LIST_THREADS, NULL, NULL);

		// Инициализируем списки процессов и потоков
		InitProcessListView(hListProcesses);
		InitThreadListView(hListThreads);

		// Заполняем список процессов
		PopulateProcessesList(hListProcesses);
		break;	
	case WM_PAINT: // Сообщение приходит при необходимости перерисовки окна
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps); // Начинаем отрисовку
		EndPaint(hWnd, &ps); // Заканчиваем отрисовку
	}
	break;
	case WM_DESTROY: // Сообщение приходит при уничтожении окна
		PostQuitMessage(0); // Завершение работы программы
		break;
	case WM_COMMAND: // Сообщение приходит при выборе пункта меню или нажатии кнопки
	{
		int wmId = LOWORD(wParam); // Идентифицируем команду

		// Проверяем изменение выбора в списке процессов
		if (LOWORD(wParam) == IDC_LIST_PROCESSES && HIWORD(wParam) == LBN_SELCHANGE) {
			int index = (int)SendMessage(hListProcesses, LB_GETCURSEL, 0, 0);
			if (index != LB_ERR) { // Если элемент выбран
				wchar_t buffer[256]; // Буфер для хранения текста
				// Получаем текст выбранного элемента
				SendMessage(hListProcesses, LB_GETTEXT, index, (LPARAM)buffer);

				DWORD processID = _wtoi(buffer);  // Получаем PID
				// Заполняем список потоков для выбранного процесса
				PopulateThreadsList(hListThreads, processID);
			}
		}

		// Реагируем на команды из меню
		switch (wmId)
		{
		case IDM_ABOUT:  // Команда "О программе"
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT: // Команда выхода
			DestroyWindow(hWnd);
			break;

		case IDM_REFRESH:  // Обновление списка
			// Обновляем список процессов
			PopulateProcessesList(hListProcesses);
			// Удаляем все элементы из списка потоков
			ListView_DeleteAllItems(hListThreads);
		}
	}

	break;
	case WM_NOTIFY: // Сообщение приходит от элементов управления
		if (((LPNMHDR)lParam)->hwndFrom == hListProcesses && ((LPNMHDR)lParam)->code == LVN_ITEMCHANGED) {
			NMLISTVIEW* pNMLV = (NMLISTVIEW*)lParam; // Получаем указатель на структуру уведомлений списка
			if (pNMLV->uNewState & LVIS_SELECTED) {  // Проверяем, был ли выделен новый элемент
				wchar_t buffer[10]; // Буфер для хранения текста
				// Получаем текст выбранного элемента
				ListView_GetItemText(hListProcesses, pNMLV->iItem, 0, buffer, sizeof(buffer));
				// Преобразуем текст в числовой идентификатор процесса
				DWORD processID = _wtoi(buffer);
				// Заполняем список потоков для выбранного процесса
				PopulateThreadsList(hListThreads, processID);
			}
		}
		break;
	default: // Для всех остальных сообщений используем стандартную обработку
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0; // Для всех остальных сообщений используем стандартную обработку
}

// Функция обработки сообщений для диалогового окна "О программе"
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Игнорируем неиспользуемый параметр
	UNREFERENCED_PARAMETER(lParam); 
	switch (message)
	{
	case WM_INITDIALOG: // Сообщение приходит при инициализации диалогового окна
		return (INT_PTR)TRUE; // Диалог успешно инициализирован

	case WM_COMMAND: // Сообщение приходит при взаимодействии с элементами диалога
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam)); // Закрываем диалоговое окно
			return (INT_PTR)TRUE; // Обработка выполнена успешно
		}
		break;
	}
	return (INT_PTR)FALSE;  // Сообщение не обработано
}