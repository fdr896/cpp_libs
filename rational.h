#include <numeric>

class Rational {
private:
    int nomin, denomin;

    void normalize() {
        int gcd = std::gcd(nomin, denomin);

        nomin /= gcd;
        denomin /= gcd;

        if (denomin < 0) {
            nomin = -nomin;
            denomin = -denomin;
        }
    }

public:
    Rational(int n = 0, int m = 1): nomin(n), denomin(m) {
        this->normalize();
    }

    Rational operator+(const Rational& other) const {
        Rational result = {nomin * other.denomin + denomin * other.nomin,
                           denomin * other.denomin};
        result.normalize();

        return result;
    }

    Rational operator-(const Rational& other) const {
        Rational result = {nomin * other.denomin - denomin * other.nomin,
                           denomin * other.denomin};
        result.normalize();

        return result;
    }

    Rational operator*(const Rational& other) const {
        Rational result = {nomin * other.nomin, denomin * other.denomin};
        result.normalize();

        return result;
    }

    Rational operator/(const Rational& other) const {
        Rational result = {nomin * other.denomin, denomin * other.nomin};
        result.normalize();

        return result;
    }

    Rational operator+(const int number) const {
        return *this + Rational(number, 1);
    }

    Rational operator-(const int number) const {
        return *this - Rational(number, 1);
    }

    Rational operator*(const int number) const {
        return *this * Rational(number, 1);
    }

    Rational operator/(const int number) const {
        return *this / Rational(number, 1);
    }

    Rational& operator+=(const Rational& other) {
        *this = *this + other;

        return *this;
    }

    Rational& operator-=(const Rational& other) {
        *this = *this - other;

        return *this;
    }

    Rational& operator*=(const Rational& other) {
        *this = *this * other;

        return *this;
    }

    Rational& operator/=(const Rational& other) {
        *this = *this / other;

        return *this;
    }

    Rational& operator++() {
        *this += Rational(1, 1);

        return *this;
    }

    Rational& operator--() {
        *this -= Rational(1, 1);

        return *this;
    }

    Rational operator++(int) {
        Rational copy = *this;
        ++(*this);

        return copy;
    }

    Rational operator--(int) {
        Rational copy = *this;
        --(*this);

        return copy;
    }

    bool operator==(const Rational& other) const {
        return (nomin == other.nomin) && (denomin == other.denomin);
    }

    bool operator!=(const Rational& other) const {
        return !(*this == other);
    }

    int numerator() const {
        return nomin;
    }

    int denominator() const {
        return denomin;
    }

    Rational operator+() const {
        return *this;
    }

    Rational operator-() const {
        return Rational(-nomin, denomin);
    }
};

Rational operator+(const int number, const Rational& rat) {
    return Rational(number, 1) + rat;
}

Rational operator-(const int number, const Rational& rat) {
    return Rational(number, 1) - rat;
}

Rational operator*(const int number, const Rational& rat) {
    return Rational(number, 1) * rat;
}

Rational operator/(const int number, const Rational& rat) {
    return Rational(number, 1) / rat;
}
