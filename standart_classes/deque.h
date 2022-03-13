#pragma once

#include <initializer_list>
#include <memory>

class Deque {
public:
    Deque() : buffer_() {
    }
    Deque(const Deque& rhs) : buffer_() {
        buffer_ = rhs.buffer_;
    }
    Deque(Deque&& rhs) : buffer_() {
        buffer_ = std::move(rhs.buffer_);
    };
    explicit Deque(size_t size) : buffer_() {
        for (size_t i = 0; i < size; ++i) {
            PushBack(0);
        }
    }

    Deque(std::initializer_list<int> list) {
        for (int l : list) {
            PushBack(l);
        }
    }

    Deque& operator=(const Deque& rhs) {
        if (this == &rhs) {
            return *this;
        }

        buffer_ = rhs.buffer_;
        return *this;
    }
    Deque& operator=(Deque&& rhs) {
        buffer_ = std::move(rhs.buffer_);

        return *this;
    }

    void Swap(Deque& rhs) {
        std::swap(buffer_, rhs.buffer_);
    }

    void PushBack(int value) {
        buffer_.PushBack(value);
    }

    void PopBack() {
        buffer_.PopBack();
    }

    void PusFront(int value) {
        buffer_.PushFront(value);
    }

    void PopFront() {
        buffer_.PopFront();
    }

    int& operator[](size_t ind) {
        return buffer_.At(ind);
    }

    int operator[](size_t ind) const {
        return buffer_.At(ind);
    }

    size_t Size() const {
        return buffer_.Size();
    }

    void Clear() {
        buffer_.Clear();
    }

private:
    class Buffer {
        inline static const size_t kIntsInBlock = 128;

    public:
        Buffer() = default;
        Buffer(const Buffer& other) {
            size_ = other.size_;
            cap_ = other.cap_;
            front_ = other.front_;
            back_ = other.back_;
            blocks_ = other.blocks_;
        }
        Buffer(Buffer&& other) {
            size_ = other.size_;
            cap_ = other.cap_;
            front_ = other.front_;
            back_ = other.back_;
            blocks_ = other.blocks_;
            other.blocks_ = nullptr;
        }

        Buffer& operator=(const Buffer& other) {
            if (this == &other) {
                return *this;
            }

            size_ = other.size_;
            cap_ = other.cap_;
            front_ = other.front_;
            back_ = other.back_;
            blocks_ = other.blocks_;

            return *this;
        }
        Buffer& operator=(Buffer&& other) {
            size_ = other.size_;
            cap_ = other.cap_;
            front_ = other.front_;
            back_ = other.back_;
            blocks_ = other.blocks_;
            other.blocks_ = nullptr;

            return *this;
        }
        ~Buffer() = default;

        size_t Size() const {
            return size_;
        }

        void Clear() {
            size_ = 0;
            front_ = Iterator(0, 0, cap_);
            back_ = Iterator(0, 0, cap_);
        }

        int& At(size_t ind) {
            if (front_.pos + ind < kIntsInBlock) {
                return blocks_.get()[front_.block].data_[front_.pos + ind];
            }
            ind -= (kIntsInBlock - front_.pos);
            size_t full_blocks = ind / kIntsInBlock;
            size_t new_block = (front_.block + 1 + full_blocks) % cap_;
            return blocks_.get()[new_block].data_[ind % kIntsInBlock];
        }
        int At(size_t ind) const {
            if (front_.pos + ind < kIntsInBlock) {
                return blocks_.get()[front_.block].data_[front_.pos + ind];
            }
            ind -= (kIntsInBlock - front_.pos);
            size_t full_blocks = ind / kIntsInBlock;
            size_t new_block = (front_.block + 1 + full_blocks) % cap_;
            return blocks_.get()[new_block].data_[ind % kIntsInBlock];
        }

