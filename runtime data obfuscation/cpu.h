#pragma once
#include <intrin.h>
#include <cstdint>

__forceinline uint64_t _rand_fallback()
{
    uint64_t tsc = __rdtsc();
    return (tsc ^ (tsc >> 17) ^ (tsc << 23));
}

__forceinline uint64_t _rand_u64()
{
    uint64_t val = 0;
    unsigned char ok = 0;

    for (int i = 0; i < 10; ++i)
    {
        if (_rdrand64_step(&val)) return val;
    }
    return _rand_fallback();
}

__forceinline int64_t _rand_i64()
{
    return (int64_t)_rand_u64();
}

__forceinline int _rand_int()
{
    return (int)(_rand_u64() & 0x7FFFFFFF);
}

__forceinline unsigned int _rand_uint()
{
    return (unsigned int)(_rand_u64() & 0xFFFFFFFFu);
}

__forceinline short _rand_short()
{
    return (short)(_rand_u64() & 0xFFFF);
}

__forceinline unsigned short _rand_ushort()
{
    return (unsigned short)(_rand_u64() & 0xFFFFu);
}

__forceinline char _rand_char()
{
    return (char)(_rand_u64() & 0xFF);
}

__forceinline unsigned char _rand_uchar()
{
    return (unsigned char)(_rand_u64() & 0xFFu);
}

__forceinline intptr_t _rand_intptr()
{
#if defined(_WIN64)
    return (intptr_t)_rand_i64();
#else
    return (intptr_t)_rand_int();
#endif
}

__forceinline uintptr_t _rand_uintptr()
{
#if defined(_WIN64)
    return (uintptr_t)_rand_u64();
#else
    return (uintptr_t)_rand_uint();
#endif
}

/* basic implementation of slow memset randomized*/
__forceinline void memset_rand(void* address, size_t size)
{
    uint8_t* dst = reinterpret_cast<uint8_t*>(address);
    size_t i = 0;

    for (; i + 8 <= size; i += 8)
        *reinterpret_cast<uint64_t*>(dst + i) = _rand_u64();

    if (i < size)
    {
        uint64_t final = _rand_u64();
        for (size_t j = 0; j < size - i; ++j)
            dst[i + j] = static_cast<uint8_t>(final >> (j * 8));
    }
}

__forceinline bool is_avx512_supported() {
    int cpuInfo[4];
    __cpuidex(cpuInfo, 7, 0);
    return (cpuInfo[1] & (1 << 16));
}

__forceinline bool is_avx2_supported() {
    int cpuInfo[4];
    __cpuid(cpuInfo, 7);
    return (cpuInfo[1] & (1 << 5));
}

static bool has_avx512 = is_avx512_supported();
static bool has_avx2 = is_avx2_supported();

/* basic implementation of fast memcpy*/
inline void __memcpy(void* dst, void const* src, size_t size) {
    auto dst_ = static_cast<uint8_t*>(dst);
    auto src_ = static_cast<const uint8_t*>(src);
    size_t i = 0;

    if (has_avx512) {
        for (; i + 64 <= size; i += 64) {
            _mm_prefetch((char*)&src_[i + 256], _MM_HINT_T0);
            __m512i data = _mm512_load_si512((__m512i*)(src_ + i));
            _mm512_store_si512((__m512i*)(dst_ + i), data);
        }
    }
    else if (has_avx2) {
        for (; i + 32 <= size; i += 32) {
            __m256i data = _mm256_load_si256((__m256i*)(src_ + i));
            _mm256_store_si256((__m256i*)(dst_ + i), data);
        }
    }

    if (i < size)
        __movsb(dst_ + i, src_ + i, size - i);
}