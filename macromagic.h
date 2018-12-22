//http://p99.gforge.inria.fr/p99-html/p99__for_8h_source.html
//https://github.com/pfultz2/Cloak/wiki/C-Preprocessor-tricks,-tips,-and-idioms
//http://pfultz2.com/blog/2012/05/10/turing/
//base 0/1
#define CAT(a, ...) PRIMITIVE_CAT(a, __VA_ARGS__)
#define PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
//IIF for 0/1
#define IIF(c) PRIMITIVE_CAT(IIF_, c)
#define IIF_0(t, ...) __VA_ARGS__
#define IIF_1(t, ...) t
//lib
#define CHECK_N(x, n, ...) n
#define CHECK(...) CHECK_N(__VA_ARGS__, 0,)
#define PROBE(x) x, 1,
#define IS_PAREN(x) CHECK(IS_PAREN_PROBE x)
#define IS_PAREN_PROBE(...) PROBE(~)
#define NOT(x) CHECK(PRIMITIVE_CAT(NOT_, x))
#define NOT_0 PROBE(~)
#define COMPL(b) PRIMITIVE_CAT(COMPL_, b)
#define COMPL_0 1
#define COMPL_1 0
#define BOOL(x) COMPL(NOT(x))
#define IF(c) IIF(BOOL(c))
#define EAT(...)
#define EXPAND(...) __VA_ARGS__
#define WHEN(c) IF(c)(EXPAND, EAT)
#define EMPTY()
#define DEFER(id) id EMPTY()
#define OBSTRUCT(...) __VA_ARGS__ DEFER(EMPTY)()

#define EVAL(...)  EVAL1(EVAL1(EVAL1(__VA_ARGS__)))
#define EVAL1(...) EVAL2(EVAL2(EVAL2(__VA_ARGS__)))
#define EVAL2(...) EVAL3(EVAL3(EVAL3(__VA_ARGS__)))
#define EVAL3(...) EVAL4(EVAL4(EVAL4(__VA_ARGS__)))
#define EVAL4(...) EVAL5(EVAL5(EVAL5(__VA_ARGS__)))
#define EVAL5(...) __VA_ARGS__

//http://www.cplusplus.com/forum/lounge/108294/
#define WHILE(macro, value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value) \
  OBSTRUCT(WHILE_INDIRECT) () (macro, __VA_ARGS__) \
)
#define WHILE_INDIRECT() WHILE 

#define WHILE1(macro, value1,value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value1,value) \
  OBSTRUCT(WHILE_INDIRECT1) () (macro, value1,__VA_ARGS__) \
)
#define WHILE_INDIRECT1() WHILE1

#define WHILE2(macro, value1,value2,value, ...) \
	WHEN(NOT(IS_PAREN(value ()))) \
( \
  OBSTRUCT(macro) (value1,value2,value) \
  OBSTRUCT(WHILE_INDIRECT2) () (macro, value1,value2,__VA_ARGS__) \
)
#define WHILE_INDIRECT2() WHILE2

//consume macro q
#define ITR(mmm,qqq,...) EVAL( WHILE( mmm,qqq,__VA_ARGS__ ) )
#define ITR1(mmm,mm1,qqq,...) EVAL( WHILE1( mmm,mm1,qqq,__VA_ARGS__) )
#define ITR2(mmm,mm1,mm2,qqq,...) EVAL( WHILE2( mmm,mm1,mm2,qqq, __VA_ARGS__ ) )

// QUOTEME(sth) => "sth"
#define QUOTEME(sth) #sth

// warp XXXX as {XXXX}
#define BRACKET_WRAP(...) {__VA_ARGS__}

//TMP SOLUTION FOR MCAT() // Variable Argument Macro (VA_MACRO) upto 9 arguments for almost 99% cases MCAT/MULTICAT
#define NUM_ARGS_(_1, _2, _3, _4, _5, _6, _7, _8, _9, TOTAL, ...) TOTAL
#define NUM_ARGS(...) NUM_ARGS_(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1)
#define MCAT(...) CAT(MCAT, NUM_ARGS(__VA_ARGS__))(__VA_ARGS__)
#define MCAT1(_1) _1
#define MCAT2(_1, _2) CAT(MCAT1(_1),_2)
#define MCAT3(_1, _2, _3) CAT(MCAT2(_1,_2),_3)
#define MCAT4(_1, _2, _3, _4) CAT(MCAT3(_1,_2,_3),_4)
#define MCAT5(_1, _2, _3, _4, _5) CAT(MCAT4(_1,_2,_3,_4),_5)
#define MCAT6(_1, _2, _3, _4, _5, _6) CAT(MCAT5(_1,_2,_3,_4,_5),_6)
#define MCAT7(_1, _2, _3, _4, _5, _6,_7) CAT(MCAT5(_1,_2,_3,_4,_5),_6)
#define MCAT8(_1, _2, _3, _4, _5, _6,_7,_8) CAT(MCAT5(_1,_2,_3,_4,_5,_6),_7)
#define MCAT9(_1, _2, _3, _4, _5, _6,_7,_8,_9) CAT(MCAT5(_1,_2,_3,_4,_5,_6,_7),_8)


#define COPY_V8_TO_STR(aaa,kkk)\
	v8::Local<v8::String> in_##kkk = v8::Local<v8::String>::Cast(aaa);\
	char kkk[255]={0};\
	V8ToCharPtr(in_##kkk,kkk);
#define COPY_TO_INT(aaa,kkk)\
	int kkk=0;\
	if(aaa.is_number_integer()){ kkk=aaa; }
#define COPY_TO_LNG(aaa,kkk)\
	long kkk=0;\
	if(aaa.is_number_integer()){ kkk=aaa; }
#define COPY_TO_DBL(aaa,kkk)\
	double kkk=0;\
	if(aaa.is_number_float()){ kkk=aaa; }
#define COPY_TO_STR(aaa,kkk)\
	char kkk[255]={0};\
	if(aaa.is_string()){\
		string str_in_##kkk=aaa;\
		strcpy(kkk,str_in_##kkk.c_str());\
	}

