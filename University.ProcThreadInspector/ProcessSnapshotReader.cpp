#include "ProcessSnapshotReader.h"
#include <Windows.h>
#include <TlHelp32.h>
#include "framework.h"
#include <string>
#include <iostream>

ProcessSnapshotReader::ProcessSnapshotReader() 
{

}

void ProcessSnapshotReader::Read()
{
	// Создаём снимок процессов
	HANDLE SnapShot_Pr = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot_Pr == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"Не могу прочитать карту процессов", L"Ошибка", MB_OK | MB_ICONERROR);
		return;
	}

	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);

	// Считываем первый процесс из списка
	if (!Process32First(SnapShot_Pr, &ProcEntry)) {
		MessageBox(NULL, L"Ошибка чтения списка процессов", L"Ошибка", MB_OK | MB_ICONERROR);
		CloseHandle(SnapShot_Pr);
		return;
	}

	// Читаем данные о процессе
	DWORD ProcID = ProcEntry.th32ProcessID;
	std::wstring ProcName = ProcEntry.szExeFile;
	DWORD ProcThreadcount = ProcEntry.cntThreads;
	int i = 1;

	// Пока есть процессы, продолжаем считывать
	while (Process32Next(SnapShot_Pr, &ProcEntry))
	{
		i++;
		ProcID = ProcEntry.th32ProcessID;
		ProcName = ProcEntry.szExeFile;
		ProcThreadcount = ProcEntry.cntThreads;
		std::cout << ProcID << " " << ProcName.c_str() << std:: endl;
	}

	CloseHandle(SnapShot_Pr);
}


