#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

const int N = 6;

void run_M() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << (x * 7) << " ";
        cout << endl;
    }
}

void run_A() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << (x + N) << " ";
        cout << endl;
    }
}

void run_P() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << (long long)x * x * x << " ";
        cout << endl;
    }
}

void run_S() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        long long sum = 0;
        int x;
        while (ss >> x) sum += x;
        cout << sum << endl;
    }
}

HANDLE start(string cmd_line, HANDLE hIn, HANDLE hOut) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.hStdInput = hIn;
    si.hStdOutput = hOut;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
    si.dwFlags |= STARTF_USESTDHANDLES;

    SetHandleInformation(hIn, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    SetHandleInformation(hOut, HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);

    vector<char> cmd(cmd_line.begin(), cmd_line.end());
    cmd.push_back('\0');

    if (!CreateProcessA(NULL, cmd.data(), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
        return NULL;
    }
    CloseHandle(pi.hThread);
    return pi.hProcess;
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        string mode = argv[1];
        if (mode == "M") run_M();
        else if (mode == "A") run_A();
        else if (mode == "P") run_P();
        else if (mode == "S") run_S();
        return 0;
    }

    char self_path[MAX_PATH];
    if (!GetModuleFileNameA(NULL, self_path, MAX_PATH)) return 1;

    HANDLE hP[5][2];
    SECURITY_ATTRIBUTES sa = { sizeof(sa), NULL, FALSE };

    for (int i = 0; i < 5; i++) {
        if (!CreatePipe(&hP[i][0], &hP[i][1], &sa, 0)) return 1;
    }

    vector<HANDLE> procs;
    const char* modes[] = {"A", "M", "P", "S"};
    
    for (int i = 0; i < 4; i++) {
        HANDLE h = start(string(self_path) + " " + modes[i], hP[i][0], hP[i+1][1]);
        if (h) procs.push_back(h);
    }

    for (int i = 0; i < 4; i++) {
        CloseHandle(hP[i][0]);
        CloseHandle(hP[i + 1][1]);
    }

    cout << "Enter numbers separated by spaces: ";
    string input_data;
    if (!getline(cin, input_data)) return 0;
    input_data += "\n";

    DWORD written;
    if (!WriteFile(hP[0][1], input_data.c_str(), (DWORD)input_data.length(), &written, NULL)) return 1;
    CloseHandle(hP[0][1]);

    char buf[256];
    DWORD bytesRead;
    if (ReadFile(hP[4][0], buf, sizeof(buf) - 1, &bytesRead, NULL)) {
        buf[bytesRead] = '\0';
        cout << "Result: " << buf;
    }

    if (!procs.empty()) {
        WaitForMultipleObjects((DWORD)procs.size(), procs.data(), TRUE, INFINITE);
        for (HANDLE h : procs) CloseHandle(h);
    }
    CloseHandle(hP[4][0]);

    return 0;
}