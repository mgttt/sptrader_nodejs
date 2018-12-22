#ifndef spcore_CCTypes_h
#define spcore_CCTypes_h

#if defined(__APPLE__) || defined(__ANDROID__) || defined(__LINUX__) || defined(__linux__)
typedef long long           bigint;
typedef unsigned long long  u_bigint;
#else
typedef __int64          bigint;
typedef unsigned __int64 u_bigint;
#endif

#endif
