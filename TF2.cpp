// TF2.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <vector>
#include <Windows.h>
#include "proc.h"
#include "mem.h"


int main()
{
	HANDLE hProcess = 0;

	uintptr_t moduleBase = 0, localPlayerPtr = 0, animAddr = 0;

	int baseAnim = 34;
	int animValue;

	DWORD procId = GetProcId(L"hl2.exe"); //get process id from name of window

	if (procId)
	{
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId); //opens process with admin rights	

		moduleBase = GetModuleBaseAddress(procId, L"client.dll");

		//localPlayerPtr = moduleBase + 0x96C024; //gets local player pointer
		localPlayerPtr = moduleBase + 0x00C614BC;
		//healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf4 }); //gets relative address based off of location of pointers
		animAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0x4, 0x0, 0x4e8, 0xf4 });

		mem::PullEx((BYTE*)animAddr, (BYTE*)&animValue, sizeof(animValue), hProcess);
		baseAnim = animValue;

		std::cout << "Anim is currently: " << animValue;
	}
	else
	{
		std::cout << "Process not found, press enter to exit \n";
		getchar();
		return 0;
	}

	DWORD dwExit = 0;

	HWND wnd;
	HDC dc;

	bool isStab = false;
	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)
	{
		//low-latency hacks
		if (animValue == baseAnim + 5 && isStab == false) //have early so no delay between stab
		{
			INPUT Inputs[2] = { 0 };

			Inputs[0].type = INPUT_MOUSE;
			Inputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

			Inputs[1].type = INPUT_MOUSE;
			Inputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

			SendInput(2, Inputs, sizeof(INPUT));
			isStab = true;
		}
		else
		{
			if (animValue != baseAnim + 5)
			{
				isStab = false;
			}
		}
		
		//draw to screen
		
		
		/*wnd = GetForegroundWindow();

		dc = GetDC(wnd);

		Rectangle(dc, 0, 0, 200, 200);
		TextOut(dc, 10, 10, TEXT("Test sentence"), 16);
		ReleaseDC(wnd, dc);*/


		if (GetAsyncKeyState(VK_NUMPAD5) & 1)
		{
			return 0; //exits hack
		}

		if (GetAsyncKeyState(VK_NUMPAD0) & 1)
		{
			moduleBase = GetModuleBaseAddress(procId, L"client.dll");
			localPlayerPtr = moduleBase + 0x00C614BC;
			animAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0x4, 0x0, 0x4e8, 0xf4 });

			mem::PullEx((BYTE*)animAddr, (BYTE*)&animValue, sizeof(animValue), hProcess);
			baseAnim = animValue;
			std::cout << "Anim is currently: " << animValue << std::endl;
		}

		mem::PullEx((BYTE*)animAddr, (BYTE*)&animValue, sizeof(animValue), hProcess);

		Sleep(10); //sleeps for 10ms
	}

	std::cout << "Process not found, press enter to exit\n";
	getchar();
	return 0;
}
