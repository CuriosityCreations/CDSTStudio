/**
 * @file shell.c
 * @brief shell.c
 * @author  bespoon.com
 * @date 30/1/2015
*/

/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
*/


#define _SHELL_COMMAND_C_
#pragma GCC optimize("Os")

#ifdef CONFIG_ACTIVATE_UART_SHELL
#include "osal_type.h"
#include "osal_time.h"
#include "osal_trace.h"
#include "osal_math.h"
#include <stdlib.h>				//malloc
#include <string.h>				//OSAL_memcpy and memset
#include <stdarg.h>
#include <types.h>
#include <config_api.h>
#include "USART.h"
#include "dac.h"
#include "RCC.h"
#include "SPI.h"
#include "TIMER.h"
#include "ADC.h"
#include "GPIO.h"
#include "Ext_Com.h"
#include "module_api.h"
#include "ProdSectionDef.h"

#include "shell.h"
#include "ShellCommand_system.h"
#include "ShellCommand_radio.h"
#include "ShellCommand_misc.h"
#ifdef PROD
#include "ShellCommand_prod.h"
#endif

#define  SHEL_PROMPT ">"
//#define  SHEL_PROMPT "$"
//#define  SHEL_PROMPT "#"

//-----------------------------------------------------------------------------
// Functions
//-----------------------------------------------------------------------------
#ifdef UWB_SPI_TEST_API
	#define MAX_IN_CHAR_LEN 122
#else
	#define MAX_IN_CHAR_LEN 40
#endif

static char rx_buffer[MAX_IN_CHAR_LEN]={0};// Local holding buffer to build line
static short rx_index = 0;

#define KEY_ETX	 0x03
#define KEY_BCKSPC  0x08
#define KEY_TAB	 0x09
#define KEY_CR	  0xD
#define KEY_SPACE   0x20
#define KEY_DELETE  0x7F


#ifndef PROD
   #define CM_BLOCK_SIZE 3
#else
   #define CM_BLOCK_SIZE 4
#endif
#define CM_SIZE (CM_BLOCK_SIZE*2)

#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
static ui_cmd_t* cm[CM_SIZE]={
	(ui_cmd_t*) cmd_array_system,(ui_cmd_t*) cmd_array_system_full
	,(ui_cmd_t*) cmd_array_radio,(ui_cmd_t*) cmd_array_radio_full
	,(ui_cmd_t*) cmd_array_misc,(ui_cmd_t*) cmd_array_misc_full
	#ifdef PROD
	,(ui_cmd_t*) cmd_array_prod,NULL
	#endif
			
};
static int cms[CM_SIZE]={
	OSAL_ARRAY_SIZE(cmd_array_system),OSAL_ARRAY_SIZE(cmd_array_system_full)
	,OSAL_ARRAY_SIZE(cmd_array_radio),OSAL_ARRAY_SIZE(cmd_array_radio_full)
	,OSAL_ARRAY_SIZE(cmd_array_misc),OSAL_ARRAY_SIZE(cmd_array_misc_full)
	#ifdef PROD
	,OSAL_ARRAY_SIZE(cmd_array_prod),0
	#endif
};
#else
static ui_cmd_t* cm[CM_SIZE]={
	(ui_cmd_t*) cmd_array_system,NULL
	,(ui_cmd_t*) cmd_array_radio,NULL
	,(ui_cmd_t*) cmd_array_misc,NULL
	#ifdef PROD
	,(ui_cmd_t*) cmd_array_prod,NULL
	#endif
};
static int cms[CM_SIZE]={
	OSAL_ARRAY_SIZE(cmd_array_system),0
	,OSAL_ARRAY_SIZE(cmd_array_radio),0
	,OSAL_ARRAY_SIZE(cmd_array_misc),0
	#ifdef PROD
	,OSAL_ARRAY_SIZE(cmd_array_prod),0
	#endif
};
#endif

