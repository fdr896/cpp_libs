#pragma once

#include <map>
#include <exception>
#include <functional>
#include <vector>
#include <string>
#include <type_traits>
#include <memory>

class AbstractTest {
public:
    virtual void SetUp(){};
    virtual void TearDown(){};
    virtual void Run(){};
    virtual ~AbstractTest() {
    }
};

class TestHolder {
    using Pointer = std::unique_ptr<AbstractTest>;

public:
    template <typename F>
    TestHolder(F&& ctor) : defer_ctor_(std::move(ctor)) {
    }

    void Initialize() {
        test_ = defer_ctor_();
    }

    std::unique_ptr<AbstractTest> Get() {
        return std::move(test_);
    }

private:
    Pointer test_{};
    std::function<Pointer()> defer_ctor_;
};

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        tests_.emplace(class_name, TestHolder([]() {
                           std::unique_ptr<AbstractTest> ptr = std::make_unique<TestClass>();
                           return std::move(ptr);
                      }));
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        if (auto test = tests_.find(class_name); test != tests_.end()) {
            test->second.Initialize();
            return test->second.Get();
        }

        throw std::out_of_range("no such test registered");
    }

    void RunTest(const std::string& test_name) {
        std::unique_ptr<AbstractTest> test;
        try {
            test = CreateTest(test_name);
        } catch (...) {
            throw;
        }

        test->SetUp();
        try {
            test->Run();
        } catch (...) {
            test->TearDown();
            throw;
        }
        test->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> show;
        for (const auto& [test_name, _] : tests_) {
            if (callback(test_name)) {
                show.emplace_back(test_name);
            }
        }

        return show;
    }

    std::vector<std::string> ShowAllTests() const {
        return ShowTests([](const std::string&) { return true; });
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (const auto& [test_name, _] : tests_) {
            if (callback(test_name)) {
                try {
                    RunTest(test_name);
                } catch (...) {
                    throw;
                }
            }
        }
    }

    void Clear() {
        tests_.clear();
    }

    static TestRegistry& Instance() {
        static TestRegistry tr{};

        return tr;
    }

private:
    std::map<std::string, TestHolder> tests_;
};

class FullMatch {
public:
    FullMatch(const std::string& actual) : actual_(actual) {
    }
    FullMatch(std::string&& actual) : actual_(std::move(actual)) {
    }

    bool operator()(const std::string& check) {
        return actual_ == check;
    }

private:
    std::string actual_;
};

class Substr {
public:
    Substr(const std::string& pattern) : pattern_(pattern) {
    }
    Substr(std::string&& pattern) : pattern_(std::move(pattern)) {
    }

    bool operator()(const std::string& other) {
        return other.find(pattern_) != std::string::npos;
    }

private:
    std::string pattern_;
};
