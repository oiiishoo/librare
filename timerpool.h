#pragma once

//мультимедийный таймер не предпологается к работе res < 1MilliSec
#include <iostream>
#include <Windows.h>
#include <mmsystem.h>
#include <vector>
#include <mutex>
#include <list>
#include <thread>
#pragma comment(lib, "winmm.lib")
struct timestruc {

    unsigned int micros,millis,seconds;
    timestruc(): micros(0),millis(0),seconds(0){}
    timestruc(unsigned int micros, unsigned int millis=0, unsigned int seconds=0) :micros(micros), millis(millis), seconds(seconds) {}
    unsigned int total_microseconds() {
        return micros + millis * 1000 + seconds * 1'000'000;
    }
};
class Timerpool {
    UINT timerId;
    size_t ticks;
    std::mutex mtx;
    HANDLE handle_t;
    volatile bool interruption, dead;
    unsigned int resolution, stackFactor;
    LARGE_INTEGER freq_qpc;

    struct UTASK {
        DWORD(WINAPI* func)(LPVOID);
        double last_call_time;
        bool branching;
        LPVOID userarg;
    };

    struct UPAIR {
        size_t interval_us;
        std::list<UTASK> array;

    };
    std::list< UPAIR > intervalpool;

public:

    Timerpool(timestruc& ts, unsigned int delegationstackFactor4096 = 16) : Timerpool(ts.total_microseconds(), delegationstackFactor4096) {
    }
    Timerpool(unsigned int resolution_us, unsigned int delegationstackFactor4096 = 16) {
    ticks = 0;
    handle_t = 0;
    timerId = 0;
    interruption = 0;
    dead = 0;
    stackFactor = delegationstackFactor4096;

   
    if (resolution_us > 1'000'000) {
        resolution_us = 1'000'000; 
    }
    this->resolution = resolution_us;
    QueryPerformanceFrequency(&freq_qpc);

    if (resolution_us >= 1000) {
        // мультимедиа таймер для больших периодов
        timeBeginPeriod(1);
        timerId = timeSetEvent(
            static_cast<UINT>(resolution_us / 1000),
            1,
            TimerCallback,
            reinterpret_cast<DWORD_PTR>(this),
            TIME_PERIODIC
        );

        if (!timerId)
            std::cerr << "[Timerpool] timeSetEvent creation failed!\n";
    }
    else {
        // busy-loop для микросекундных интервалов (или нулевого разрешения)
        handle_t = CreateThread(
            0,
            4096 * stackFactor,
            BusyLoop,
            this,
            0,
            0
        );

        if (!handle_t)
            std::cerr << "[Timerpool] busywait thread creation failed!\n";
    }
}


    ~Timerpool() {
        interruption = 0;
        dead = 1;
        if (handle_t) {
            WaitForSingleObject(handle_t, INFINITE);
            CloseHandle(handle_t);
        }
        if (timerId)
            timeKillEvent(timerId);
        timeEndPeriod(1);
    }

    Timerpool& add_start(timestruc ts, DWORD(WINAPI* func)(LPVOID), LPVOID argumentPTR = 0, bool isbranching = 0) {
        return add_start(ts.total_microseconds(), func, argumentPTR, isbranching);
    }


    Timerpool& add_start(size_t microseconds, DWORD(WINAPI* func)(LPVOID), LPVOID argumentPTR = 0, bool isbranching = 0) {
        std::lock_guard<std::mutex> lock(mtx);

        // ищем существующий интервал
        for (auto& it : intervalpool) {
            if (it.interval_us == microseconds) {
                UTASK ut;
                ut.branching = isbranching;
                ut.func = func;
                ut.last_call_time = 0.0;
                ut.userarg = argumentPTR;

                it.array.push_front(ut);
                return *this;
            }
        }

        UPAIR newpair;
        newpair.interval_us = microseconds;

        UTASK ut;
        ut.branching = isbranching;
        ut.func = func;
        ut.last_call_time = 0.0;
        ut.userarg = argumentPTR;

        newpair.array.push_front(ut);
        intervalpool.push_front(newpair);

        return *this;
    }

    Timerpool& add_end(timestruc ts, DWORD(WINAPI* func)(LPVOID), LPVOID argumentPTR = 0, bool isbranching = 0) {
        return add_end(ts.total_microseconds(), func, argumentPTR, isbranching);
    }

    Timerpool& add_end(size_t microseconds, DWORD(WINAPI* func)(LPVOID), LPVOID argumentPTR = nullptr, bool isbranching = false) {
        std::lock_guard<std::mutex> lock(mtx);

        for (auto& it : intervalpool) {
            if (it.interval_us == microseconds) {
                UTASK ut;
                ut.branching = isbranching;
                ut.func = func;
                ut.last_call_time = 0.0;
                ut.userarg = argumentPTR;

                it.array.push_back(ut);
                return *this;
            }
        }

        UPAIR newpair;
        newpair.interval_us = microseconds;

        UTASK ut;
        ut.branching = isbranching;
        ut.func = func;
        ut.last_call_time = 0.0;
        ut.userarg = argumentPTR;

        newpair.array.push_back(ut);
        intervalpool.push_back(newpair);

        return *this;
    }




