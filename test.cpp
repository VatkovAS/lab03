#include "histogram.h"

#include <cassert>

void
test_positive() {
    double min = 0;
    double max = 0;
    find_minmax({1, 2, 3}, min, max);
    assert(min == 1);
    assert(max == 3);
}

void test_negative()
{
    double min;
    double max;
    find_minmax({ -2, -9,-28 }, min, max);
    assert(min == -28);
    assert(max == -2);
}


void test_one_number()
{
    double min;
    double max;
    find_minmax({ 666 }, min, max);
    assert(min == 666);
    assert(max == 666);
}


void test_same_numbers()
{
    double min;
    double max;
    find_minmax({ 3,3,3 }, min, max);
    assert(min == 3);
    assert(max == 3);
}

int
main() {
    test_positive();
    test_negative();
    test_one_number();
    test_same_numbers();
}
