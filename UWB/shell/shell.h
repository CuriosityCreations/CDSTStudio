/**
 * @file shell.h
 * @brief shell.h
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/
#define SHELL_HELP_MAX_LINE_STRLEN 70
typedef struct ui_cmd {
  	const char* name;
	signed char (*fct)(int argv, char** argc);
	const char* usage;
} ui_cmd_t;

#define ITSA_GET_CMD NULL
#define ITSA_DO_CMD (const char*) 0xFFFFFFFF
#define ITSA_TEST_CMD (const char*) 0xFFFFFFFE

#ifndef _SHELL_H_
#define _SHELL_H_

/** Maximum string size allowed for sen host answer */
#ifdef PROD
char send_host(char * cmd_name, int err_c, int arg_nb, ...);
#else
#define send_host(args...) do{}while(0)
#endif
int shell_interface(void);
void shell_help(OSAL_bool_t isFull);
void shell_prompt(void);
void shell_process(void);
#endif // _SHELL_H_
//-----------------------------------------------------------------------------
// End Of File
//-----------------------------------------------------------------------------
