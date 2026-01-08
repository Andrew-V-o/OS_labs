#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

void start_process(const char* app_name, int input_fd, int output_fd) {
    pid_t pid = fork();

    if (pid == 0) {
        dup2(input_fd, STDIN_FILENO);
        dup2(output_fd, STDOUT_FILENO);

        close(input_fd);
        close(output_fd);

        string path = "./" + string(app_name);
        char* args[] = {(char*)path.c_str(), NULL};
        execv(args[0], args);

        perror("execv failed");
        exit(1);
    }
}

int main() {
    const char* apps[] = {"taskA", "taskM", "taskP", "taskS"};
    int pipes[5][2];

    for (int i = 0; i < 5; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed");
            return 1;
        }
    }

    for (int i = 0; i < 4; i++) {
        start_process(apps[i], pipes[i][0], pipes[i+1][1]);
        
        close(pipes[i][0]);
        close(pipes[i+1][1]);
    }

    cout << "Enter numbers separated by spaces: ";
    string input;
    getline(cin, input);
    input += "\n";

    write(pipes[0][1], input.c_str(), input.length());
    close(pipes[0][1]);

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    ssize_t bytesRead = read(pipes[4][0], buf, sizeof(buf) - 1);
    if (bytesRead > 0) {
        cout << "Result: " << buf;
    }

    close(pipes[4][0]);

    for (int i = 0; i < 4; i++) wait(NULL);

    return 0;
}