static int analyzeChar(char val)
{
	if((val == KEY_TAB)&& (rx_index >0 )) //TAB
	{
		//completion
		int i=0,j=0,k=0;
		int index=0;
		int matchnb=0;
		for(k=0;k<CM_BLOCK_SIZE;k++)
		{ 
			for(j=0;j<2;j++)
			{
				index=2*k+j;
				for(i=0;i<cms[index];i++)
				{
					if(
						(cm[index][i].name != NULL)&&
						(cm[index][i].fct != NULL)&&
						(!strncmp(cm[index][i].name,rx_buffer,strlen(rx_buffer)))
					)
						matchnb++;
				}
			}
		}
		i=0;
		if(matchnb>0) printline("");
		for(k=0;k<CM_BLOCK_SIZE;k++)
		{ 
			for(j=0;j<2;j++)
			{
				index=2*k+j;
				for(i=0;i<cms[index];i++)
				{
					if(
						(cm[index][i].name != NULL)&&
						(cm[index][i].fct != NULL)&&
						(!strncmp(cm[index][i].name,rx_buffer,strlen(rx_buffer)))
					)
					{
						if(matchnb==1)
						{
							rx_index=strlen(cm[index][i].name);
							OSAL_memcpy(rx_buffer,cm[index][i].name,rx_index);
							shell_prompt();
							print("%s",rx_buffer);
							return 0;
						}
						printline("%s",cm[index][i].name);
					}
				}
			}
		}
		shell_prompt();
		print("%s",rx_buffer);
		return 0;
	}
	if((val == KEY_BCKSPC)||(val == KEY_DELETE)) //delete
	{
	  if(rx_index==0) return 0;
	  rx_index--;
	  rx_buffer[rx_index]=' ';
	  print("\r>%s",rx_buffer);
	  rx_buffer[rx_index]=0;
	  print("\r>%s",rx_buffer);
	  return 0;
	}	
	if(val == KEY_ETX)
	{
		rx_index=0;
		OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
		shell_prompt();  
		return 0; 
	}
	if((val == KEY_SPACE) && (rx_index ==0))
	{
		//do nothing, command may be misunderstood
		return 0;
	}
	PrintChar(val);//ECHO the character	

	if ((val == KEY_CR) || (val == '\r') || (val == '\n')) // Is this an end-of-line condition, either will suffice?
	{
		if (rx_index != 0) // Line has some content?
		{
			rx_buffer[rx_index++] = '\0';
			rx_index = 0; // Reset content pointer
			if(rx_buffer[0] == '\0') //nothing!
			{
				OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
				return 0;  
			}
			return 1;
		}
		else
		{
			rx_index=0;
			OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
			shell_prompt();
		}
	}
	else
	{
		rx_buffer[rx_index++] = val; // Copy to buffer and increment
		rx_buffer[rx_index] = '\0';
		if (rx_index >= MAX_IN_CHAR_LEN) // If overflows consider a CTRL+C, and warn user
		{
			printline("MAX_IN_CHAR_LEN too small");
			rx_index=0;
			OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
			shell_prompt();
		}
	}
	return 0;
}

/** Maximum string size allowed for sen host answer */
#ifdef PROD
#define MAX_STRING_SIZE	100
char send_host(char * cmd_name, int err_c, int arg_nb, ...)
{
	//save ram by using the stack if this is only in this func
	  char cmd_answ[MAX_STRING_SIZE]={0};
	int leftChar=MAX_STRING_SIZE;
	char ret = 0;
	unsigned char CS=0x0;
	  va_list argptr;
	va_start(argptr, arg_nb);
	  
	snprintf(cmd_answ,leftChar," %s %d ",cmd_name,err_c);
	leftChar =MAX_STRING_SIZE-strlen(cmd_answ);
	while(arg_nb--)
	{
		  snprintf(&cmd_answ[strlen(cmd_answ)],leftChar,"%d ",(int) va_arg(argptr, long));
		leftChar =MAX_STRING_SIZE-strlen(cmd_answ);
	}	
	GET_SIMPLE_U8CHECKSUM(cmd_answ,strlen(cmd_answ),CS);
	  CS = ~CS +1;
	snprintf(&cmd_answ[strlen(cmd_answ)],leftChar,"%02X ",CS);
	printf("\n[%s]\n\r",cmd_answ);
	va_end(argptr);
	
	return ret;
}
#endif

