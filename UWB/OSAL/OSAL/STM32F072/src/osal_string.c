/**
 * @file osal_string.c
 * @brief implementation for Pinpointer string functions
 * @author bgi@bespoon.com
 * @date 4/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
// */


#include "../../../include/osal_string.h"
#include <string.h>

#ifdef TODO
OSAL_u32       OSAL_strlen( const OSAL_s8 * str ) { return strlen(str); }

OSAL_s32         OSAL_strcmp( const OSAL_s8 * str1, const OSAL_s8 * str2 ){ return strcmp(str1, str2); }

OSAL_s32         OSAL_strncmp( const OSAL_s8 * str1, const OSAL_s8 * str2, OSAL_u32 num ){return strncmp(str1, str2,num); }

OSAL_s8*       OSAL_strchr( const OSAL_s8 * str, OSAL_u32 character ){ return strchr(str, character); }

OSAL_s8*       OSAL_strrchr( const OSAL_s8 * str, OSAL_u32 character ){ return strrchr(str, character);}

OSAL_s8*       OSAL_strstr( const OSAL_s8 * str, const OSAL_s8 * sub ){ return strstr(str,sub);}

OSAL_s8*       OSAL_strcasestr(const OSAL_s8 *str, const OSAL_s8 *sub){ return strcasestr(str, sub);}

OSAL_s8*       OSAL_strcpy( OSAL_s8 * destination, const OSAL_s8 * source ){ return strcpy(destination, source);}

OSAL_s8*       OSAL_strncpy( OSAL_s8 * destination, const OSAL_s8 * source, OSAL_u32 num ){ return strncpy(destination, source, num);}

OSAL_s8*       OSAL_strcat( OSAL_s8 * destination, const OSAL_s8 * source ){return strcat(destination, source);}

OSAL_s8*       OSAL_strdup( const OSAL_s8 *s ){ return strdup(s);}

OSAL_s8*       OSAL_strndup( const OSAL_s8 *s, OSAL_u32 num ){ return strndup(s, num);}

OSAL_u32         OSAL_strcasecmp( const OSAL_s8 *str1, const OSAL_s8 *str2 ){ return 0;} // strcasecmp(str1, str2);}

OSAL_u32         OSAL_strncasecmp( const OSAL_s8 *str1, const OSAL_s8 *str2, OSAL_u32 size ){return 0;} // strncasecmp(str1,str2,size);}

#endif

