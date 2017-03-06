/**
 * @file osal_string.h
 *   definitions for Pinpointer string functions
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _OSAL_STRING_H_
#define _OSAL_STRING_H_

#include "osal_type.h"
/******* definitions of main PinPointer string functions  ********/
#ifdef OSAL_STRING

/** OSAL_strlen returns the length of str */
OSAL_u32 OSAL_strlen ( const OSAL_s8 * str );

/** OSAL_strcmp returns an integer less than, equal to, or greater than zero if str1 is found, respectively, to be less than, to match, or be greater than str2.*/
OSAL_s32         OSAL_strcmp( const OSAL_s8 * str1, const OSAL_s8 * str2 );

/** OSAL_strncmp returns an integer less than, equal to, or greater than zero if at most num character of str1 is found, respectively, to be less than, to match, or be greater than at most num character of str2.*/
OSAL_s32         OSAL_strncmp( const OSAL_s8 * str1, const OSAL_s8 * str2, OSAL_u32 num );

/** OSAL_strchr returns a pointer to the first occurrence of the charater c in the string str*/
OSAL_s8*       OSAL_strchr( const OSAL_s8 * str, OSAL_u32 character );

/**  OSAL_strrchr function returns a pointer to the last occurrence of the character c in the string str*/
OSAL_s8*       OSAL_strrchr( const OSAL_s8 * str, OSAL_u32 character );

/**  OSAL_strstr function returns a pointer to the first first occurrence of the substring sub in the string str*/
OSAL_s8*       OSAL_strstr( const OSAL_s8 * str, const OSAL_s8 * sub );

/**  OSAL_strcasestr is the same than OSAL_strstr but ignore the case */
OSAL_s8*       OSAL_strcasestr(const OSAL_s8 *str, const OSAL_s8 *sub);

/**  OSAL_strcpy copies  the  string pointed to by src, including the terminating null byte, to the buffer pointed to by dest.*/
OSAL_s8*       OSAL_strcpy( OSAL_s8 * destination, const OSAL_s8 * source );

/**  OSAL_strncpy is similar to OSAL_strcpy but for at least num characters*/
OSAL_s8*       OSAL_strncpy( OSAL_s8 * destination, const OSAL_s8 * source, OSAL_u32 num );

/**  OSAL_strcat appends the src string to the dest string,*/
OSAL_s8*       OSAL_strcat( OSAL_s8 * destination, const OSAL_s8 * source );

/**  OSAL_strdup returns  a pointer to a new string which is a duplicate of the string s*/
OSAL_s8*       OSAL_strdup( const OSAL_s8 *s );

/**  OSAL_strndup is similar to OSAL_strdup, but only copies at most n characters*/
OSAL_s8*       OSAL_strndup( const OSAL_s8 *s, OSAL_u32 num );

/**  OSAL_strcasecmp is similar to OSAL_strcmp, but ignore the case */
OSAL_u32         OSAL_strcasecmp( const OSAL_s8 *str1, const OSAL_s8 *str2 );

/**  OSAL_strncasecmp is similar to OSAL_strncmp, but ignore the case*/
OSAL_u32         OSAL_strncasecmp( const OSAL_s8 *str1, const OSAL_s8 *str2, OSAL_u32 size );
#else
#warning osal_string.h included but OSAL_STRING not defined
#endif //defined OSAL_STRING
#endif
