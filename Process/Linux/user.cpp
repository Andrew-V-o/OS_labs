#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <cstdlib>

using namespace std;

void runKiller(const char* flag = nullptr, const char* value = nullptr) {
    pid_t pid = fork();
    if (pid == 0) {
        if (!flag) execl("./Killer", "Killer", nullptr);
        else execl("./Killer", "Killer", flag, value, nullptr);
        _exit(1);
    } else if (pid > 0) {
        waitpid(pid, nullptr, 0);
    }
}

int main() {
    setenv("PROC_TO_KILL", "gedit,vim", 1);

    runKiller("--name", "sleep"); 
    runKiller("--id", "1234");
    runKiller();

    unsetenv("PROC_TO_KILL");

    return 0;
}