#pragma once
//////////////////////////////////////////////////////////////////////////
// Universal typedefs
typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned char byte_t;

typedef signed char        int8;
typedef short              int16;
typedef int                int32;
typedef long long          int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned int       uint32;
typedef unsigned long long uint64;
//////////////////////////////////////////////////////////////////////////
// Universal Functions
template <typename T>
inline void MemZero(T *ptr)
{
	memset(ptr, 0, sizeof(T));
}

inline void Toggle(bool& value)
{
	value = !value;
}
//////////////////////////////////////////////////////////////////////////
// Universal Constants
float constexpr MAX_FLOAT = (3.402823466e+38f); // stolen from Forseth which he stole from D3D11
constexpr size_t M_KiB = 1024U;
constexpr size_t M_MiB = 1048576U;
constexpr size_t M_GiB = 1073741824U;
//////////////////////////////////////////////////////////////////////////
// Universal macros
#define	BIT(x)		(1 << (x))
#define UNUSED(x)	(void)(x)
#define SAFE_DELETE(ptr) if((ptr) != nullptr) {delete ptr; ptr = nullptr;}
#define SAFE_DELETE_OBV(ptr) if((*ptr) != nullptr) {delete *ptr; *ptr = nullptr;}

#define COMBINE1(X,Y) X##Y
#define COMBINE(X,Y) COMBINE1(X,Y)

#define KiB * M_KiB
#define MiB * M_MiB
#define GiB * M_GiB

#define FORCEINLINE __forceinline									/* Force code to be inline */
#define FORCENOINLINE __declspec(noinline)							/* Force code to NOT be inline */
//------------------------------------------------------------------------
// PRAGMA TODO 
// Idea from Chris Forseth's friend Micheal Bosley
// Source from http://www.flipcode.com/archives/FIXME_TODO_Notes_As_Warnings_In_Compiler_Output.shtml
#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)

#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// THE IMPORANT BITS
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
		" --------------------------------------------------------------------------------------\n" \
		"|  TODO :   " ##x "\n" \
		" --------------------------------------------------------------------------------------\n" )
//ENDSNIPPET//////////////////////////////////////////////////////////////