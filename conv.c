/* Understanding C's integer promotion and arithmetic conversion rules
 * -------------------------------------------------------------------
 *
 *
 * Because many programmers are confused about the rules, we implement
 * simple macros that demonstrate the logic of integer promotion and
 * usual arithmetic converson rules in C.
 */

/* preliminaries 
 *
 * You do not have to understand this part. We define some macros
 * for meta-programming with types and this is much more complicated
 * then the rules we want to explain below.
 * */

#include <limits.h>
#include <stdbool.h>

// produce a value of the type
#define NIL(T) (T){ 0 }

#define MAX(a, b) ((a) > (b) ? (a) : (b))

// define the integer conversion rank
#define RANK(T)							\
	_Generic(T,						\
	bool:	1,						\
	char:   2, signed char: 2, unsigned char: 2,		\
	short:  3, unsigned short: 3,				\
	int:	4, unsigned int: 4,				\
	long:	5, unsigned long: 5,				\
	long long: 6, unsigned long long: 6)

// produce a signed type for the rank (we only need int and higher)
#define TYPE_SIGNED_FROM_RANK(R)				\
	typeof(*_Generic(&NIL(int[R]),				\
	int(*)[4]: &NIL(int),					\
	int(*)[5]: &NIL(long),					\
	int(*)[6]: &NIL(long long)))

// produce an unsigned type for the rank
#define TYPE_UNSIGNED_FROM_RANK(R)				\
	typeof(*_Generic(&NIL(int[R]),				\
	int(*)[4]: &NIL(unsigned int),				\
	int(*)[5]: &NIL(unsigned long),				\
	int(*)[6]: &NIL(unsigned long long)))

// return the maximum value of the type
#define TYPE_MAX(T)						\
	_Generic(T,						\
	bool: 1, 						\
	char: CHAR_MAX, signed char: SCHAR_MAX, unsigned char: UCHAR_MAX,	\
	short: SHRT_MAX, unsigned short: USHRT_MAX,		\
	int: INT_MAX, unsigned int: UINT_MAX,			\
	long: LONG_MAX, unsigned long: ULONG_MAX,		\
	long long: LLONG_MAX, unsigned long long: ULLONG_MAX)


// choose type A or type B depending on a constant
#define TYPE_CHOOSE(x, A, B)	\
	typeof(_Generic(&NIL(int[1 + !!(x)]), int(*)[2]: NIL(A), int(*)[1]: NIL(B)))

// check that the expression 'x' has type 'T'
#define TYPE_CHECK(T, x)    \
    _Static_assert(_Generic(x, T: 1, default: 0), "")




/* The logic of integer promotion 
 * ------------------------------
 *
 * The logic for integer promotion is very simple. For types with
 * rank smaller or equal the rank of 'int', we increase the rank to the
 * rank of 'int'.  Then, the resulting type is a signed type of this
 * rank if the type can repesent all values of the original type,
 * otherwise it is the corresponding unsigned type.
 *
 * Examples:
 * 
 * 'char' is promoted to 'int'
 * 'int' is promoted to 'int'
 * 'unsigned int' is promoted to 'unsigned int' because 'int' can
 * not represent all values of 'unsigned int'.
 * */
TYPE_CHECK(int, +(char)1);
TYPE_CHECK(int, +(int)1);
TYPE_CHECK(unsigned int, +(unsigned int)1);


/* Implement integer promotion rules in three simple macros
 */

// the rank after promotion, simple the max of the rank of A and the rank of an 'int'
#define PROMOTION_RANK(A)   MAX(RANK(A), RANK(NIL(int)))

// the sign after promotion, if we can represent all values we use signed, otherwise unsigned
#define PROMOTION_SIGN(A)   (TYPE_MAX(NIL(TYPE_SIGNED_FROM_RANK(PROMOTION_RANK(A)))) >= TYPE_MAX(A))

