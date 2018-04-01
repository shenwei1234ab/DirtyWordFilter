//
//  vnDefine.h
//  vnlib
//
//  Created by Wayne on 5/7/14.
//  Copyright (c) 2014 viichi.com. All rights reserved.
//

#ifndef vnlib_vnDefine_h
#define vnlib_vnDefine_h


#include <cstddef>
#include <cstdint>
//#include <cuchar>
#include <string>




typedef int8_t          s8;
typedef int16_t         s16;
typedef int32_t         s32;


typedef uint8_t         u8;
typedef uint16_t        u16;
typedef uint32_t        u32;

//typedef int64_t         s64;
//typedef uint64_t        u64;

typedef long long		s64;
typedef unsigned long long u64;

typedef float           f32;
typedef double          f64;




typedef char            c8;
typedef char16_t        c16;
typedef char32_t        c32;

typedef std::string u8string;
typedef std::string     str8;
typedef std::u16string  str16;
typedef std::u32string  str32;

typedef long ssize_t;



#endif
