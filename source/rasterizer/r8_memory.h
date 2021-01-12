/*
 * r8_memory.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_MEMORY_H
#define R8_MEMORY_H


#define R8_MALLOC(t)        (t*)malloc(sizeof(t))
#define R8_CALLOC(t, n)     (t*)calloc(n, sizeof(t))

#define R8_FREE(m)      \
    if ((m) != NULL)    \
    {                   \
        free(m);        \
        m = NULL;       \
    }

#define R8_ZERO_MEMORY(m)   memset(&m, 0, sizeof(m))


#endif
