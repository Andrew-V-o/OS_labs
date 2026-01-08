#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

HANDLE start_process(string app_name, HANDLE hIn, HANDLE hOut) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.dwFlags |= STARTF_USESTDHANDLES;

    SetHandleInformation(hIn, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    SetHandleInformation(hOut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

    if (!CreateProcessA(app_name.c_str(), NULL, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return NULL;
    }
    CloseHandle(pi.hThread);
    return pi.hProcess;
}

int main() {
    const char* apps[] = { "taskA.exe", "taskM.exe", "taskP.exe", "taskS.exe" };
    HANDLE hP[5][2];
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, TRUE }; 

    for (int i = 0; i < 5; i++) {
        CreatePipe(&hP[i][0], &hP[i][1], &sa, 0);
    }

    vector<HANDLE> procs;
    for (int i = 0; i < 4; i++) {
        HANDLE h = start_process(apps[i], hP[i][0], hP[i+1][1]);
        if (h) procs.push_back(h);
        
        CloseHandle(hP[i][0]);
        CloseHandle(hP[i+1][1]);
    }

    cout << "Enter numbers separated by spaces: ";
    string input;
    getline(cin, input);
    input += "\n";

    DWORD written;
    WriteFile(hP[0][1], input.c_str(), (DWORD)input.length(), &written, NULL);
    CloseHandle(hP[0][1]);

    char buf[256] = {0};
    DWORD read;
    if (ReadFile(hP[4][0], buf, sizeof(buf)-1, &read, NULL)) {
        cout << "Result: " << buf;
    }

    WaitForMultipleObjects((DWORD)procs.size(), procs.data(), TRUE, INFINITE);
    for (HANDLE h : procs) CloseHandle(h);
    CloseHandle(hP[4][0]);

    return 0;
}