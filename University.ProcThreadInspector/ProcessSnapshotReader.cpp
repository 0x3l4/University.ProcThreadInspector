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
	// ������ ������ ���������
	HANDLE SnapShot_Pr = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (SnapShot_Pr == INVALID_HANDLE_VALUE) {
		MessageBox(NULL, L"�� ���� ��������� ����� ���������", L"������", MB_OK | MB_ICONERROR);
		return;
	}

	PROCESSENTRY32 ProcEntry;
	ProcEntry.dwSize = sizeof(PROCESSENTRY32);

	// ��������� ������ ������� �� ������
	if (!Process32First(SnapShot_Pr, &ProcEntry)) {
		MessageBox(NULL, L"������ ������ ������ ���������", L"������", MB_OK | MB_ICONERROR);
		CloseHandle(SnapShot_Pr);
		return;
	}

	// ������ ������ � ��������
	DWORD ProcID = ProcEntry.th32ProcessID;
	std::wstring ProcName = ProcEntry.szExeFile;
	DWORD ProcThreadcount = ProcEntry.cntThreads;
	int i = 1;

	// ���� ���� ��������, ���������� ���������
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


