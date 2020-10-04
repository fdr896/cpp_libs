#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <tuple>
#include <numeric>
#include <vector>
#include <unordered_set>

static std::vector<int> daysInYear;
static std::vector<int> daysInMonth;
static std::vector<int> daysInMonthLeapYear;

class Date {
private:
    int day;
    int month;
    int year;

    static inline const int MIN_YEAR = 1970;
    static inline const int MAX_YEAR = 3500;

    static inline const int MIN_MONTH = 1;
    static inline const int MAX_MONTH = 12;

    static inline const int MIN_DAY = 1;

    static inline const int DAYS_IN_YEAR = 365;
    static inline const int DAYS_IN_LEAP_YEAR = 366;

    static inline const std::unordered_set<int> longMonths = {1, 3, 5, 7, 8, 10, 12};

    bool is_leap_year(int y) const {
        return (y % 4 == 0 && y % 100 != 0) ||
               (y % 100 == 0 && y % 400 == 0);
    }

    int days_in_year(int y) const {
        return DAYS_IN_YEAR + static_cast<int>(is_leap_year(y));
    }

    int days_in_month(int m) const {
        if (m == 2) {
            return static_cast<int>(is_leap_year(year)) + 28;
        } else if (longMonths.count(m)) {
            return 31;
        } else {
            return 30;
        }
    }

    int days_since_beginning() const {
        int daysSinceBeginning = 0;
        daysSinceBeginning += daysInYear[year - 1];

        daysSinceBeginning += (is_leap_year(year) ?
                               daysInMonthLeapYear[month - 1] :
                               daysInMonth[month - 1]);

        daysSinceBeginning += day;

        return daysSinceBeginning;
    }

    Date from_days_to_date(int days) const {
        int year = static_cast<int>(std::lower_bound(
                daysInYear.begin(),
                daysInYear.end(), days) - daysInYear.begin());
        days -= daysInYear[year - 1];

        int month;
        if (is_leap_year(year)) {
            month = static_cast<int>(std::lower_bound(
                    daysInMonthLeapYear.begin(),
                    daysInMonthLeapYear.end(), days) - daysInMonthLeapYear.begin());
            days -= daysInMonthLeapYear[month - 1];
        } else {
            month = static_cast<int>(std::lower_bound(
                    daysInMonth.begin(),
                    daysInMonth.end(), days) - daysInMonth.begin());
            days -= daysInMonth[month - 1];
        }

        return {days, month, year};
    }

public:
    Date() {
        auto d = Date(0, 0, 1970);
    }

    Date(int _day, int _month, int _year) {
        if (_year < MIN_YEAR || MAX_YEAR < _year) {
            throw std::invalid_argument("too early");
        }

        if (_month < MIN_MONTH || MAX_MONTH < _month) {
            throw std::invalid_argument("invalid month");
        }

        if (_day < MIN_DAY || days_in_month(_day) < _day) {
            throw std::invalid_argument("invalid day");
        }

        day = _day;
        month = _month;
        year = _year;

        if (daysInYear.empty()) {
            daysInYear = std::vector<int>(MAX_YEAR + 1);
            for (int y = 1; y <= MAX_YEAR; ++y) {
                daysInYear[y] = days_in_year(y);
            }

            std::partial_sum(
                daysInYear.begin() + 1,
                daysInYear.begin() + MAX_YEAR + 1,
                daysInYear.begin() + 1);
        }

        if (daysInMonth.empty()) {
            daysInMonth = std::vector<int>(MAX_MONTH + 1);

            for (int m = MIN_MONTH; m <= MAX_MONTH; ++m) {
                daysInMonth[m] = days_in_month(m);
            }

            std::partial_sum(
                daysInMonth.begin() + MIN_MONTH,
                daysInMonth.begin() + MAX_MONTH+ 1,
                daysInMonth.begin() + MIN_MONTH);

            daysInMonthLeapYear = std::vector<int>(MAX_MONTH + 1);

            for (int m = MIN_MONTH; m <= MAX_MONTH; ++m) {
                daysInMonthLeapYear[m] = days_in_month(m);
                if (m == 2) {
                    ++daysInMonthLeapYear[m];
                }
            }

            std::partial_sum(
                daysInMonthLeapYear.begin() + MIN_MONTH,
                daysInMonthLeapYear.begin() + MAX_MONTH + 1,
                daysInMonthLeapYear.begin() + MIN_MONTH);
        }
    }

    int GetDay() const {
        return day;
    }

    int GetMonth() const {
        return month;
    }

    int GetYear() const {
        return year;
    }

    Date operator+(int delta) const {
        int days = this->days_since_beginning() + delta;

        return from_days_to_date(days);
    }

    Date operator-(int delta) const {
        int days = this->days_since_beginning() - delta;

        return from_days_to_date(days);
    }

    int operator-(const Date& other) const {
        return this->days_since_beginning() - other.days_since_beginning();
    }

    Date& operator++() {
        ++day;

        if (day == 1 + days_in_month(month)) {
            day = MIN_DAY;
            ++month;

            if (month == 1 + MAX_MONTH) {
                month = MIN_MONTH;
                ++year;
            }
        }

        return *this;
    }

    Date& operator--() {
        --day;

        if (day == MIN_DAY - 1) {
            --month;
            if (month == MIN_MONTH - 1) {
                month = MAX_MONTH;
                --year;
            }
            day = days_in_month(month);
        }

        return *this;
    }
};
