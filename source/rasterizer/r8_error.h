/*
 * r8_error.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_ERROR_H
#define R8_ERROR_H

#include "r8_types.h"
#include "r8_error_codes.h"


#define R8_ERROR(err)            r8_error_set(err, __FUNCTION__)
#define R8_SET_ERROR_FATAL(msg)  r8_error_set(R8_ERROR_FATAL, msg)


void r8_error_set(R8enum errorID, const char* info);
R8enum r8_error_get();

/// Sets the error event handler.
void r8_error_set_handler(R8_ERROR_HANDLER_PROC errorHandler);


#endif
