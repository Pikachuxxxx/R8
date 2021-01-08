/* r8_error.c
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#include "r8_error.h"

static R8enum error_ = R8_ERROR_NONE;
static R8_ERROR_HANDLER_PROC errorHandler_ = NULL;

R8void r8SetError(R8enum errorID, const char* msg)
{
    error_ = errorID;
    if (errorHandler_ != NULL)
        errorHandler_(errorID, msg);
}

R8enum r8GetErrorCode()
{
    return error_;
}

R8void r8SetErrorHandler(R8_ERROR_HANDLER_PROC errorHandler)
{
    errorHandler_ = errorHandler;
}