    Timerpool& remove(timestruc ts, DWORD(WINAPI* func)(LPVOID)) {
        return remove(ts.total_microseconds(), func);
    }
    Timerpool& remove(size_t microseconds, DWORD(WINAPI* func)(LPVOID)) {
        std::lock_guard<std::mutex> lock(mtx); 

        for (auto it = intervalpool.begin(); it != intervalpool.end(); ++it) {
            if (it->interval_us == microseconds) {
                for (auto fit = it->array.begin(); fit != it->array.end(); ++fit) {
                    if (fit->func == func) {
                        it->array.erase(fit);
                        break;
                    }
                }

                if (it->array.empty())
                    intervalpool.erase(it);

                break;
            }
        }
        return *this;
    }

    Timerpool& skipTicks(size_t nticks) {
        //std::lock_guard<std::mutex> lock(mtx);
        interruption = 1;
        ticks += nticks;
        interruption = 0;
        return *this;
    }

    Timerpool& XskipTicksSync(size_t nticks) {
        //std::lock_guard<std::mutex> lock(mtx);
        interruption = 1;
        for (size_t i = 0; i < nticks; i++) {
            ++ticks;
            for (auto& i_pair : intervalpool) {
                if ((!i_pair.interval_us) || !(ticks % i_pair.interval_us)) {
                    for (auto& ut : i_pair.array)
                        if (ut.func) ut.func(0);
                }
            }
        }
        interruption = 0;
        return *this;
    }


    Timerpool& XskipTicksAsync(size_t nticks) {

        XskipParams* params = new XskipParams{ this, nticks };

        HANDLE handle_temp = CreateThread(
            0,
            0,
            XskipTicksThread,
            params,
            0,
            0
        );

        if (!handle_temp) {
            std::cerr << "[Timerpool] XskipTicksAsync thread creation failed!\n";
            delete params;
        }
        else {
            CloseHandle(handle_temp);
        }

        return *this;
    }

    HANDLE getThread() {
        return handle_t;
    }
private:
    
    static void CALLBACK TimerCallback(UINT uTimerID,
        UINT uMsg,
        DWORD_PTR dwUser,
        DWORD_PTR dw1,
        DWORD_PTR dw2) {

        reinterpret_cast<Timerpool*>(dwUser)->timedtick();

    }
    void timedtick(LARGE_INTEGER nowcache={}) {
        if (nowcache.QuadPart == 0)
            QueryPerformanceCounter(&nowcache);
        //std::lock_guard<std::mutex> lock(mtx); // потокобезопасный доступ

        for (auto& interval : intervalpool) {
            for (auto& task : interval.array) {
                if (task.last_call_time == 0.0)
                    task.last_call_time = static_cast<double>(nowcache.QuadPart);

                double ticks_per_interval = static_cast<double>(interval.interval_us) * freq_qpc.QuadPart / 1'000'000.0;

                double delta_ticks = static_cast<double>(nowcache.QuadPart) - task.last_call_time;

                if (delta_ticks >= ticks_per_interval) {
                    task.last_call_time += ticks_per_interval;

                    if (task.branching) {
                        HANDLE handle_temp = CreateThread(
                            0,
                            4096 * stackFactor,
                            task.func,
                            task.userarg,
                            0,
                            0
                        );

                        if (!handle_temp)
                            std::cerr << "[Timerpool] task delegation thread creation failed!\n";
                        else
                            CloseHandle(handle_temp);
                    }
                    else {
                        task.func(task.userarg);
                    }
                }
            }
        }
    }
  

    static DWORD WINAPI BusyLoop(LPVOID param) {
        Timerpool* self = reinterpret_cast<Timerpool*>(param);
        if (!self) return 0;
        LARGE_INTEGER nowcache;

        LARGE_INTEGER freq = self->freq_qpc; // кэш для ускорения
        const double tick_per_us = (double)freq.QuadPart / 1'000'000.0;
        const double period_ticks = (self->resolution ? self->resolution : 0) * tick_per_us;

        QueryPerformanceCounter(&nowcache);
        double next_tick = (double)nowcache.QuadPart + period_ticks;
        if (!period_ticks) {
            while (!self->dead) {

                self->timedtick();  
                //_mm_pause(); // лёгкий CPU релакс

                
            }
        }
        else {
            while (!self->dead) {
                QueryPerformanceCounter(&nowcache);
                double now_ticks = (double)nowcache.QuadPart;

                if (now_ticks >= next_tick) {
                    self->timedtick();  // ✅ без аргумента, сам считает время
                    next_tick += period_ticks;
                }

            }
        }

        return 0;
    }


    
    static DWORD WINAPI XskipTicksThread(LPVOID param) {
        XskipParams* p = reinterpret_cast<XskipParams*>(param);
        if (p && p->timer) {
            p->timer->XskipTicksSync(p->nticks);
        }
        delete p;
        return 0;
    }
    struct XskipParams {
        Timerpool* timer;
        size_t nticks;
    };
};
