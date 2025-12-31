#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

void runKiller(const string& args)
{
    STARTUPINFOA si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);

    string cmd = "Killer.exe " + args;

    vector<char> buffer(cmd.begin(), cmd.end());
    buffer.push_back('\0');

    BOOL result = CreateProcessA(
        nullptr,
        buffer.data(),
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );

    if (!result)
    {
        cerr << "CreateProcess failed. Error: "
                  << GetLastError() << "\n";
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

int main()
{
    SetEnvironmentVariableA("PROC_TO_KILL", "notepad.exe");
    cout << "PROC_TO_KILL set\n";

    runKiller("--name notepad.exe");
    runKiller("");

    SetEnvironmentVariableA("PROC_TO_KILL", nullptr);
    cout << "PROC_TO_KILL removed\n";

    return 0;
}
