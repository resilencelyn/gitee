/*
 * =====================================================================================
 *
 *       Filename:  i_debug.h
 *
 *    Description:  Debug macros.
 *
 *        Version:  1.0
 *        Created:  18/04/08 08:19:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Aitor Viana Sanchez (avs), aitor.viana.sanchez@esa.int
 *        Company:  European Space Agency (ESA-ESTEC)
 *
 * =====================================================================================
 */

#ifndef _SPARC_DEBUG_H_
#define _SPARC_DEBUG_H_

/* 
 * LL: log level
 * 0: serous error
 * 1: normal error
 * 2: important debug 
 * 3: normal debug
 * 4: all info
 */
#define LL	2
#define SPARC_LOG(level, fmt, args...)		\
	if(level <= LL)				\
		printf("(%s:%d)" fmt, __FILE__, __LINE__, ##args);

#define SPARC_DEBUG 0
#if SPARC_DEBUG
#include <stdio.h>

#define DBG(msg...) fprintf(stderr, ##msg)
#else
#define DBG(msg...)
#endif

#define ERR(fmt,args...) fprintf(stderr, "%s:%d: " fmt "\n", __FILE__, __LINE__, ##args);

#endif
