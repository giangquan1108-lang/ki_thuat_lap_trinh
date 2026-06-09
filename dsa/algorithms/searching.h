#ifndef SEARCHING_H
#define SEARCHING_H

#include <cstddef>

// ============================================================
// Linear Search
// ============================================================

// Returns index of first match, or -1 if not found
template <typename T>
int linearSearch(const T arr[], size_t size, const T& target) {
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Overload with custom comparator
template <typename T, typename Comparator>
int linearSearch(const T arr[], size_t size, const T& target, Comparator matchFunc) {
    for (size_t i = 0; i < size; ++i) {
        if (matchFunc(arr[i], target)) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

// Find all matches, stores indices in result array
template <typename T>
size_t linearSearchAll(const T arr[], size_t size, const T& target,
                       int result[], size_t maxResults) {
    size_t count = 0;
    for (size_t i = 0; i < size && count < maxResults; ++i) {
        if (arr[i] == target) {
            result[count++] = static_cast<int>(i);
        }
    }
    return count;
}

// ============================================================
// Binary Search
// ============================================================

// Returns index of target, or -1 if not found
template <typename T>
int binarySearch(const T arr[], size_t size, const T& target) {
    if (size == 0) return -1;

    int left = 0;
    int right = static_cast<int>(size) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (arr[mid] == target) {
            return mid;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return -1;
}

// Overload with custom comparator
template <typename T, typename Comparator>
int binarySearch(const T arr[], size_t size, const T& target, Comparator compareFunc) {
    if (size == 0) return -1;

    int left = 0;
    int right = static_cast<int>(size) - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        int cmp = compareFunc(arr[mid], target);

        if (cmp == 0) return mid;
        else if (cmp < 0) left = mid + 1;
        else right = mid - 1;
    }
    return -1;
}

// Binary search for first occurrence
template <typename T>
int binarySearchFirst(const T arr[], size_t size, const T& target) {
    if (size == 0) return -1;

    int left = 0;
    int right = static_cast<int>(size) - 1;
    int result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            result = mid;
            right = mid - 1;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

// Binary search for last occurrence
template <typename T>
int binarySearchLast(const T arr[], size_t size, const T& target) {
    if (size == 0) return -1;

    int left = 0;
    int right = static_cast<int>(size) - 1;
    int result = -1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (arr[mid] == target) {
            result = mid;
            left = mid + 1;
        } else if (arr[mid] < target) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return result;
}

#endif // SEARCHING_H