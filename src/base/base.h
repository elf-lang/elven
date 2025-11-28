#ifndef BASE_H
#define BASE_H

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <setjmp.h>
#include <math.h>
#include <stdarg.h>
#include <assert.h>


#define STATIC_ASSERT(x) _Static_assert(x, "")
#define COUNTOF _countof
#define ASSERT    assert

#define ISPOW2(x) ((x) != 0 && !((x) & ((x) - 1)))

#define MASK_8  U8_MAX
#define MASK_16 U16_MAX

#define GB_MASK ((1 << 30) - 1)
#define MB_MASK ((1 << 20) - 1)
#define KB_MASK ((1 << 10) - 1)

#define GIGABYTES(A) ((u64)(A) << 30LLU)
#define MEGABYTES(A) ((u64)(A) << 20LLU)
#define KILOBYTES(A) ((u64)(A) << 10LLU)

#define KiB KILOBYTES
#define MB  MEGABYTES

#define MIN(A,B) (((A) < (B)) ? (A) : (B))
#define MAX(A,B) (((A) > (B)) ? (A) : (B))
#define ABS(A) (((A) < 0) ? -(A) : (A))
#define CLAMP(X, A, B) MIN(MAX(X, A), B)

#define CONCAT_(A,B) A##B
#define CONCAT(A,B) CONCAT_(A,B)



enum { true = 1, false = 0 };

enum {
	CORNER_TOP_L = 0,
	CORNER_BOT_L = 1,
	CORNER_TOP_R = 2,
	CORNER_BOT_R = 3,
};

// MUST BE THIS ORDER
typedef enum {
	AXIS_X = 0, AXIS_Y = 1
} AXIS;

#include "base_types.h"
#include "base_math.h"
#include "base_rect.h"
#include "base_hash.h"
#include "base_stack_alloc.h"
#include "base_string.h"
#include "base_thread_context.h"
#include "base_platform.h"
#include "base_format.h"
#include "base_logger.h"
#include "base_list.h"
#include "base_requires.h"
#include "base_profiler.h"
#include "base_interner.h"
#include "base_image.h"
#include "base_times.h"
#include "base_color.h"

#endif