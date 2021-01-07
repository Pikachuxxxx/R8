/* r8_memory.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_MEMORY_H
#define R_8_MEMORY_H

#define R8_MALLOC(T)    (T*)malloc(sizeof(T))
#define R8_CALLOC(T, n) (T*)calloc(n, sizeof(T))

#define R8_FREE(m)      \
    if ((m) != NULL)    \
    {                   \
        free(m);        \
        m = NULL;       \
    }
#define R8_SET__ZERO_MEM(m) memset(&m, 0, sizeof(m))

#endif
