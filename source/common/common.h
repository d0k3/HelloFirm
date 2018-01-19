#pragma once

#include <inttypes.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define u64 uint64_t

#define vu8 volatile u8
#define vu16 volatile u16
#define vu32 volatile u32
#define vu64 volatile u64

#define max(a,b) \
	(((a) > (b)) ? (a) : (b))
#define min(a,b) \
	(((a) < (b)) ? (a) : (b))
#define getbe16(d) \
	((((u8*)d)[0]<<8) | ((u8*)d)[1])
#define getbe32(d) \
	((((u32) getbe16(d))<<16) | ((u32) getbe16(d+2)))
#define getbe64(d) \
	((((u64) getbe32(d))<<32) | ((u64) getbe32(d+4)))
#define getle16(d) \
	((((u8*)d)[1]<<8) | ((u8*)d)[0])
#define getle32(d) \
	((((u32) getle16(d+2))<<16) | ((u32) getle16(d)))
#define getle64(d) \
	((((u64) getle32(d+4))<<32) | ((u64) getle32(d)))
#define align(v,a) \
	(((v) % (a)) ? ((v) + (a) - ((v) % (a))) : (v))

