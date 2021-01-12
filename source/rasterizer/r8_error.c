/*
 * r8_error.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_error.h"
#include "r8_error_codes.h"


static R8enum _error = R8_ERROR_NONE;
static R8_ERROR_HANDLER_PROC _errorHandler = NULL;

void r8_error_set(R8enum errorID, const char* info)
{
    _error = errorID;
    if (_errorHandler != NULL)
        _errorHandler(errorID, info);
}

R8enum r8_error_get()
{
    return _error;
}

void r8_error_set_handler(R8_ERROR_HANDLER_PROC errorHandler)
{
    _errorHandler = errorHandler;
}
