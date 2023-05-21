#include <iostream>
#include <vector>
#include <thread>
#include <random>
#include <chrono>
#include <mutex>

using namespace std;
mutex mtx;

void multiplyMatrix(const vector<int>& A, const vector<int>& B, vector<int>& C, int n, int m, int k, int numThreads) {
    vector<thread> threads(numThreads);

    auto threadFunc = [&](int threadIndex) {
        int chunkSize = n * k / numThreads;
        int startIndex = threadIndex * chunkSize;
        int endIndex = (threadIndex == numThreads - 1) ? n * k : startIndex + chunkSize;

        for (int index = startIndex; index < endIndex; ++index) {
            int row = index / k;
            int col = index % k;
            int result = 0;

            for (int i = 0; i < m; ++i) {
                result += A[row * m + i] * B[i * k + col];
            }

            C[index] = result;
            mtx.lock();
            // Виведення елементу після обчислення
            cout << "[" << row << ", " << col << "] = " << result << flush << endl;
            mtx.unlock();
        }
    };

    for (int i = 0; i < numThreads; ++i) {
        threads[i] = thread(threadFunc, i);
    }

    for (int i = 0; i < numThreads; ++i) {
        threads[i].join();
    }
}

int main() {
    int n, m, k;
    cout << "Enter the number of rows for matrix A: ";
    cin >> n;

    cout << "Enter the number of columns for matrix A and rows for matrix B: ";
    cin >> m;

    cout << "Enter the number of columns for matrix B: ";
    cin >> k;

    vector<int> A(n * m); // Матриця A
    vector<int> B(m * k); // Матриця B
    vector<int> C(n * k); // Матриця результату

    // Заповнення матриці A рандомними числами від 1 до 9
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dist(1, 100);

    for (int i = 0; i < n * m; i++) {
        A[i] = dist(gen);
    }

    // Заповнення матриці B рандомними числами від 1 до 9
    for (int i = 0; i < m * k; i++) {
        B[i] = dist(gen);
    }

    cout << "Matrix A:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            cout << A[i * m + j] << " ";
        }
        cout << endl;
    }

    cout << "Matrix B:" << endl;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < k; j++) {
            cout << B[i * k + j] << " ";
        }
        cout << endl;
    }

    vector<int> numThreadsVector = { 1, 2, 4, 8 };

    for (int numThreads : numThreadsVector) {
        auto startTime = chrono::steady_clock::now();

        multiplyMatrix(A, B, C, n, m, k, numThreads);

        auto endTime = chrono::steady_clock::now();
        auto elapsedTime = chrono::duration_cast<chrono::duration<double>>(endTime - startTime).count();

        cout << "Number of Threads: " << numThreads << ", Average Time: " << elapsedTime << " seconds" << endl<<endl;
    }

    cout << "Multiplication result:" << endl;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            cout << C[i * k + j] << " ";
        }
        cout << endl;
    }

    return 0;
}

//За результатами можна зробити наступні висновки стосовно залежності часу від кількості потоків :
//
//При розмірі матриць 2x3 та 3x2 використання 2 потоків є ефективним, оскільки воно дозволяє розділити роботу між двома потоками 
//і досягти парадигми паралелізму. Збільшення кількості потоків після досягнення певної точки може призвести до додаткових 
//накладних витрат на управління потоками, що збільшує загальний час виконання.Отже, в даному випадку найкращим варіантом є 
//використання 2 потоків, оскільки це дозволяє досягти балансу між паралельним виконанням обчислень та ефективним використанням ресурсів системи.
//Збільшення кількості потоків зменшує середній час виконання до певного моменту.
//В даному випадку найкращий результат показує використання 2 потоків, де час виконання найменший.
//Зауважимо, що при збільшенні кількості потоків до 16 та 32, час виконання збільшується порівняно з використанням 2 потоків.

