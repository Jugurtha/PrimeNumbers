//
// Created by Jugurtha on 25/07/2018.
//

#include <stdint.h>


/**
 * \brief An iterative binary search function. It returns location of x in given array arr[l..r] if present,
 * otherwise returns the index of the smallest element, immediately superior to x in the array.
 * \param arr the sorted array to search into
 * \param l the index from which search starts
 * \param r the index from which search starts
 * \param x the element to search for
 * \return index of x or the index of the smallest element, immediately superior to x in the array.
 */
uint32_t binarySearchGreaterOrEqual(const uint32_t arr[], uint32_t l, uint32_t r, uint32_t x)
{
    uint32_t m;
    while (l <= r)
    {
        m = l + (r-l)/2;
        // Check if x is present at mid
        if (arr[m] == x)
            return m;

        // If x greater, ignore left half
        if (arr[m] < x)
            l = m + 1;

            // If x is smaller, ignore right half
        else
            r = m - 1;
    }

    // if we reach here, then element was
    // not present and we return the element in the array that's immediately superior to x.
    return l;
}