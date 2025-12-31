#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <random>
#include <pthread.h>

using namespace std;
using Matrix = vector<vector<int>>;

int matrixSize;
pthread_mutex_t taskMutex = PTHREAD_MUTEX_INITIALIZER;
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

void* ThreadMultiplyWorker(void* arg) {
    SharedData *data = static_cast<SharedData*>(arg);
    int n = matrixSize;
    int k = data->blockSize;

    while (true) {
        int blockIdx;

        pthread_mutex_lock(&taskMutex);
        if (currentBlockIndex >= data->totalBlocks) {
            pthread_mutex_unlock(&taskMutex);
            break; 
        }
        blockIdx = currentBlockIndex++; 
        pthread_mutex_unlock(&taskMutex);

        int row = blockIdx / data->numBlocksPerRow;
        int col = blockIdx % data->numBlocksPerRow;

        int iStart = row * k;
        int iEnd = min(iStart + k, n);
        int jStart = col * k;
        int jEnd = min(jStart + k, n);

        for (int i = iStart; i < iEnd; ++i) {
            for (int j = jStart; j < jEnd; ++j) {
                int tempSum = 0;
                for (int kIndex = 0; kIndex < n; ++kIndex) {
                    tempSum += (*(data->inputA))[i][kIndex] * (*(data->inputB))[kIndex][j];
                }
                (*(data->resultMatrix))[i][j] = tempSum;
            }
        }
    }
    return NULL;
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

    cout << "Введите размер матрицы N: ";
    cin >> matrixSize;

    Matrix matrixA = createMatrix(matrixSize);
    Matrix matrixB = createMatrix(matrixSize);

    Matrix classicResult = timeClassicMultiplication(matrixA, matrixB);

    int k;
    cout << "Введите размер блока k: ";
    cin >> k;

    int numBlocksPerRow = (int)ceil((double)matrixSize / k);
    int totalBlocks = numBlocksPerRow * numBlocksPerRow;

    int numThreads = totalBlocks;
    if (numThreads > 800) numThreads = 800;

    Matrix threadResult(matrixSize, vector<int>(matrixSize, 0));
    SharedData data = {&matrixA, &matrixB, &threadResult, k, numBlocksPerRow, totalBlocks};
    currentBlockIndex = 0;

    vector<pthread_t> threads(numThreads);
    
    auto start = chrono::high_resolution_clock::now();

    for (int i = 0; i < numThreads; ++i) {
        pthread_create(&threads[i], NULL, ThreadMultiplyWorker, &data);
    }

    for (int i = 0; i < numThreads; ++i) {
        pthread_join(threads[i], NULL);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Размер блока k: " << k << endl;
    cout << "Всего блоков: " << totalBlocks << endl;
    cout << "Число запущенных потоков: " << numThreads << endl;
    cout << "Многопоточное время: " << fixed << setprecision(6) << elapsed.count() << " сек" << endl;

    if (compareMatrices(classicResult, threadResult)) {
        cout << "Матрицы совпадают!" << endl;
    } else {
        cout << "Ошибка: Матрицы НЕ совпадают!" << endl;
    }

    pthread_mutex_destroy(&taskMutex);
    return 0;
}