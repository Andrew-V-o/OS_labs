#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <thread>
#include <random>
#include <mutex>

using namespace std;
using Matrix = vector<vector<int>>;

int matrixSize;
mutex taskMutex;
int currentBlockIndex = 0;

struct SharedData {
    const Matrix *inputA;
    const Matrix *inputB;
    Matrix *resultMatrix;
    int blockSize;
    int numBlocksPerRow;
    int totalBlocks;
};

Matrix createMatrix(int size) {
    Matrix resultMatrix(size, vector<int>(size));
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distrib(0, 9);

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            resultMatrix[i][j] = distrib(gen);
        }
    }
    return resultMatrix;
}

Matrix timeClassicMultiplication(const Matrix &A, const Matrix &B) {
    Matrix C(matrixSize, vector<int>(matrixSize, 0));
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            for (int k = 0; k < matrixSize; ++k) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Классическое умножение: " << fixed << setprecision(6) << elapsed.count() << " сек" << endl;
    return C;
}


void threadWorker(SharedData &data) {
    int n = matrixSize;
    int k = data.blockSize;

    while (true) {
        int blockIdx;
        {
            lock_guard<mutex> lock(taskMutex);
            if (currentBlockIndex >= data.totalBlocks) break;
            blockIdx = currentBlockIndex++;
        }

        int row = blockIdx / data.numBlocksPerRow;
        int col = blockIdx % data.numBlocksPerRow;
        int iStart = row * k;
        int iEnd = min(iStart + k, n);
        int jStart = col * k;
        int jEnd = min(jStart + k, n);

        for (int i = iStart; i < iEnd; ++i) {
            for (int j = jStart; j < jEnd; ++j) {
                int tempSum = 0;
                for (int m = 0; m < n; ++m) {
                    tempSum += (*(data.inputA))[i][m] * (*(data.inputB))[m][j];
                }
                (*(data.resultMatrix))[i][j] = tempSum;
            }
        }
    }
}

bool compareMatrices(const Matrix &A, const Matrix &B) {
    for (int i = 0; i < matrixSize; ++i) {
        for (int j = 0; j < matrixSize; ++j) {
            if (A[i][j] != B[i][j]) return false;
        }
    }
    return true;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    cout << "Введите размер матрицы N: "; cin >> matrixSize;

    Matrix matrixA = createMatrix(matrixSize);
    Matrix matrixB = createMatrix(matrixSize);
    Matrix classicResult = timeClassicMultiplication(matrixA, matrixB);

    int k;
    cout << "Введите размер блока k: "; cin >> k;

    int numBlocksPerRow = (int)ceil((double)matrixSize / k);
    int totalBlocks = numBlocksPerRow * numBlocksPerRow;
    int numThreads = (totalBlocks > 800) ? 800 : totalBlocks;

    Matrix threadResult(matrixSize, vector<int>(matrixSize, 0));
    SharedData data = {&matrixA, &matrixB, &threadResult, k, numBlocksPerRow, totalBlocks};
    currentBlockIndex = 0;

    vector<thread> threads;
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(threadWorker, ref(data));
    }

    for (auto &t : threads) t.join();

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Размер блока k: " << k << "\nВсего блоков: " << totalBlocks 
         << "\nЧисло запущенных потоков: " << numThreads 
         << "\nМногопоточное время: " << fixed << setprecision(6) << elapsed.count() << " сек" << endl;

    if (compareMatrices(classicResult, threadResult)) cout << "Матрицы совпадают!" << endl;
    return 0;
}