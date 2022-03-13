#pragma once

#include <linux/futex.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <atomic>

// Atomically do the following:
//    if (*value == expected_value) {
//        sleep_on_address(value)
//    }
void FutexWait(int *value, int expected_value) {
    syscall(SYS_futex, value, FUTEX_WAIT_PRIVATE, expected_value, nullptr, nullptr, 0);
}

// Wakeup 'count' threads sleeping on address of value(-1 wakes all)
void FutexWake(int *value, int count) {
    syscall(SYS_futex, value, FUTEX_WAKE_PRIVATE, count, nullptr, nullptr, 0);
}

class Mutex {
public:
    void Lock() {
        int c = 0;
        if (!__atomic_compare_exchange_n(&value_, &c, 1, true, __ATOMIC_SEQ_CST,
                                         __ATOMIC_SEQ_CST)) {
            if (c != 2) {
                c = __atomic_exchange_n(&value_, 2, __ATOMIC_SEQ_CST);
            }
            while (c != 0) {
                FutexWait(&value_, 2);
                c = __atomic_exchange_n(&value_, 2, __ATOMIC_SEQ_ST);
            }
        }
    }

    void Unlock() {
        if (__atomic_fetch_sub(&value_, 1, __ATOMIC_SEQ_CST) != 1) {
            __atomic_store_n(&value_, 0, __ATOMIC_SEQ_CST);
            FutexWake(&value_, 1);
        }
    }

private:
    int value_{};
};