//  put it together
#define PROMOTE(A)    TYPE_CHOOSE(PROMOTION_SIGN(A), \
                            TYPE_SIGNED_FROM_RANK(PROMOTION_RANK(A)),    \
                            TYPE_UNSIGNED_FROM_RANK(PROMOTION_RANK(A)))


// Let's check...

// define some one letter variabes with different types for testing
bool b = 0;
char c = 0;
signed char sc = 0;
unsigned char uc = 0;
short s = 0;
unsigned short us = 0;
int i = 0;
unsigned int ui = 0;
long l = 0;
unsigned long ul = 0;
long long ll = 0;
unsigned long long u2 = 0;
struct { int x:32; } x;
struct { unsigned int x:32; } y;
enum { X = UINT_MAX };


// we now check that the type after promotion (as seen and returned by '+')
// is the same as what we compute

#define C(X)  TYPE_CHECK(PROMOTE(X), +X)
C(b); C(c); C(sc); C(uc); C(s); C(us); C(i); C(ui); C(l); C(ul); C(ll); C(u2); C(x.x); C(y.x); C(X);
#undef C

// If this compiles, the compiler uses the same simple rules for
// integer promotion as we implemented in our macro.




/* The logic of usual arithmetic conversion
 * ----------------------------------------
 *
 * The logic for usual arithmetic conversions is equally simple. The type
 * used for the operation has a rank which is the maximum rank of the
 * types of the arguments after integer promotion. Then, the same rule
 * decides whether we get a signed type or an unsigned type: If the
 * signed type can represent all values of the original types, it is
 * used. Otherwise, we use the unsigned type.
 *
 * Examples:
 * 
 * 'char' + 'int' is promoted to 'int' + 'int' and this yields 'int'
 * 'int' + 'unsigned int' yields 'unsigned int'
 * 'int' + 'long' is promoted to 'long'
 * */
TYPE_CHECK(int, (char)1 + (int)1);
TYPE_CHECK(unsigned int, (int)1 + (unsigned int)1);
TYPE_CHECK(long, (int)1 + (long)1);
/* 
 * An exceptional example is 'long long' which typically has the same
 * with as 'long' on 64 bit archs:
 */
TYPE_CHECK(unsigned long long, (long long)1 + (unsigned long)1);
/* 
 * So what is happening here? 'long long' has higher rank, so the
 * type must be 'long long' or 'unsigned long long'. Because
 * 'long long' has the same width as 'long' (despite the higher rank),
 * it can not represent all values of 'unsigned long', so the
 * unsigned version is used.
 */


/* Implement conversion rules rules in three simple macros
 */

// compute rank after integer promotion and usual arithmetic conversion
#define UAC_RANK(A, B)	MAX(MAX(RANK(A), RANK(B)), RANK(NIL(int)))

// signed type is used when it can represent all values of the original types
#define SGN_OK(A, B) (TYPE_MAX(NIL(TYPE_SIGNED_FROM_RANK(UAC_RANK(A, B)))) >= MAX(TYPE_MAX(A), TYPE_MAX(B)))

// type after usual arithmetic conversion
#define UAC(A, B) TYPE_CHOOSE(SGN_OK(A, B),                     \
                    TYPE_SIGNED_FROM_RANK(UAC_RANK(A, B)),      \
                    TYPE_UNSIGNED_FROM_RANK(UAC_RANK(A, B)))


