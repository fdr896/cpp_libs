#pragma once

#include <vector>
#include <stdexcept>
#include <array>
#include <type_traits>
#include <numeric>
#include <list>

struct Base {};

template <class L, class R>
struct Glue : public Base {
    Glue(const L& l, const R& r) : left_(l), right_(r) {
    }

    const L& left_;
    const R& right_;
};

template <class L, class R>
std::enable_if_t<std::is_base_of_v<Base, L> && std::is_base_of_v<Base, R>, Glue<L, R>> operator*(
    const L& lhs, const R& rhs) {
    return Glue(lhs, rhs);
}

template <class T>
struct GetCount {
    static constexpr int kCount = 0;
};

template <class L, class R>
struct GetCount<Glue<L, R>> {
    static constexpr int kCount = GetCount<L>::kCount + GetCount<R>::kCount;
};

template <class T>
class Matrix : public Base {
public:
    constexpr Matrix() = default;
    constexpr Matrix(int n) {
        m_.resize(n, std::vector<T>(n, T{}));
    }
    constexpr Matrix(int n, int m) {
        m_.resize(n, std::vector<T>(m, T{}));
    }
    constexpr Matrix(const std::vectr<std::vector<T>>& m) : m_(m) {
    }
    constexpr Matrix(std::initializer_list<std::vector<T>> l) {
        m_.reserve(l.size());
        for (const auto& m : l) {
            m_.emplace_back(m);
        }
    }
    constexpr Matrix(const Matrix& other) : m_(other.m_) {
    }

    Matrix& operator=(const Matrix& other) {
        if (this == &other) {
            return *this;
        }
        m_ = other.m_;

        return *this;
    }

    template <class L, class R>
    constexpr Matrix(const Glue<L, R>& tree) {
        const int count = GetCount<Glue<L, R>>::kCount;

        std::array<const Matrix<T>*, count> ops;
        PlaceMatrix<L, R, T, count>(tree, ops);

        for (int i = 0; i + 1 < count; ++i) {
            if (ops[i]->Columns() != ops[i + 1]->Rows()) {
                throw std::runtime_error("sizes don't satisfies");
            }
        }

        std::array<std::array<size_t, count>, count> dp;
        std::array<std::array<size_t, count>, count> anc;
        for (int i = 0; i < count; ++i) {
            dp[i][i] = 0;
            anc[i][i] = i;
        }
        for (int i = 0; i + 1 < count; ++i) {
            dp[i][i + 1] = ops[i]->Rows() * ops[i]->Columns() * ops[i + 1]->Columns();
            anc[i][i + 1] = i;
        }

        for (int len = 3; len <= count; ++len) {
            for (int i = 0; i + len - 1 < count; ++i) {
                int j = i + len - 1;
                dp[i][j] = std::numeric_limits<size_t>::max();
                // find opt for [i...j]
                for (int k = i; k < j; ++k) {
                    size_t curr = dp[i][k] + dp[k + 1][j] +
                                  ops[i]->Rows() * ops[k]->Columns() * ops[j]->Columns();
                    if (dp[i][j] > curr) {
                        dp[i][j] = curr;
                        anc[i][j] = k;
                    }
                }
            }
        }

        std::array<std::pair<int, int>, count * count + 33> q;
        int beg = 0, end = 0;
        std::array<std::tuple<int, int, int>, count - 1> order;
        int idx = 0;
        q[0] = {0, count - 1};
        end += 1;
        int iters = 0;
        while (beg != end) {
            ++iters;
            auto [l, r] = q[beg];
            beg += 1;
            int nxt = anc[l][r];
            order[idx++] = {l, nxt, r};

            if (l < nxt) {
                q[end] = {l, nxt};
                end += 1;
            }
            if (nxt + 1 < r) {
                q[end] = {nxt + 1, r};
                end += 1;
            }
        }

        std::reverse(order.begin(), order.end());

        std::array<std::array<Matrix, count>, count> result;
        for (int i = 0; i < count; ++i) {
            result[i][i] = *ops[i];
        }

        for (const auto& [l, m, r] : order) {
            const auto& a = result[l][m].m_;
            const auto& b = result[m + 1][r].m_;

            std::vector<std::vector<T>> curr_res(a.size(), std::vector<T>(b[0].size()));

            for (size_t i = 0; i < a.size(); ++i) {
                for (size_t j = 0; j < b[0].size(); ++j) {
                    for (size_t k = 0; k < a[0].size(); ++k) {
                        curr_res[i][j] += a[i][k] * b[k][j];
                    }
                }
            }

            result[l][r] = Matrix(curr_res);
        }

        m_ = result[0][count - 1].m_;
    }

    T& operator()(size_t i, size_t j) {
        return m_[i][j];
    }
    const T& operator()(size_t i, size_t j) const {
        return m_[i][j];
    }

    size_t Rows() const {
        return m_.size();
    }
    size_t Columns() const {
        return m_[0].size();
    }

    friend Matrix operator+(const Matrix& lhs, const Matrix& rhs) {
        Matrix ret(lhs.Rows(), lhs.Columns());
        for (size_t i = 0; i < lhs.Rows(); ++i) {
            for (size_t j = 0; j < lhs.Columns(); ++j) {
                ret(i, j) = lhs(i, j) + rhs(i, j);
            }
        }

        return ret;
    }

    friend Matrix operator-(const Matrix& lhs, const Matrix& rhs) {
        Matrix ret(lhs.Rows(), lhs.Columns());
        for (size_t i = 0; i < lhs.Rows(); ++i) {
            for (size_t j = 0; j < lhs.Columns(); ++j) {
                ret(i, j) = lhs(i, j) - rhs(i, j);
            }
        }

        return ret;
    }

private:
    std::vector<std::vector<T>> m_;
};

template <class L, class R, class T, int S>
constexpr int PlaceMatrix(const Glue<L, R>& m, std::array<const Matrix<T>*, S>& ops) {
    int left = 0, right = 0;
    if constexpr (std::is_same_v<Matrix<T>, L>) {
        ops[0] = &m.left_;
        left = 1;
    } else {
        left = PlaceMatrix<std::decay_t<decltype(m.left_.left_)>,
                           std::decay_t<decltype(m.left_.right_)>, T, S>(m.left_, ops);
    }

    if constexpr (std::is_same_v<Matrix<T>, R>) {
        ops[left] = &m.right_;
        right = 1;
    } else {
        right = PlaceMatrix(m.right_, ops);
    }

    return left + right;
}

template <class T>
struct GetCount<Matrix<T>> {
    static constexpr int kCount = 1;
};
o
