/* r8_error_codes.h
 *
 * This file is part of the "R8" (Copyright(c) 2021 by Phani Srikar (Pikachuxxxx))
 * See "LICENSE.txt" for license information.
 */

#ifndef R_8_ERROR_CODES_H
#define R_8_ERROR_CODES_H

typedef void (*R8_ERROR_HANDLER_PROC)(R8enum errorID, const char* info);

/// No Error
#define R8_ERROR_NONE                   0

/// Null pointer error
#define R8_ERROR_NULL_POINTER           1     

/// Invalid argument error 
#define R8_ERROR_INVALID_ARGUMENT       2    

/// Invalid ID error
#define R8_ERROR_INVALID_ID             3

/// Invalid state error for state machine
#define R8_ERROR_INVALID_STATE          4            

/// Array bounds error
#define R8_ERROR_INDEX_OUT_OF_BOUNDS    5   

/// Argument type mismatch error
#define R8_ERROR_ARGUMENT_MISMATCH      6

/// End of File error
#define R8_ERROR_EOF                    7               

/// Context creation error
#define R8_ERROR_CONTEXT                8

/// Fatal error that causes severe crashes
#define R8_ERROR_FATAL                  9

/// Invalid enum error
#define R8_ERROR_INVALID_ENUM           10

#endif