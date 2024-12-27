#include <stdio.h>
#include <time.h>

int is_leap_year(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1; // Leap year
    }
    return 0; // Not a leap year
}

long calculate_unix_time(int year) {
    long total_seconds = 0;
    int start_year = 1970;

    // Calculate seconds from 1970 to the first leap year (1972)
    for (int y = start_year; y < 1972; y++) {
        total_seconds += (is_leap_year(y)) ? 366 * 24 * 3600 : 365 * 24 * 3600;
    }

    // Calculate in 4-year chunks starting from 1972, with careful leap year handling
    for (int y = 1972; y <= year; y += 4) {
        for (int i = 0; i < 4; i++) {
            if (y + i > year) break; // Ensure we don't go past the target year
            total_seconds += (is_leap_year(y + i)) ? 366 * 24 * 3600 : 365 * 24 * 3600;
        }
    }

    // Add the remaining years after the last chunk
    for (int y = year - (year % 4); y < year; y++) {
        total_seconds += (is_leap_year(y)) ? 366 * 24 * 3600 : 365 * 24 * 3600;
    }

    return total_seconds;
}

void compare_with_mktime(int year) {
    // Standard mktime calculation
    struct tm time_info = {0};
    time_info.tm_year = year - 1900; // tm_year is years since 1900
    time_info.tm_mon = 0; // January
    time_info.tm_mday = 1; // First day of the year
    time_info.tm_hour = 0;
    time_info.tm_min = 0;
    time_info.tm_sec = 0;

    // Using mktime to calculate Unix time
    time_t mktime_time = mktime(&time_info);
    if (mktime_time == (time_t)(-1)) {
        printf("mktime() failed for year %d\n", year);
    } else {
        printf("Unix time for year %d (using mktime): %ld seconds\n", year, mktime_time);
    }

    // Compare with custom calculation
    long custom_time = calculate_unix_time(year);
    printf("Unix time for year %d (custom calculation): %ld seconds\n", year, custom_time);

    // Validate the difference
    if (mktime_time != (time_t)(-1) && mktime_time == custom_time) {
        printf("The custom calculation matches mktime for year %d.\n", year);
    } else if (mktime_time != (time_t)(-1)) {
        printf("There is a discrepancy between mktime and custom calculation for year %d.\n", year);
    }
}

int main() {
    int year;
    printf("Enter the year for which you want to calculate Unix time: ");
    scanf("%d", &year);

    // Compare custom calculation with mktime
    compare_with_mktime(year);

    return 0;
}
