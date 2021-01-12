/*
 * r8_error_codes.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R8_ERROR_CODES_H
#define R8_ERROR_CODES_H


typedef void (*R8_ERROR_HANDLER_PROC)(R8enum errorID, const char* info);


/// No error
#define R8_ERROR_NONE                   0

/// Null pointer error
#define R8_ERROR_NULL_POINTER           1

/// Invalid argument error
#define R8_ERROR_INVALID_ARGUMENT       2

/// Invalid ID error
#define R8_ERROR_INVALID_ID             3

/// Invalid state error
#define R8_ERROR_INVALID_STATE          4

/// Index out of bounds error
#define R8_ERROR_INDEX_OUT_OF_BOUNDS    5

/// Argument mismatch error
#define R8_ERROR_ARGUMENT_MISMATCH      6

/// Missing plugin error
#define R8_ERROR_MISSING_PLUGIN         7

/// Unexpected end-of-file error
#define R8_ERROR_UNEXPECTED_EOF         8

/// Context creation error (e.g. when X11 server is missing on a Linux system)
#define R8_ERROR_CONTEXT                9

/// Fatal error (occures only in debug mode)
#define R8_ERROR_FATAL                  10


#endif
