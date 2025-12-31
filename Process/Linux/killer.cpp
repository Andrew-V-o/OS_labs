#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <cstring>

using namespace std;

void killByPID(pid_t pid) {
    kill(pid, SIGKILL);
}

string getProcessName(pid_t pid) {
    string path = "/proc/" + to_string(pid) + "/comm";
    ifstream file(path);
    string name;
    if (file) getline(file, name);
    return name;
}

void killByName(const string& name) {
    DIR* dir = opendir("/proc");
    if (!dir) return;
    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            pid_t pid = atoi(entry->d_name);
            if (getProcessName(pid) == name) killByPID(pid);
        }
    }
    closedir(dir);
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--id") == 0 && i + 1 < argc)
            killByPID(stoi(argv[++i]));
        else if (strcmp(argv[i], "--name") == 0 && i + 1 < argc)
            killByName(argv[++i]);
    }

    if (char* env = getenv("PROC_TO_KILL")) {
        stringstream ss(env);
        string name;
        while (getline(ss, name, ',')) {
            if (!name.empty()) killByName(name);
        }
    }
    return 0;
}