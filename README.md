# My C++ code snippets

- [Scheme interpreter implementation](https://github.com/fdr896/cpp_libs/tree/master/scheme)

- Basic Math classes
  - [Complex numbers](https://github.com/fdr896/cpp_libs/blob/master/math_classes/complex.h)
  - [Polynomials](https://github.com/fdr896/cpp_libs/blob/master/math_classes/polynomial.h)
  - [Rational numbers](https://github.com/fdr896/cpp_libs/blob/master/math_classes/rational.h)
  - [Date](https://github.com/fdr896/cpp_libs/blob/master/math_classes/date.h)
  - [Matrix](https://github.com/fdr896/cpp_libs/blob/master/math_classes/matrix.h) (with constexpr multiplication sequence optimization)

- Basic thread concepts
    - [Concurrent hash-map](https://github.com/fdr896/cpp_libs/blob/master/thread_basics/concurrent_hash_map.h)
    - [Semaphore](https://github.com/fdr896/cpp_libs/blob/master/thread_basics/semaphore.h)
    - [Buffered channel](https://github.com/fdr896/cpp_libs/blob/master/thread_basics/buffered_channel.h)
    - [Unbuffered channel](https://github.com/fdr896/cpp_libs/blob/master/thread_basics/unbuffered_channel.h)

- Implementation of several basic data structures
    - [Deque](https://github.com/fdr896/cpp_libs/blob/master/standart_classes/deque.h)
    - [LRU-Cache](https://github.com/fdr896/cpp_libs/blob/master/standart_classes/lru_cache.h)
    - [Intrusive-list](https://github.com/fdr896/cpp_libs/blob/master/standart_classes/intrusive_list.h)

- C++ smart pointers implementation
    - [Unique pointer](https://github.com/fdr896/cpp_libs/blob/master/smart_ptrs/unique.h)
    - [Shared pointer](https://github.com/fdr896/cpp_libs/blob/master/smart_ptrs/shared.h)
    - [Weak pointer](https://github.com/fdr896/cpp_libs/blob/master/smart_ptrs/weak.h)
    - [Shared from this](https://github.com/fdr896/cpp_libs/blob/master/smart_ptrs/sw_fwd.h)

- Lock-free data structres implementation
    - [Mutex](https://github.com/fdr896/cpp_libs/blob/master/lock_free/mutex.h) (based on futex)
    - [Read-Write spinlock](https://github.com/fdr896/cpp_libs/blob/master/lock_free/rw_spinlock.h)
    - [MPMC-Queue](https://github.com/fdr896/cpp_libs/blob/master/lock_free/mpmc_queue.h) (multi-producer multi-consumer queue)
    - [MPSC-Stack](https://github.com/fdr896/cpp_libs/blob/master/lock_free/mpsc_stack.h) (multi-producer single-consumer stack)

- Meta programming implementation on C++
    - [BindFront](https://github.com/fdr896/cpp_libs/blob/master/meta/bind_front.h) (`std::bind_front` analogue)
    - [Constexpr map](https://github.com/fdr896/cpp_libs/blob/master/meta/constexpr_map.h)
    - [Curry](https://github.com/fdr896/cpp_libs/blob/master/meta/curry.h)
    - [TransformTuple](https://github.com/fdr896/cpp_libs/blob/master/meta/transform_tuple.h) (`std::transform` equivalent for `std::tuple`)

- Some design patterns implementation
    - [Any](https://github.com/fdr896/cpp_libs/blob/master/patterns/any.h) (equivalent to `std::any`)
    - [Immutable Vector from Scala](https://github.com/fdr896/cpp_libs/blob/master/patterns/immutable_vector.h)
    - [Test framework](https://github.com/fdr896/cpp_libs/blob/master/patterns/test_framework.h) (example of abstract farm pattern)

- Coroutines
    - [Coroutine](https://github.com/fdr896/cpp_libs/blob/master/coroutines/coroutine.h) (coroutine implementation based on `boost/continuation`)
    - [Yield and Generator](https://github.com/fdr896/cpp_libs/blob/master/coroutines/generator.h) (implementation of `co_yield` and `generator<T>`)