// check all pairs
#define C(A, B) TYPE_CHECK(UAC(A, B), A + B)
/*  b */ C( b, b); C( b, c); C( b, sc); C( b, uc); C( b, s); C( b, us); C( b, i); C( b, ui); C( b, l); C( b, ul); C( b, ll); C( b, u2); C( b, X); C( b, x.x); C( b, y.x);
/*  c */ C( c, b); C( c, c); C( c, sc); C( c, uc); C( c, s); C( c, us); C( c, i); C( c, ui); C( c, l); C( c, ul); C( c, ll); C( c, u2); C( c, X); C( c, x.x); C( c, y.x);
/* sc */ C(sc, b); C(sc, c); C(sc, sc); C(sc, uc); C(sc, s); C(sc, us); C(sc, i); C(sc, ui); C(sc, l); C(sc, ul); C(sc, ll); C(sc, u2); C(sc, X); C(sc, x.x); C(sc, y.x);
/* uc */ C(uc, b); C(uc, c); C(uc, sc); C(uc, uc); C(uc, s); C(uc, us); C(uc, i); C(uc, ui); C(uc, l); C(uc, ul); C(uc, ll); C(uc, u2); C(uc, X); C(uc, x.x); C(uc, y.x);
/*  s */ C( s, b); C( s, c); C( s, sc); C( s, uc); C( s, s); C( s, us); C( s, i); C( s, ui); C( s, l); C( s, ul); C( s, ll); C( s, u2); C( s, X); C( s, x.x); C( s, y.x);
/* us */ C(us, b); C(us, c); C(us, sc); C(us, uc); C(us, s); C(us, us); C(us, i); C(us, ui); C(us, l); C(us, ul); C(us, ll); C(us, u2); C(us, X); C(us, x.x); C(us, y.x);
/*  i */ C( i, b); C( i, c); C( i, sc); C( i, uc); C( i, s); C( i, us); C( i, i); C( i, ui); C( i, l); C( i, ul); C( i, ll); C( i, u2); C( i, X); C( i, x.x); C( i, y.x);
/* ui */ C(ui, b); C(ui, c); C(ui, sc); C(ui, uc); C(ui, s); C(ui, us); C(ui, i); C(ui, ui); C(ui, l); C(ui, ul); C(ui, ll); C(ui, u2); C(ui, X); C(ui, x.x); C(ui, y.x);
/*  l */ C( l, b); C( l, c); C( l, sc); C( l, uc); C( l, s); C( l, us); C( l, i); C( l, ui); C( l, l); C( l, ul); C( l, ll); C( l, u2); C( l, X); C( l, x.x); C( l, y.x);
/* ul */ C(ul, b); C(ul, c); C(ul, sc); C(ul, uc); C(ul, s); C(ul, us); C(ul, i); C(ul, ui); C(ul, l); C(ul, ul); C(ul, ll); C(ul, u2); C(ul, X); C(ul, x.x); C(ul, y.x);
/* ll */ C(ll, b); C(ll, c); C(ll, sc); C(ll, uc); C(ll, s); C(ll, us); C(ll, i); C(ll, ui); C(ll, l); C(ll, ul); C(ll, ll); C(ll, u2); C(ll, X); C(ll, x.x); C(ll, y.x);
/* u2 */ C(u2, b); C(u2, c); C(u2, sc); C(u2, uc); C(u2, s); C(u2, us); C(u2, i); C(u2, ui); C(u2, l); C(u2, ul); C(u2, ll); C(u2, u2); C(u2, X); C(u2, x.x); C(u2, y.x);
/*  X */ C( X, b); C( X, c); C( X, sc); C( X, uc); C( X, s); C( X, us); C( X, i); C( X, ui); C( X, l); C( X, ul); C( X, ll); C( X, u2); C( X, X); C( X, x.x); C( X, y.x);
/*  x */ C(x.x,b); C(x.x, c); C(x.x, sc); C(x.x, uc); C(x.x, s); C(x.x, us); C(x.x, i); C(x.x, ui); C(x.x, l); C(x.x, ul); C(x.x, ll); C(x.x, u2); C(x.x, X); C(x.x, x.x); C(x.x, y.x);
/*  y */ C(y.x,b); C(y.x, c); C(y.x, sc); C(y.x, uc); C(y.x, s); C(y.x, us); C(y.x, i); C(y.x, ui); C(y.x, l); C(y.x, ul); C(y.x, ll); C(y.x, u2); C(y.x, X); C(y.x, x.x); C(y.x, y.x);
#undef C


