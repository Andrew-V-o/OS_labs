#include <iostream>
#include <string>
#include <sstream>

using namespace std;

int main() {
    string line;
    if (getline(cin, line)) {
        stringstream ss(line);
        int x;
        while (ss >> x) cout << (long long)x * x * x << " ";
        cout << endl;
    }
    return 0;
}