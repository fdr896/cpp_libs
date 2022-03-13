#pragma once

#include <algorithm>
#include <map>

class ListHook {
public:
    ListHook() = default;

    bool IsLinked() const {
        return (left_ != nullptr) || (right_ != nullptr);
    }

    void Unlink() {
        ListHook* old_left = left_;
        ListHook* old_right = right_;
        if (old_left != nullptr) {
            old_left->right_ = old_right;
        }
        if (old_right != nullptr) {
            old_right->left_ = old_left;
        }
        left_ = right_ = nullptr;
    }

    // Must unlink element from list
    virtual ~ListHook() {
        Unlink();
    }

    ListHook(const ListHook&) = delete;
    ListHook(ListHook&& other) {
        left_ = other.left_;
        right_ = other.right_;
        other.left_ = other.right_ = nullptr;
    }

    ListHook& operator=(const ListHook&) = delete;
    ListHook& operator=(ListHook&& other) {
        left_ = other.left_;
        right_ = other.right_;
        other.left_ = other.right_ = nullptr;

        return *this;
    }

    bool operator==(const ListHook& other) {
        return (left_ == other.left_) && (right_ == other.right_);
    }

private:
    template <class T>
    friend class List;

    // that helper function might be useful
    void LinkBefore(ListHook* other) {  // a->b => a->other->b
        if (left_ == nullptr) {
            left_ = other;
            other->right_ = this;
            return;
        }
        left_->right_ = other;
        other->left_ = left_;
        left_ = other;
        other->right_ = this;
    }

    ListHook* left_ = nullptr;
    ListHook* right_ = nullptr;
};

template <typename T>
class List {
public:
    class Iterator : public std::iterator<std::bidirectional_iterator_tag, T> {
    public:
        Iterator() = default;
        Iterator(ListHook* l) : lh_(l) {
        }

        Iterator& operator++() {
            lh_ = lh_->right_;

            return *this;
        }
        Iterator operator++(int) {
            auto old(*this);
            this->operator++();

            return old;
        }

        T& operator*() const {
            return static_cast<T&>(*(lh_));
        }
        T* operator->() const {
            return static_cast<T*>(lh_);
        }

        bool operator==(const Iterator& rhs) const {
            return lh_ == rhs.lh_;
        }
        bool operator!=(const Iterator& rhs) const {
            return !(*this == rhs);
        }

    private:
        ListHook* lh_ = nullptr;
    };

    List() {
        Init();
    }
    List(const List&) = delete;
    List(List&& other) {
        Init();

        while (!other.IsEmpty()) {
            T* ptr = static_cast<T*>(other.back_->left_);
            other.PopBack();
            PushFront(ptr);
        }
    }

    // must unlink all elements from list
    ~List() {
        while (!IsEmpty()) {
            PopBack();
        }
        delete front_;
        delete back_;
    }

    List& operator=(const List&) = delete;
    List& operator=(List&& other) {
        while (!IsEmpty()) {
            PopBack();
        }

        while (!other.IsEmpty()) {
            T* ptr = static_cast<T*>(other.back_->left_);
            other.PopBack();
            PushFront(ptr);
        }

        return *this;
    }

    bool IsEmpty() const {
        return front_->right_ == back_;
    }
    // that method is allowed to be O(n)
    size_t Size() const {
        size_t sz = 0;
        for (auto it = Begin(); it != End(); ++it) {
            sz += 1;
        }

        return sz;
    }

    // note that IntrusiveList doesn't own elements,
    // and never copies or moves T
    void PushBack(T* elem) {
        back_->LinkBefore(static_cast<ListHook*>(elem));
    }
    void PushFront(T* elem) {
        front_->right_->LinkBefore(static_cast<ListHook*>(elem));
    }

    T& Front() {
        return static_cast<T&>(*(front_->right_));
    }
    const T& Front() const {
        return static_cast<T&>(*(front_->right_));
    }

    T& Back() {
        return static_cast<T&>(*(back_->left_));
    }
    const T& Back() const {
        return static_cast<T&>(*(back_->left_));
    }

    void PopBack() {
        back_->left_->Unlink();
    }
    void PopFront() {
        front_->right_->Unlink();
    }

    Iterator Begin() const {
        return Iterator(front_->right_);
    }
    Iterator End() const {
        return Iterator(back_);
    }

    // complexity of this function must be O(1)
    Iterator IteratorTo(T* element) {
        return Iterator(static_cast<ListHook*>(element));
    }

private:
    void Init() {
        front_ = new ListHook;
        back_ = new ListHook;
        front_->right_ = back_;
        back_->left_ = front_;
    }

    ListHook* front_ = nullptr;
    ListHook* back_ = nullptr;
};

template <typename T>
typename List<T>::Iterator begin(List<T>& list) {  // NOLINT
    return list.Begin();
}

template <typename T>
typename List<T>::Iterator end(List<T>& list) {  // NOLINT
    return list.End();
}
