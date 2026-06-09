#ifndef SORTING_H
#define SORTING_H

#include <cstddef>

// ============================================================
// Insertion Sort
// ============================================================
// Best for: small datasets (n <= ~50)
// Stable: yes
// Time: O(n^2), Space: O(1)

template <typename T>
void insertionSort(T arr[], size_t size) {
    for (size_t i = 1; i < size; ++i) {
        T key = arr[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

template <typename T, typename Comparator>
void insertionSort(T arr[], size_t size, Comparator compareFunc) {
    for (size_t i = 1; i < size; ++i) {
        T key = arr[i];
        int j = static_cast<int>(i) - 1;
        while (j >= 0 && compareFunc(key, arr[j])) {
            arr[j + 1] = arr[j];
            --j;
        }
        arr[j + 1] = key;
    }
}

// ============================================================
// Quick Sort
// ============================================================
// Best for: general-purpose large datasets
// Stable: no
// Time: O(n log n) average; Space: O(log n)

namespace detail {

template <typename T>
int quickSortPartition(T arr[], int low, int high) {
    int mid = low + (high - low) / 2;

    if (arr[mid] < arr[low]) {
        T temp = arr[low]; arr[low] = arr[mid]; arr[mid] = temp;
    }
    if (arr[high] < arr[low]) {
        T temp = arr[low]; arr[low] = arr[high]; arr[high] = temp;
    }
    if (arr[high] < arr[mid]) {
        T temp = arr[mid]; arr[mid] = arr[high]; arr[high] = temp;
    }

    T pivot = arr[mid];
    T temp = arr[mid]; arr[mid] = arr[high - 1]; arr[high - 1] = temp;

    int i = low;
    int j = high - 1;

    while (true) {
        while (arr[++i] < pivot) {}
        while (pivot < arr[--j]) {}
        if (i >= j) break;
        T swapVal = arr[i]; arr[i] = arr[j]; arr[j] = swapVal;
    }

    T swapVal = arr[i]; arr[i] = arr[high - 1]; arr[high - 1] = swapVal;
    return i;
}

template <typename T>
void quickSortRecursive(T arr[], int low, int high) {
    if (low + 10 <= high) {
        int pivot = quickSortPartition(arr, low, high);
        quickSortRecursive(arr, low, pivot - 1);
        quickSortRecursive(arr, pivot + 1, high);
    }
}

template <typename T, typename Comparator>
int quickSortPartition(T arr[], int low, int high, Comparator compareFunc) {
    int mid = low + (high - low) / 2;

    if (compareFunc(arr[mid], arr[low])) {
        T temp = arr[low]; arr[low] = arr[mid]; arr[mid] = temp;
    }
    if (compareFunc(arr[high], arr[low])) {
        T temp = arr[low]; arr[low] = arr[high]; arr[high] = temp;
    }
    if (compareFunc(arr[high], arr[mid])) {
        T temp = arr[mid]; arr[mid] = arr[high]; arr[high] = temp;
    }

    T pivot = arr[mid];
    T temp = arr[mid]; arr[mid] = arr[high - 1]; arr[high - 1] = temp;

    int i = low;
    int j = high - 1;

    while (true) {
        while (compareFunc(arr[++i], pivot)) {}
        while (compareFunc(pivot, arr[--j])) {}
        if (i >= j) break;
        T swapVal = arr[i]; arr[i] = arr[j]; arr[j] = swapVal;
    }

    T swapVal = arr[i]; arr[i] = arr[high - 1]; arr[high - 1] = swapVal;
    return i;
}

template <typename T, typename Comparator>
void quickSortRecursive(T arr[], int low, int high, Comparator compareFunc) {
    if (low + 10 <= high) {
        int pivot = quickSortPartition(arr, low, high, compareFunc);
        quickSortRecursive(arr, low, pivot - 1, compareFunc);
        quickSortRecursive(arr, pivot + 1, high, compareFunc);
    }
}

} // namespace detail

template <typename T>
void quickSort(T arr[], size_t size) {
    if (size <= 1) return;
    detail::quickSortRecursive(arr, 0, static_cast<int>(size) - 1);
    insertionSort(arr, size);
}

template <typename T, typename Comparator>
void quickSort(T arr[], size_t size, Comparator compareFunc) {
    if (size <= 1) return;
    detail::quickSortRecursive(arr, 0, static_cast<int>(size) - 1, compareFunc);
    insertionSort(arr, size, compareFunc);
}

// ============================================================
// Merge Sort
// ============================================================
// Best for: stable sorting, large datasets
// Stable: yes
// Time: O(n log n), Space: O(n)

namespace detail {

template <typename T>
void mergeSortMerge(T arr[], T temp[], int left, int mid, int right) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (arr[i] <= arr[j]) temp[k++] = arr[i++];
        else temp[k++] = arr[j++];
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int idx = left; idx <= right; ++idx) arr[idx] = temp[idx];
}

template <typename T>
void mergeSortRecursive(T arr[], T temp[], int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortRecursive(arr, temp, left, mid);
    mergeSortRecursive(arr, temp, mid + 1, right);
    mergeSortMerge(arr, temp, left, mid, right);
}

template <typename T, typename Comparator>
void mergeSortMerge(T arr[], T temp[], int left, int mid, int right, Comparator compareFunc) {
    int i = left, j = mid + 1, k = left;
    while (i <= mid && j <= right) {
        if (compareFunc(arr[i], arr[j]) ||
            (!compareFunc(arr[j], arr[i]) && !compareFunc(arr[i], arr[j]))) {
            temp[k++] = arr[i++];
        } else {
            temp[k++] = arr[j++];
        }
    }
    while (i <= mid) temp[k++] = arr[i++];
    while (j <= right) temp[k++] = arr[j++];
    for (int idx = left; idx <= right; ++idx) arr[idx] = temp[idx];
}

template <typename T, typename Comparator>
void mergeSortRecursive(T arr[], T temp[], int left, int right, Comparator compareFunc) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    mergeSortRecursive(arr, temp, left, mid, compareFunc);
    mergeSortRecursive(arr, temp, mid + 1, right, compareFunc);
    mergeSortMerge(arr, temp, left, mid, right, compareFunc);
}

} // namespace detail

template <typename T>
void mergeSort(T arr[], size_t size) {
    if (size <= 1) return;
    T* temp = new T[size];
    detail::mergeSortRecursive(arr, temp, 0, static_cast<int>(size) - 1);
    delete[] temp;
}

template <typename T, typename Comparator>
void mergeSort(T arr[], size_t size, Comparator compareFunc) {
    if (size <= 1) return;
    T* temp = new T[size];
    detail::mergeSortRecursive(arr, temp, 0, static_cast<int>(size) - 1, compareFunc);
    delete[] temp;
}

#endif // SORTING_H