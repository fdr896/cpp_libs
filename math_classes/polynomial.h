#include <iostream>
#include <algorithm>
#include <vector>

template<typename T>
class Polynomial {
private:
    std::vector<T> coafs;

    void delete_leading_zeroes() {
        while (!coafs.empty() && coafs.back() == T()) {
            coafs.pop_back();
        }
    }

    Polynomial get_single(const T& coaf, int deg) const {
        std::vector<T> tmp(deg + 1);
        tmp.back() = coaf;

        return Polynomial(tmp);
    }

public:
    Polynomial(const std::vector<T>& _coafs): coafs(_coafs) {
        this->delete_leading_zeroes();
    }

    Polynomial(const T& coaf = T()) {
        coafs = std::vector<T>(1, coaf);
        this->delete_leading_zeroes();
    }

    template<typename It>
    Polynomial(It first, It last) {
        coafs = std::vector<T>(0);
        while (first != last) {
            coafs.push_back(*first);
            first = next(first);
        }
        this->delete_leading_zeroes();
    }

    bool operator==(const Polynomial& other) const {
        if (coafs.size() != other.coafs.size()) {
            return false;
        }
        return coafs == other.coafs;
    }

    bool operator!=(const Polynomial& other) const {
        return !(*this == other);
    }

    bool operator==(const T scalar) const {
        return *this == Polynomial(scalar);
    }

    bool operator!=(const T scalar) const {
        return !(*this == Polynomial(scalar));
    }

    T operator[](size_t i) const {
        if (i < coafs.size()) {
            return coafs[i];
        }
        return T();
    }

    int Degree() const {
        return static_cast<int>(coafs.size()) - 1;
    }

    Polynomial operator+(const Polynomial& other) const {
        size_t maxSize = std::max(coafs.size(), other.coafs.size());

        std::vector<T> result(maxSize);
        for (size_t i = 0; i < maxSize; ++i) {
            result[i] = (*this)[i] + other[i];
        }

        return Polynomial(result);
    }

    Polynomial operator+(const T& scalar) const {
        return *this + Polynomial(scalar);
    }

    Polynomial& operator+=(const Polynomial& other) {
        *this = *this + other;

        return *this;
    }

    Polynomial operator-(const Polynomial& other) const {
        size_t maxSize = std::max(coafs.size(), other.coafs.size());

        std::vector<T> result(maxSize);
        for (size_t i = 0; i < maxSize; ++i) {
            result[i] = (*this)[i] - other[i];
        }

        return Polynomial(result);
    }

    Polynomial operator-(const T& scalar) const {
        return *this - Polynomial(scalar);
    }

    Polynomial& operator-=(const Polynomial& other) {
        *this = *this - other;

        return *this;
    }

    typename std::vector<T>::const_iterator begin() const {
        return coafs.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return coafs.end();
    }

    T operator()(const T& x) const {
        if (this->Degree() == -1) {
            return T();
        }
        if (this->Degree() == 0) {
            return coafs[0];
        }

        T result = coafs.back();
        for (int i = static_cast<int>(coafs.size()) - 2; i >= 0; --i) {
            result *= x;
            result += coafs[i];
        }

        return result;
    }

    Polynomial operator*(const Polynomial& other) const {
        if (this->Degree() == -1 || other.Degree() == -1) {
            return Polynomial(0);
        }

        size_t resultSize = coafs.size() + other.coafs.size();

        std::vector<T> result(resultSize);
        for (size_t i = 0; i < coafs.size(); ++i) {
            for (size_t j = 0; j < other.coafs.size(); ++j) {
                result[i + j] += (*this)[i] * other[j];
            }
        }

        return Polynomial(result);
    }

    Polynomial operator*(const T& scalar) const {
        return *this * Polynomial(scalar);
    }

    Polynomial& operator*=(const Polynomial& other) {
        *this = *this * other;

        return *this;
    }

    Polynomial operator&(const Polynomial& other) const {
        if (this->Degree() == -1) {
            return Polynomial();
        }
        if (this->Degree() == 0) {
            return *this;
        }

        Polynomial result(coafs.back());
        for (int i = static_cast<int>(coafs.size()) - 2; i >= 0; --i) {
            result *= other;
            result += Polynomial(coafs[i]);
        }

        return result;
    }

    Polynomial operator/(const Polynomial& other) const {
        Polynomial divisible(*this);
        Polynomial result;

        while (divisible.Degree() >= other.Degree()) {
            int deg = divisible.Degree() - other.Degree();
            T coaf = divisible[divisible.Degree()] / other[other.Degree()];

            Polynomial newMember = get_single(coaf, deg);

            result += newMember;
            divisible -= (newMember * other);
        }

        return result;
    }

    Polynomial operator%(const Polynomial& other) const {
        return *this - ((*this / other) * other);
    }

    Polynomial operator,(const Polynomial& other) const {
        if (this->Degree() == -1) {
            return other;
        }
        if (other.Degree() == -1) {
            return *this;
        }

        Polynomial lhs(*this);
        Polynomial rhs(other);

        while (rhs.Degree() != -1) {
            lhs = lhs % rhs;
            std::swap(lhs, rhs);
        }

        if (lhs.Degree() != -1) {
            auto div = lhs[lhs.Degree()];
            for (int i = 0; i <= lhs.Degree(); ++i) {
                lhs.coafs[i] /= div;
            }
        }

        return lhs;
    }
};

template<typename T>
Polynomial<T> operator+(const T& scalar, const Polynomial<T>& pol) {
    return Polynomial(scalar) + pol;
}

template<typename T>
Polynomial<T> operator-(const T& scalar, const Polynomial<T>& pol) {
    return Polynomial(scalar) - pol;
}

template<typename T>
Polynomial<T> operator*(const T& scalar, const Polynomial<T>& pol) {
    return Polynomial(scalar) * pol;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const Polynomial<T>& pol) {
    int deg = pol.Degree();

    auto abs = [](const T& coaf) -> T {
        if (coaf >= T(0)) {
            return coaf;
        }
        return -coaf;
    };

    if (deg == -1) {
        return out << 0;
    }
    if (deg == 0) {
        if (pol[0] == T()) {
            return out;
        }
        if (pol[0] == -abs(pol[0])) {
            out << "-";
        }
        return out << abs(pol[0]);
    }

    auto printCoaf = [&out, &abs](const T& coaf, bool isFirst, bool isLast) {
        if (!isFirst) {
            if (coaf == abs(coaf)) {
                out << "+";
            }
        }
        if (coaf == -abs(coaf)) {
            out << "-";
        }
        if (isLast) {
            out << abs(coaf);
        } else {
            if (abs(coaf) != T(1)) {
                out << abs(coaf);
                out << "*";
            }
        }
    };

    auto printDegree = [&out](int deg) {
        if (deg > 0) {
            out << "x";
        }
        if (deg > 1) {
            out << "^" << deg;
        }
    };

    for (int currentDeg = deg; currentDeg >= 0; --currentDeg) {
        if (pol[currentDeg] == T(0)) {
            continue;
        }

        printCoaf(pol[currentDeg], currentDeg == deg, currentDeg == 0);
        printDegree(currentDeg);
    }

    return out;
}
