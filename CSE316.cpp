#include <iostream>
#include <mutex>
#include <chrono>
#include <vector>
#include <random>
#include <thread>

using namespace std;

const int num_resources = 3;
int num_processes;

vector<int> available(num_resources);
vector<vector<int>> maximum;
vector<vector<int>> need;
vector<vector<int>> allocation;

vector<thread> threads;
vector<mutex> mutex_locks;

void display_state() {
    cout << "Available resources: ";
    for (int i = 0; i < num_resources; ++i) {
        cout << available[i] << " ";
    }
    cout << endl;
    cout << "Allocation matrix:\n";
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_resources; ++j) {
            cout << allocation[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Maximum matrix:\n";
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_resources; ++j) {
            cout << maximum[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Need matrix:\n";
    for (int i = 0; i < num_processes; ++i) {
        for (int j = 0; j < num_resources; ++j) {
            cout << need[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

bool is_safe() {
    vector<int> work(available);
    vector<bool> finish(num_processes, false);

    bool found;
    do {
        found = false;
        for (int i = 0; i < num_processes; ++i) {
            if (!finish[i]) {
                bool enough_resources = true;
                for (int j = 0; j < num_resources; ++j) {
                    if (need[i][j] > work[j]) {
                        enough_resources = false;
                        break;
                    }
                }
                if (enough_resources) {
                    finish[i] = true;
                    for (int j = 0; j < num_resources; ++j) {
                        work[j] += allocation[i][j];
                    }
                    found = true;
                }
            }
        }
    } while (found);

    return all_of(finish.begin(), finish.end(), [](bool f) { return f; });
}

void request_resources(int process_num) {
    while (true) {
        vector<int> request(num_resources);
        for (int i = 0; i < num_resources; ++i) {
            request[i] = rand() % (need[process_num][i] + 1);
        }

        bool safe;
        {
            lock_guard<mutex> guard(mutex_locks[process_num]);

            if (all_of(request.begin(), request.end(), [](int r) { return r == 0; })) {
                continue;
            }

            if (all_of(request.begin(), request.end(), [](int r) { return r <= available[r]; })) {
                for (int i = 0; i < num_resources; ++i) {
                    available[i] -= request[i];
                    allocation[process_num][i] += request[i];
                    need[process_num][i] -= request[i];
                }
                safe = is_safe();
                if (!safe) {
                    for
