#pragma once
#include <Windows.h>
#include <iostream>
#include <string>
#include <codecvt>

int runcommand(std::wstring commandline, std::wstring workingdir) {
    std::wstring mutableCmd = commandline;

    STARTUPINFOW si{};
    si.cb = sizeof(si);

    PROCESS_INFORMATION pi{};
	
    BOOL created = ::CreateProcessW(
        nullptr,
        mutableCmd.data(),
        nullptr, nullptr, FALSE,
        CREATE_NEW_CONSOLE,
        nullptr,
        workingdir.c_str(),
        &si,
        &pi
    );

    if (!created) {
        DWORD err = ::GetLastError();
        std::wcerr << L"Failed to launch process. error=" << err << std::endl;
        return 1;
    }
	
    ::WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exitCode = 1;
    ::GetExitCodeProcess(pi.hProcess, &exitCode);

    std::wcout << L"Process exited with code: " << exitCode << std::endl;

    ::CloseHandle(pi.hThread);
    ::CloseHandle(pi.hProcess);

    return 0;
}


std::wstring towstring(const std::string in) {
	size_t i;
	wchar_t* buf = new wchar_t[in.size() + 1];
	mbstowcs_s(&i, buf, in.size() + 1, in.c_str(), _TRUNCATE);
	std::wstring res = buf;
	delete[] buf;
	return res;
}

int compile(std::string dir) {
    std::wstring commandLine = LR"(cmd.exe /c launch.bat & pause)";
	
	return runcommand(commandLine, towstring(dir));
}
