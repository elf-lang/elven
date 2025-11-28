
#define copy_memory(dst, src, size)    memcpy((dst), (src), (size))
#define fill_memory(dst, byte, size)    memset((dst), (byte), (size))
#define MemoryCompare(a, b, size)     memcmp((a), (b), (size))

#define MemoryCopyStruct(d,s)  copy_memory((d),(s),sizeof(*(d)))
#define MemoryCopyArray(d,s)   copy_memory((d),(s),sizeof(d))
#define MemoryCopyTyped(d,s,c) copy_memory((d),(s),sizeof(*(d))*(c))

#define clear_memory(s,z)       memset((s),0,(z))
#define MemoryZeroStruct(s)   clear_memory((s),sizeof(*(s)))
#define MemoryZeroArray(a)    clear_memory((a),sizeof(a))
#define MemoryZeroTyped(m,c)  clear_memory((m),sizeof(*(m))*(c))

#define memory_match(a,b,z)     (MemoryCompare((a),(b),(z)) == 0)
#define MemoryMatchStruct(a,b)  memory_match((a),(b),sizeof(*(a)))
#define MemoryMatchArray(a,b)   memory_match((a),(b),sizeof(a))


#define ArrayCount(a) (sizeof(a) / sizeof((a)[0]))


#define Min(A,B) (((A) < (B)) ? (A) : (B))
#define Max(A,B) (((A) > (B)) ? (A) : (B))


#define    internal static
#define      global static
#define thread_decl __declspec(thread)

//
// function tags:
//
// a function that's visible to other folders
#define APIFUNC

// a function that's visible only to the folder it resides within,
// for instance, a helper function in a file that's private to the
// folder or compilation unit, each folder has one compilation unit,
// each module is one compilation unit... it's the same in my head...
#define FOLDERFUNC static

// a function that's visible only to the file
#define FILEFUNC   static

// a function defined in a portable header file, a portable header
// files are included when needed and aren't compiled as part of
// a module.
#define PORTABLEFUNC static

// a function that's meant to be inlined
#define INLINEFUNC   inline



typedef   signed long long    int   i64;
typedef                       int   i32;
typedef   signed            short   i16;
typedef   signed             char    i8;


typedef unsigned long long    int   u64;
typedef unsigned              int   u32;
typedef unsigned            short   u16;
typedef unsigned             char    u8;

typedef                     float   f32;
typedef                    double   f64;

typedef                       i32   b32;


typedef u32 Rune;


typedef struct {
	f64 seconds;
} Seconds;

#define NANOSECONDS_PER_SECOND (1e9)
#define NANOSECONDS_TO_SECONDS (1.0 / NANOSECONDS_PER_SECOND)

#define U8_MAX  (~  (u8) 0)
#define U16_MAX (~ (u16) 0)
#define U32_MAX (~ (u32) 0)
#define U64_MAX (~ (u64) 0)

#define U64FromU32Packed(HI, LO) (((u64) (HI) << 32) | ((u64) (LO) & U32_MAX))



