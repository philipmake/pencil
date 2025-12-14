#ifndef CHAR_H_
#define CHAR_H_

#ifndef EOF
#define EOF (-1)
#endif

#define _U 0x01     /* uppercase letters [A-Z] */
#define _L 0x02     /* lowercase letters [a-z] */
#define _N 0x04     /* digits [0-9] */
#define _S 0x08     /* witespace symbols \t \r \f \n etc */
#define _P 0x10     /* punctuation characters */
#define _C 0x20     /* control characters */
#define _X 0x40     /* hex digits */


int _ctype[128] = {
    /*0-31, control characters */
    0, 
    _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, 
    _C, _C, _C, _C, _C, _C, _C, _C, _C, _C, 
    _C, _C, _C, _C, _C, _C, _C, _C, _C, _C,
    /*32-47, space and symbols*/
    _S, _S, _S, _S, _S, _S, _S, _S, _S, _S,
    _S, _S, _S, _S, _S,
    /*48-57, digits(0-9)*/
    _N, _N, _N, _N, _N, _N, _N, _N, _N, _N,
    /*58-64, symbols*/
    _P, _P, _P, _P, _P, _P, _P, 
    /*65-90, _Uppercase(A-Z)*/
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,
    _U,_U,_U,_U,_U,_U,_U,_U,_U,_U,_U,
    _U,_U,_U,_U,
    /*91-96, Symbols*/
    0,0,0,0,0,0,
    /*97-122, Lowercase(a-z)*/
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,
    _L,_L,_L,_L,_L,_L,_L,_L,_L,_L,_L,
    _L,_L,_L,_L,
    /*123-127, Symbols*/
    0,0,0,0,0
};

/* 
function prototypes
All of these functions take a character pointer as arguments and returns true(1) or false(0);
*/
int isalnum(int c);
int isalpha(int c);
int isascii(int c);
int iscntrl(int c);
int isdigit(int c);
int isxdigit(int c);
int isspace(int c);
int ispunct(int c);
int isprint(int c);
int isgraph(int c);
int islower(int c);
int isupper(int c);
int isblank(int c);

int toascii(int c);
int tolower(int c);
int toupper(int c);

/* Macros for identifying characters */
#define isalnum(c)  ((_ctype)[c]&(_U|_L|_N))
#define isalpha(c)  ((_ctype)[c]&(_U|_L))
#define iscntrl(c)  ((_ctype)[c]&(_C))
#define isdigit(c)  ((_ctype)[c]&(_N))
#define isxdigit(c)  ((_ctype)[c]&(_X|_N))
#define isspace(c)  ((_ctype)[c]&(_S))
#define isgraph(c)  ((_ctype)[c]&(_P|_U|L))
#define isupper(c)  ((_ctype)[c]&(_U))
#define islower(c)  ((_ctype)[c]&(_L))
#define isblank(c)  ((_ctype)[c]&!(isgraph(c)))

#define isprint(c)  ((c > 31) & (c < 128)) 
#define isascii(c)  ((unsigned) (c) < 128)

#define tolower(c)  ((unsigned) (c) - 32)
#define toupper(c)  ((unsigned) (c) + 32)
#define toascii(c)  ((unsigned) (c) < 128)

#define isoperator(c) ((c) == '+' || (c) == '-' || (c) == '*' || (c) == '%' || (c) == '/' || (c) == '>' || (c) == '<' || (c) == '=' || (c) == '&' || (c) == '|' || (c) == '!' || (c) == '?')

#define ispunct(c)  ((_ctype)[c]&(_P))
#define parenthesis(c) 
#define lparenthesis(c)
#define rparenthesis(c)

#define curlybraces(c)
#define rcurlybraces(c)
#define lcurlybraces(c)

#endif


