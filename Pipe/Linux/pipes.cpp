#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <sys/wait.h>
#include <cmath>

using namespace std;

const int N = 6;

void run_M() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << x * 7 << " ";
        cout << endl;
    }
}

void run_A() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << x + N << " ";
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

void start_proc(const char* self, const char* mode, int in_fd, int out_fd, int all_pipes[][2], int count) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }
    if (pid == 0) {
        if (dup2(in_fd, STDIN_FILENO) < 0) { perror("dup2"); _exit(1); }
        if (dup2(out_fd, STDOUT_FILENO) < 0) { perror("dup2"); _exit(1); }

        for (int i = 0; i < count; i++) {
            close(all_pipes[i][0]);
            close(all_pipes[i][1]);
        }

        execl(self, self, mode, nullptr);
        perror("execl");
        _exit(1);
    }
}

int main(int argc, char* argv[]) {
    if (argc > 1) {
        string mode = argv[1];
        if (mode == "A") run_A();
        else if (mode == "M") run_M();
        else if (mode == "P") run_P();
        else if (mode == "S") run_S();
        return 0;
    }

    int p[5][2];
    for (int i = 0; i < 5; i++) {
        if (pipe(p[i]) < 0) {
            perror("pipe");
            return 1;
        }
    }

    start_proc(argv[0], "A", p[0][0], p[1][1], p, 5);
    start_proc(argv[0], "M", p[1][0], p[2][1], p, 5);
    start_proc(argv[0], "P", p[2][0], p[3][1], p, 5);
    start_proc(argv[0], "S", p[3][0], p[4][1], p, 5);

    for (int i = 0; i < 5; i++) {
        if (i != 0) close(p[i][1]);
        if (i != 4) close(p[i][0]);
    }

    cout << "Enter numbers separated by spaces: ";
    string input;
    if (!getline(cin, input)) return 0;
    input += "\n";

    if (write(p[0][1], input.c_str(), input.size()) < 0) perror("write");
    close(p[0][1]);

    char buffer[256];
    ssize_t n = read(p[4][0], buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        cout << "Result: " << buffer;
    }
    close(p[4][0]);

    for (int i = 0; i < 4; i++) {
        int status;
        wait(&status);
    }

    return 0;
}