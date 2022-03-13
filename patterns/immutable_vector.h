#pragma once

#include <vector>
#include <memory>

template <class T>
class ImmutableVector {
    inline static const std::vector<size_t> kCumSum = {
        (1 << 5),
        (1 << 5) + (1 << 10),
        (1 << 5) + (1 << 10) + (1 << 15),
        (1 << 5) + (1 << 10) + (1 << 15) + (1 << 20),
        (1 << 5) + (1 << 10) + (1 << 15) + (1 << 20) + (1 << 25),
        (1 << 5) + (1 << 10) + (1 << 15) + (1 << 20) + (1 << 25) + (1 << 30)};

    class Trie {
        template <typename U>
        friend class ImmutableVector;

        inline static const size_t kElements = 32;

        struct Node {
            std::vector<T> values_;
            std::vector<std::shared_ptr<Node>> next_;

            Node() {
                values_.resize(kElements);
                next_.resize(kElements);
            }

            Node(const Node& other) : values_(other.values_), next_(other.next_) {
            }
            Node(Node&& other) = delete;
        };

    public:
        Trie() {
            root_ = std::make_shared<Node>();
        }

        Trie(const Trie& other) : root_(other.root_) {
        }
        Trie(Trie&& other) = delete;

        Trie(std::shared_ptr<Node> root) : root_(root) {
        }

        Trie& operator=(const Trie& other) {
            if (this == &other) {
                return *this;
            }

            root_ = other.root_;

            return *this;
        }
        Trie& operator=(Trie&& other) {
            if (this == &other) {
                return *this;
            }

            root_ = std::move(other.root_);

            return *this;
        }

        Trie Set(size_t index, const T& value) {
            if (root_->next_[0] == nullptr) {
                root_->next_[0] = std::make_shared<Node>();
                root_->next_[0]->values_[index] = value;
                return *this;
            }

            auto path = GetPath(index);

            return Trie(TraverseAndCopy(root_, GetPosInLevel(index) % kElements, value, path, 0));
        }

        const T& Get(size_t index) const {
            auto path = GetPath(index);
            return Traverse(root_, GetPosInLevel(index) % kElements, path, 0);
        }

    public:
        const T& Traverse(std::shared_ptr<Node> ptr, size_t index, const std::vector<size_t>& path,
                          size_t path_pos) const {
            if (ptr == root_) {
                return Traverse(ptr->next_[0], index, path, path_pos);
            }

            if (path_pos == path.size()) {
                return ptr->values_[index];
            }

            size_t next_node = path[path_pos];
            return Traverse(ptr->next_[next_node], index, path, path_pos + 1);
        }
        std::shared_ptr<Node> TraverseAndCopy(std::shared_ptr<Node> ptr, size_t index,
                                              const T& value, const std::vector<size_t>& path,
                                              size_t path_pos) {
            if (ptr == root_) {
                auto new_node = TraverseAndCopy(ptr->next_[0], index, value, path, path_pos);
                auto new_root(std::make_shared<Node>(*ptr));
                new_root->next_[0] = new_node;

                return new_root;
            }

            if (path_pos == path.size()) {
                std::shared_ptr<Node> new_node(std::make_shared<Node>(*ptr));
                new_node->values_[index] = value;

                return new_node;
            }

            size_t next_node = path[path_pos];
            if (ptr->next_[next_node] == nullptr) {
                ptr->next_[next_node] = std::make_shared<Node>();
            }

            auto new_node =
                TraverseAndCopy(ptr->next_[next_node], index, value, path, path_pos + 1);
            auto curr_new_node(std::make_shared<Node>(*ptr));
            curr_new_node->next_[next_node] = new_node;

            return curr_new_node;
        }

        size_t GetSum(int pos) const {
            if (pos == -1) {
                return 0;
            }

            return kCumSum[pos];
        }

        size_t GetLevel(size_t index) const {
            return std::upper_bound(kCumSum.begin(), kCumSum.end(), index) - kCumSum.begin();
        }

        size_t GetPosInLevel(size_t index) const {
            if (index < kElements) {
                return index;
            }

            return index - kCumSum[GetLevel(index) - 1];
        }

        size_t GetBlockInLevel(size_t index) const {
            return GetPosInLevel(index) / kElements;
        }

        size_t GetNumberByBlock(int level, size_t pos) const {
            return GetSum(level - 1) + pos * kElements;
        }

        std::vector<size_t> GetPath(size_t index) const {
            size_t level = GetLevel(index);
            std::vector<size_t> res;
            res.reserve(level);
            while (level > 0) {
                auto curr_block = GetBlockInLevel(index);
                res.emplace_back(curr_block % kElements);
                level -= 1;
                index = GetNumberByBlock(level, curr_block / kElements);
            }

            reverse(res.begin(), res.end());

            return res;
        }

        std::shared_ptr<Node> root_{};
        std::vector<int> anc;
    };

public:
    ImmutableVector() {
    }

    explicit ImmutableVector(size_t count, const T& value = T()) {
        for (size_t i = 0; i < count; ++i) {
            *this = PushBack(value);
        }
    }

    template <typename Iterator>
    ImmutableVector(Iterator first, Iterator last) {
        for (auto it = first; it != last; ++it) {
            *this = PushBack(*it);
        }
    }

    ImmutableVector(std::initializer_list<T> l) {
        for (auto it = l.begin(); it != l.end(); ++it) {
            *this = PushBack(*it);
        }
    }

    ImmutableVector(const ImmutableVector& other) : size_(other.size_), trie_(other.trie_) {
    }

    ImmutableVector& operator=(const ImmutableVector& other) {
        size_ = other.size_;
        trie_ = other.trie_;

        return *this;
    }

    ImmutableVector Set(size_t index, const T& value) {
        auto new_trie = trie_.Set(index, value);

        return ImmutableVector(size_, new_trie);
    }

    const T& Get(size_t index) const {
        static T dummy_value;
        dummy_value = trie_.Get(index);
        return dummy_value;
    }

    ImmutableVector PushBack(const T& value) {
        auto res = Set(size_, value);
        res.size_ += 1;

        return res;
    }

    ImmutableVector PopBack() {
        return ImmutableVector(size_ - 1, trie_);
    }

    size_t Size() const {
        return size_;
    }

private:
    ImmutableVector(size_t size, Trie trie) : size_(size), trie_(trie) {
    }

    size_t size_{};
    Trie trie_{};
};
