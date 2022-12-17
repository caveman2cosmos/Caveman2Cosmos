#pragma once

#ifndef CV_MACROS_H
#define CV_MACROS_H


#define CONCAT(arg1, arg2)	         CONCAT_EXP_ARG(arg1, arg2)
#define CONCAT_EXP_ARG(arg1, arg2)   CONCAT_EXP_ARG2(arg1, arg2)
#define CONCAT_EXP_ARG2(arg1, arg2)  arg1##arg2
#define CONCAT3(arg1, arg2, arg3)        CONCAT(arg1, CONCAT(arg2, arg3))
#define CONCAT4(arg1, arg2, arg3, arg4)  CONCAT(arg1, CONCAT3(arg2, arg3, arg4))

#endif