        void PushBack(int value) {
            Resize();

            if (size_ == 1) {
                blocks_.get()[0] = Block();
                blocks_.get()[0].data_[0] = value;
                return;
            }
            if (back_.Next()) {
                if (front_.block == back_.block) {
                    back_.Prev();
                    size_ -= 1;
                    Resize(true);
                    PushBack(value);
                }
                blocks_.get()[back_.block] = Block();
            }
            blocks_.get()[back_.block].data_[back_.pos] = value;
        }
        void PopBack() {
            size_ -= 1;

            if (size_ == 0) {
                Clear();
                return;
            }

            back_.Prev();
        }
        void PushFront(int value) {
            Resize();

            if (size_ == 1) {
                blocks_.get()[0] = Block();
                blocks_.get()[0].data_[kIntsInBlock - 1] = value;
                front_ = Iterator(0, kIntsInBlock - 1, cap_);
                back_ = Iterator(0, kIntsInBlock - 1, cap_);
                return;
            }
            if (front_.Prev()) {
                if (front_.block == back_.block) {
                    front_.Next();
                    size_ -= 1;
                    Resize(true);
                    PushFront(value);
                }
                blocks_.get()[front_.block] = Block();
            }
            blocks_.get()[front_.block].data_[front_.pos] = value;
        }
        void PopFront() {
            size_ -= 1;

            if (size_ == 0) {
                Clear();
                return;
            }

            front_.Next();
        }

    private:
        void Resize(bool force = false) {
            if (force || kIntsInBlock * cap_ == size_) {
                if (cap_ == 0) {
                    size_ = 1;
                    cap_ = 1;
                    front_ = Iterator(0, 0, cap_);
                    back_ = Iterator(0, 0, cap_);
                    blocks_ = std::shared_ptr<Block>(new Block[cap_], [](Block* b) { delete[] b; });

                    return;
                }
                size_t new_cap = 2 * cap_;
                std::shared_ptr<Block> new_blocks =
                    std::shared_ptr<Block>(new Block[new_cap], [](Block* b) { delete[] b; });

                size_t iters = cap_;
                for (size_t i = front_.block, block = 0; iters > 0;
                     i = (i + 1) % cap_, ++block, --iters) {
                    new_blocks.get()[block] = std::move(blocks_.get()[i]);
                }
                front_ = Iterator(0, front_.pos, new_cap);
                back_ = Iterator(cap_ - 1, back_.pos, new_cap);
                cap_ = new_cap;
                blocks_ = new_blocks;
                new_blocks = nullptr;
            }
            if (!force) {
                size_ += 1;
            }
        }

        struct Block {
            Block() : data_(new int[kIntsInBlock]) {
                for (size_t i = 0; i < kIntsInBlock; ++i) {
                    data_[i] = 0;
                }
            }
            Block& operator=(Block&& other) {
                data_ = std::move(other.data_);

                return *this;
            }
            ~Block() = default;

            std::unique_ptr<int[]> data_;
        };
        struct Iterator {
            Iterator() = default;
            Iterator(size_t b, size_t p, size_t c) : block(b), pos(p), cap(c) {
            }

            bool operator==(const Iterator& other) {
                return (block == other.block) && (pos == other.pos);
            }
            bool operator!=(const Iterator& other) {
                return !(*this == other);
            }

            bool Next() {
                pos += 1;
                if (pos == kIntsInBlock) {
                    pos = 0;
                    block += 1;
                    if (block == cap) {
                        block = 0;
                    }

                    return true;
                }

                return false;
            }
            bool Prev() {
                pos -= 1;
                if (pos == -1) {
                    pos = kIntsInBlock - 1;
                    block -= 1;
                    if (block == -1) {
                        block = cap - 1;
                    }

                    return true;
                }

                return false;
            }

            size_t block = 0;
            size_t pos = 0;
            size_t cap = 0;
        };

        size_t size_ = 0;
        size_t cap_ = 0;
        Iterator front_;
        Iterator back_;
        std::shared_ptr<Block> blocks_;
    } buffer_;
};
