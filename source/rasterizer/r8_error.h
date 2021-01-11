/* r8_error.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_ERROR_H
#define R_8_ERROR_H

#include "r8_types.h"
#include "r8_error_codes.h"

#define R8_ERROR(err)   r8SetError(err, __FUNCTION__)
#define R8_SET_FATAL_ERROR(msg)  r8SetError(R8_ERROR_FATAL, msg)

R8void r8SetError(R8enum errorID, const char* msg);
R8enum r8GetErrorCode();

R8void r8SetErrorHandler(R8_ERROR_HANDLER_PROC errorHandler);

#endif
