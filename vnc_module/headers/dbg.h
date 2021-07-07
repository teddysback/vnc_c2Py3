/**
* \file dbg.h
*
* \brief Debugging messages
*/


#ifndef _DBG_H_
#define _DBG_H_

#include "stdio.h"
#include "string.h"
#include "errno.h"
#include "windows.h"

///
/// Defines
///
#define COLOUR_RESET     FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE
#define COLOUR_WARNING   FOREGROUND_RED | FOREGROUND_GREEN
#define COLOUR_ERROR     FOREGROUND_RED 


#define FILE_LINE_FORMAT "%-6s:% 3d: "

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)  // file name from where it was printed

///
/// Debug is on
///
#ifdef DEBUG_ON
#   define DEBUG(Message, ...) printf(FILE_LINE_FORMAT Message "\n", __FILENAME__, __LINE__, __VA_ARGS__)
#else
#   define DEBUG(Message, ...)
#endif

///
/// Logging
///
#ifdef LOG_ON
#   define LOG_INFO(Message, ...)                               \
            printf(                                             \
                 "[INFO] " Message "\n",                        \
                 __VA_ARGS__                                    \
                 )

#   define _LOG_WARN(Message, ...)                              \
            printf(                                             \
                "[WARN] " FILE_LINE_FORMAT Message "\n",        \
                __FILENAME__,                                   \
                __LINE__,                                       \
                __VA_ARGS__                                     \
                )

#   define _LOG_ERROR(dwError, Message, ...)                    \
            printf(                                             \
                "[ERROR] " FILE_LINE_FORMAT Message             \
                "\n\tErrorCode <0x%08x> \n",                    \
                __FILENAME__,                                   \
                __LINE__,                                       \
                __VA_ARGS__,                                    \
                dwError == NULL ? GetLastError() : dwError      \
                )
#else
#   define LOG_INFO(Message, ...)
#   define _LOG_WARN(Message, ...)
#   define _LOG_ERROR(dwError, Message, ...)
#endif



#define LOG_ERROR(dwErrorCode, Message, ...)                                                                \
            do{                                                                                             \
                HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);                                           \
                if (hStdout == INVALID_HANDLE_VALUE)                                                        \
                {                                                                                           \
                    break;                                                                                  \
                }                                                                                           \
                /* sets the color to intense red on blue background */                                      \
                SetConsoleTextAttribute(hStdout, COLOUR_ERROR);                                             \
                _LOG_ERROR(dwErrorCode, Message, __VA_ARGS__);                                              \
                /* reverting back to the normal color */                                                    \
                SetConsoleTextAttribute(hStdout, COLOUR_RESET);                                             \
            }while(0)

#define LOG_WARN(Message, ...)                                                                              \
            do{                                                                                             \
                HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);                                           \
                if (hStdout == INVALID_HANDLE_VALUE)                                                        \
                {                                                                                           \
                    break;                                                                                  \
                }                                                                                           \
                /* sets the color to intense red on blue background */                                      \
                SetConsoleTextAttribute(hStdout, COLOUR_WARNING);                                           \
                _LOG_WARN(Message, __VA_ARGS__);                                                            \
                /* reverting back to the normal color */                                                    \
                SetConsoleTextAttribute(hStdout, COLOUR_RESET);                                             \
            }while(0)


#endif  // _DBG_H_