int shell_interface(void)
{
	char curChar = 0;
	if(!(USE_SHELL_USART_NB&0xF0)) return 0;
	if(HAL_USART_GetChar(&curChar)==0)
	{
		char* tst=NULL;
		signed char status=0;
		char* p=NULL;
		signed char (*fct)(int,char**) = NULL;
		char** argv=NULL;
		int argc=0;
		int i,j,k;
		int index;
		if(analyzeChar(curChar)>0)
		{
			//get comamnd and arguments
			printline("");
			argc=0;
			tst = rx_buffer;
			while(*tst==' ') tst++;
			if(*tst == '\0') // no args
			{
				rx_index=0;
				OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
				shell_prompt();
				return 1;
			}		 
			p = strchr(tst,' ');
			while (p != NULL)
			{
				argc++;
				tst=p+1;			  
				while(*tst==' ') tst++;
				p = strchr(tst,' ');
			}
			argc++;
			if(argc==0) // no args
			{
				rx_index=0;
				OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
				shell_prompt();
				return 1;
			}
			argv = malloc(argc*sizeof(char*));
			argc=0;
			tst = rx_buffer;
			while(*tst==' ') tst++;
			p = strchr(tst,' ');
			while (p != NULL)
			{
				*p='\0';
				argv[argc++] = tst;
				tst=p+1;
				while(*tst==' ') tst++;
				p = strchr(tst,' ');
			}
			argv[argc++] = tst;
			for(k=0;k<CM_BLOCK_SIZE;k++)
			{
				fct=NULL;
				for(j=0;j<2;j++)
				{
					index=2*k+j;
					for(i=0;i<cms[index];i++)
					{
						if(
							(cm[index][i].name != NULL)&&
							(cm[index][i].fct != NULL)
						)
						{
								if(!strcmp(argv[0],cm[index][i].name))
								{
									fct = cm[index][i].fct;
									goto found_function;
								}
						}
					}
				}
			}
		found_function:
			if(fct == NULL)
				printline("%s undef", argv[0]);
			else
			{
				status = fct(argc,argv);
				if(status != 0)
				{
					printline("%s fail", argv[0]);
					send_host(argv[0], -2, 1, (signed long)status);
				}
			}
			rx_index=0;
			OSAL_memset(rx_buffer,0,MAX_IN_CHAR_LEN);
			free(argv);
			argv=NULL;
			shell_prompt();
		}
		return 1;//something happen anyway on uart if happen >=0
	}	
	return 0;
}

static void shell_trace_line()
{
	char k=16;
	while(--k) print("_____");
	printline("");
}

void shell_help(OSAL_bool_t isFull)
{
	unsigned char i=0,j=0,k=0,stepInc=2;
	unsigned char index=0;
	shell_trace_line();
	#ifdef CONFIG_ACTIVATE_ADVANCEDUART_SHELL
	if(isFull==OSAL_true) stepInc=1;
	#endif
	shell_trace_line();
	for(k=0;k<CM_BLOCK_SIZE;k++)
	{
		for(j=0;j<2;j+=stepInc)
		{
			index=2*k+j;
			for(i=0;i<cms[index];i++)
			{
				if(
					(cm[index][i].name != NULL)&&
					(cm[index][i].fct != NULL)
				)
				{
					//you can put NULL=> it will show GET <funcname> in help
					//you can put 0xFFFFFFFF=> it will show DO <funcname> in help
					//you can put 0xFFFFFFFE=> it will show TEST <funcname> in help
					char aline[SHELL_HELP_MAX_LINE_STRLEN];
					char* strn=NULL,*p=(char*) cm[index][i].usage;
					if(p==ITSA_GET_CMD)
						printline("<%s>:%s\tGET %s",cm[index][i].name,(strlen(cm[index][i].name)<5)?"\t":"",cm[index][i].name);
					else if(p==ITSA_DO_CMD)
						printline("<%s>:%s\tDO %s",cm[index][i].name,(strlen(cm[index][i].name)<5)?"\t":"",cm[index][i].name);
					else if(p==ITSA_TEST_CMD)
						printline("<%s>:%s\tTEST %s",cm[index][i].name,(strlen(cm[index][i].name)<5)?"\t":"",cm[index][i].name);
					else
					{
						strn=strchr(p,'\n');
						if(strn==NULL)
							printline("<%s>:%s\t%s",cm[index][i].name,(strlen(cm[index][i].name)<5)?"\t":"",p);
						else
						{
							strncpy(aline,p,SHELL_HELP_MAX_LINE_STRLEN);
							p=strn;
							p++;
							strn=strchr(aline,'\n');
							*strn='\0';
							printline("<%s>:%s\t%s",cm[index][i].name,(strlen(cm[index][i].name)<5)?"\t":"",aline);
							strncpy(aline,p,SHELL_HELP_MAX_LINE_STRLEN);
							strn=strchr(aline,'\n');
							while(strn)
							{
								*strn='\0';
								printline("\t\t%s",aline);
								strn=strchr(p,'\n');
								strn++;
								p=strn;
								strncpy(aline,p,SHELL_HELP_MAX_LINE_STRLEN);
								strn=strchr(aline,'\n');
							}
							printline("\t\t%s",p);
						}
					}
				}
			}
		}
	}
	shell_trace_line();
}

void shell_prompt(void)
{
	if(!(USE_SHELL_USART_NB&0xF0)) return;
	print(PRINT_ENDOFLINE SHEL_PROMPT);
}

void shell_process()
{
	shellcommand_misc_Process();
	#ifdef PROD
	shellcommand_prod_Process();
	#endif
	
}

#endif //#ifdef CONFIG_ACTIVATE_UART_SHELL

