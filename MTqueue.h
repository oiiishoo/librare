#pragma once
#include <queue>
#include <mutex>
#include <list>
#include <vector>
#include <type_traits>

template <typename T>
class MTQueue {
private:
    std::queue<T> queue;
    std::mutex mtx;

public:
    // ------------------------
    // Одиночные элементы (типа T)
    // ------------------------
    void push(T item) {
        std::lock_guard<std::mutex> lock(mtx);
        queue.push(std::move(item));
    }

    bool pop(T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        item = std::move(queue.front());
        queue.pop();
        return true;
    }

    bool peek(T& item) {
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;
        item = queue.front();
        return true;
    }

    size_t size() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.size();
    }

    bool isempty() {
        std::lock_guard<std::mutex> lock(mtx);
        return queue.empty();
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mtx);
        while (!queue.empty()) queue.pop();
    }



    MTQueue<T>& operator<(T item) {  // ввод
        std::lock_guard<std::mutex> lock(mtx);
        push(std::move(item));
        return *this;
    }

    bool operator>(T& item) {  // вывод
        std::lock_guard<std::mutex> lock(mtx);
        return pop(item);
    }


    // ------------------------
    // Операторы << и >> для контейнера
    // ------------------------
    template <typename Container>
    MTQueue<T>& operator<<(const Container& container) {
        static_assert(std::is_same_v<typename Container::value_type, T>,
            "Container value_type must match T");
        std::lock_guard<std::mutex> lock(mtx);
        for (const auto& item : container)
            queue.push(item);
        return *this;
    }

    // ============================================================
    // Batch извлечение через >>
    // ============================================================
    template <typename Container>
    bool operator>>(Container& dest) {
        static_assert(std::is_same_v<typename Container::value_type, T>,
            "Container value_type must match T");
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return false;

        size_t limit = dest.capacity() ? dest.capacity() : queue.size();
        size_t count = std::min(limit, queue.size());
        for (size_t i = 0; i < count; ++i) {
            dest.push_back(std::move(queue.front()));
            queue.pop();
        }
        return count > 0;
    }

    // ============================================================
    // Альтернативный batch push с лимитом
    // ============================================================
    template <typename Container>
    void pushBatch(const Container& container, size_t limit = SIZE_MAX) {
        static_assert(std::is_same_v<typename Container::value_type, T>,
            "Container value_type must match T");
        std::lock_guard<std::mutex> lock(mtx);
        size_t count = std::min(limit, container.size());
        auto it = container.begin();
        for (size_t i = 0; i < count; ++i, ++it)
            queue.push(*it);
    }

    // ============================================================
    // Альтернативный batch pop с лимитом
    // ============================================================
    template <typename Container>
    size_t popBatch(Container& dest, size_t limit = SIZE_MAX) {
        static_assert(std::is_same_v<typename Container::value_type, T>,
            "Container value_type must match T");
        std::lock_guard<std::mutex> lock(mtx);
        if (queue.empty()) return 0;

        size_t count = std::min(limit, queue.size());
        for (size_t i = 0; i < count; ++i) {
            dest.push_back(std::move(queue.front()));
            queue.pop();
        }
        return count;
    }
};










//#include <iostream>
//#include <thread>
//#include <vector>
//#include <chrono>
//#include "MTqueue.h"
//
//MTQueue<int> tsQueue;
//
//const int TEST_DURATION_SEC = 5; // тестируем 5 секунд
//
//constexpr int BATCH_SIZE = 10;    // размер пачки
//
//void writer(int id, bool& running, long long& ops) {
//    std::vector<int> batch;
//    batch.reserve(BATCH_SIZE);
//
//    int value = id * 10000000; // уникальное начало для каждого потока
//    while (running) {
//        batch.clear();
//        for (int i = 0; i < BATCH_SIZE; ++i)
//            batch.push_back(value++);
//
//        // один lock на всю пачку
//        tsQueue.pushBatch(batch, BATCH_SIZE);
//        ops += batch.size();
//    }
//}
//
//void reader(bool& running, long long& ops) {
//    std::vector<int> batch;
//    batch.reserve(BATCH_SIZE);
//
//    while (running) {
//        batch.clear();
//        size_t got = tsQueue.popBatch(batch, BATCH_SIZE);
//        ops += got;
//    }
//}
//
//int main() {
//    bool running = true;
//    long long
//        writerOps1 = 0, writerOps2 = 0, writerOps3 = 0,
//        readerOps = 0, readerOps2 = 0, readerOps3 = 0;
//
//    std::thread t1(writer, 1, std::ref(running), std::ref(writerOps1));
//    std::thread t2(writer, 2, std::ref(running), std::ref(writerOps2));
//    std::thread t3(writer, 3, std::ref(running), std::ref(writerOps3));
//    std::thread tReader(reader, std::ref(running), std::ref(readerOps));
//    std::thread t2Reader(reader, std::ref(running), std::ref(readerOps2));
//    std::thread t3Reader(reader, std::ref(running), std::ref(readerOps3));
//    // Ждём фиксированное время теста
//    std::this_thread::sleep_for(std::chrono::seconds(TEST_DURATION_SEC));
//    running = false;
//
//    t1.join();
//    t2.join();
//    t3.join();
//    tReader.join();
//    t2Reader.join();
//    t3Reader.join();
//    long long totalWriterOps = writerOps1 + writerOps2 + writerOps3;
//    long long totalreaderOps = readerOps + readerOps2 + readerOps3;
//    std::cout << "Test duration: " << TEST_DURATION_SEC << " seconds\n";
//    std::cout << "Writer ops: " << totalWriterOps << "\n";
//    std::cout << "Reader ops: " << totalreaderOps << "\n";
//    std::cout << "Writer throughput: " << totalWriterOps / TEST_DURATION_SEC << " ops/sec\n";
//    std::cout << "Reader throughput: " << totalreaderOps / TEST_DURATION_SEC << " ops/sec\n";
//
//    return 0;
//}
//


