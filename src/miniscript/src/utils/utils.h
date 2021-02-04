/*
 *  Copyright (c) 2021 Thakee Nathees
 *  Licensed under: MIT License
 */

#ifndef MS_UTILS_H
#define MS_UTILS_H

/** Returns the smallest power of two that is equal to or greater than [n].
 * Copyied from : https://github.com/wren-lang/wren/blob/main/src/vm/wren_utils.h#L119
 * Reference    : http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2Float */
int ms_powerOf2Ceil(int n);


#endif // MS_UTILS_H