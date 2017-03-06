/**
 * @file prodSectionTestsDef.h
 * @brief definitions of tests done through bs_prod_module
 * @author ore@bespoon.com
 * @date 25/07/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _PRODSECTIONTESTDEF_H_
#define _PRODSECTIONTESTDEF_H_

#include <osal_type.h>
typedef enum{
    FAIL=-1,
    PASS=0,
    SKIP=1
}OSAL_PACKED_ENUM testRes;

/** < This enum serve for test definition, but serve for bitfield filling as well.
 * negative enum are undone test
 * WARNING: we re already on limit !!! 32 enums*/
typedef enum{
    TEST_INTERMEDIATE_FLASH=-2,
    TEST_MAC=-1,
    TEST_ID_NB=0,
    TEST_SOLDERING,
    TEST_V1,
    TEST_V2,
    TEST_V3,
    TEST_TX_TUNE,
    TEST_LNA,
    TEST_THLD,
    TEST_SENSITIVITY,
    TEST_RANGING,
    TEST_TX_IDLE,
    TEST_TX_CONT,
    TEST_RX_IDLE,
    TEST_IO,
    TEST_PM2,
    TEST_ID_NB_DONE,
    TEST_SOLDERING_DONE,
    TEST_V1_DONE,
    TEST_V2_DONE,
    TEST_V3_DONE,
    TEST_TX_TUNE_DONE,
    TEST_LNA_DONE,
    TEST_THLD_DONE,
    TEST_SENSITIVITY_DONE,
    TEST_RANGING_DONE,
    TEST_TX_IDLE_DONE,
    TEST_TX_CONT_DONE,
    TEST_RX_IDLE_DONE,
    TEST_IO_DONE,
    TEST_PM2_DONE, //29 IDS PASS IN 32 BITS
    TEST_END   // batch test is end. ( with of without error(s) )
}OSAL_PACKED_ENUM test_id;

#define get_TestRslt(_int32,_id,_rslt) do{_rslt = ((_int32)>>(_id))&0x1;}while(0)

#define reset_TestRslt(_int32) do{_int32=0;}while(0)

#define setbit_TestRslt(_int32,_id) do{_int32|=(1<<(_id));}while(0)

#define resetbit_TestRslt(_int32,_id) do{_int32&=~(1<<(_id));}while(0)

#define set_TestRslt(_int32,_id,_rslt) do{\
        if((_rslt)==FAIL)\
            resetbit_TestRslt(_int32,_id);\
        else \
            setbit_TestRslt(_int32,_id);\
    }while(0)

#define setResultInCsv(_test) ((_test == SKIP)?"SKIPPED;":(_test == PASS)?"PASS;":"FAIL;")

#define setResultValInCsv(_test,_val) (((_test) != SKIP)?(_val):"N.C.")

#define setBitFieldRslt(_btfield,_cond,_tid, _tiddone,_testRes) do{\
        if(_cond==1){\
            set_TestRslt(_btfield,_tid,_testRes);\
            set_TestRslt(_btfield,_tiddone,1);\
        }\
        else if(_cond>1){\
            set_TestRslt(_btfield,_tid,1);\
            set_TestRslt(_btfield,_tiddone,1);\
        }\
    }while(0)

#define SERIAL_NB_DIGIT 12

#endif // _PRODSECTIONTESTDEF_H_
