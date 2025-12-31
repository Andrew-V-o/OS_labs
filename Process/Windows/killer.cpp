#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;

void killByPID(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess)
    {
        TerminateProcess(hProcess, 0);
        CloseHandle(hProcess);
        cout << "Killed PID: " << pid << "\n";
    }
}

void killByName(const string& name)
{
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe{};
    pe.dwSize = sizeof(pe);

    if (Process32First(snap, &pe))
    {
        do
        {
            if (_stricmp(pe.szExeFile, name.c_str()) == 0)
            {
                killByPID(pe.th32ProcessID);
            }
        } while (Process32Next(snap, &pe));
    }
    CloseHandle(snap);
}

vector<string> split(const string& s)
{
    vector<string> result;
    stringstream ss(s);
    string item;
    while (getline(ss, item, ','))
        result.push_back(item);
    return result;
}

int main(int argc, char* argv[])
{

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--id") == 0 && i + 1 < argc)
            killByPID(stoi(argv[++i]));
        else if (strcmp(argv[i], "--name") == 0 && i + 1 < argc)
            killByName(argv[++i]);
    }


    char buffer[1024];
    DWORD len = GetEnvironmentVariableA("PROC_TO_KILL", buffer, 1024);
    if (len > 0)
    {
        auto names = split(buffer);
        for (auto& n : names)
            killByName(n);
    }

    return 0;
}
