// SPDX-FileCopyrightText: 2019-2023 Connor McLaughlin <stenzek@gmail.com>
// SPDX-License-Identifier: (GPL-3.0 OR CC-BY-NC-ND-4.0)

#pragma once
#include "types.h"

#ifdef _MSC_VER
#include <intrin.h>
#endif

// Zero-extending helper
template<typename TReturn, typename TValue>
ALWAYS_INLINE constexpr TReturn ZeroExtend(TValue value)
{
  return static_cast<TReturn>(static_cast<typename std::make_unsigned<TReturn>::type>(
    static_cast<typename std::make_unsigned<TValue>::type>(value)));
}
// Sign-extending helper
template<typename TReturn, typename TValue>
ALWAYS_INLINE constexpr TReturn SignExtend(TValue value)
{
  return static_cast<TReturn>(
    static_cast<typename std::make_signed<TReturn>::type>(static_cast<typename std::make_signed<TValue>::type>(value)));
}

// Type-specific helpers
template<typename TValue>
ALWAYS_INLINE constexpr u16 ZeroExtend16(TValue value)
{
  return ZeroExtend<u16, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u32 ZeroExtend32(TValue value)
{
  return ZeroExtend<u32, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u64 ZeroExtend64(TValue value)
{
  return ZeroExtend<u64, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u16 SignExtend16(TValue value)
{
  return SignExtend<u16, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u32 SignExtend32(TValue value)
{
  return SignExtend<u32, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u64 SignExtend64(TValue value)
{
  return SignExtend<u64, TValue>(value);
}
template<typename TValue>
ALWAYS_INLINE constexpr u8 Truncate8(TValue value)
{
  return static_cast<u8>(static_cast<typename std::make_unsigned<decltype(value)>::type>(value));
}
template<typename TValue>
ALWAYS_INLINE constexpr u16 Truncate16(TValue value)
{
  return static_cast<u16>(static_cast<typename std::make_unsigned<decltype(value)>::type>(value));
}
template<typename TValue>
ALWAYS_INLINE constexpr u32 Truncate32(TValue value)
{
  return static_cast<u32>(static_cast<typename std::make_unsigned<decltype(value)>::type>(value));
}

// BCD helpers
ALWAYS_INLINE constexpr u8 BinaryToBCD(u8 value)
{
  return ((value / 10) << 4) + (value % 10);
}
ALWAYS_INLINE constexpr u8 PackedBCDToBinary(u8 value)
{
  return ((value >> 4) * 10) + (value % 16);
}
ALWAYS_INLINE constexpr u8 IsValidBCDDigit(u8 digit)
{
  return (digit <= 9);
}
ALWAYS_INLINE constexpr u8 IsValidPackedBCD(u8 value)
{
  return IsValidBCDDigit(value & 0x0F) && IsValidBCDDigit(value >> 4);
}

// Boolean to integer
ALWAYS_INLINE constexpr u8 BoolToUInt8(bool value)
{
  return static_cast<u8>(value);
}
ALWAYS_INLINE constexpr u16 BoolToUInt16(bool value)
{
  return static_cast<u16>(value);
}
ALWAYS_INLINE constexpr u32 BoolToUInt32(bool value)
{
  return static_cast<u32>(value);
}
ALWAYS_INLINE constexpr u64 BoolToUInt64(bool value)
{
  return static_cast<u64>(value);
}

// Integer to boolean
template<typename TValue>
ALWAYS_INLINE constexpr bool ConvertToBool(TValue value)
{
  return static_cast<bool>(value);
}

// Unsafe integer to boolean
template<typename TValue>
ALWAYS_INLINE bool ConvertToBoolUnchecked(TValue value)
{
  // static_assert(sizeof(uint8) == sizeof(bool));
  bool ret;
  std::memcpy(&ret, &value, sizeof(bool));
  return ret;
}

// Generic sign extension
template<int NBITS, typename T>
ALWAYS_INLINE constexpr T SignExtendN(T value)
{
  // http://graphics.stanford.edu/~seander/bithacks.html#VariableSignExtend
  constexpr int shift = 8 * sizeof(T) - NBITS;
  return static_cast<T>((static_cast<std::make_signed_t<T>>(value) << shift) >> shift);
}

/// Returns the number of zero bits before the first set bit, going MSB->LSB.
template<typename T>
ALWAYS_INLINE unsigned CountLeadingZeros(T value)
{
#ifdef _MSC_VER
  if constexpr (sizeof(value) >= sizeof(u64))
  {
    unsigned long index;
    _BitScanReverse64(&index, ZeroExtend64(value));
    return static_cast<unsigned>(index) ^ static_cast<unsigned>((sizeof(value) * 8u) - 1u);
  }
  else
  {
    unsigned long index;
    _BitScanReverse(&index, ZeroExtend32(value));
    return static_cast<unsigned>(index) ^ static_cast<unsigned>((sizeof(value) * 8u) - 1u);
  }
#else
  if constexpr (sizeof(value) >= sizeof(u64))
    return static_cast<unsigned>(__builtin_clzl(ZeroExtend64(value)));
  else if constexpr (sizeof(value) == sizeof(u32))
    return static_cast<unsigned>(__builtin_clz(ZeroExtend32(value)));
  else
    return static_cast<unsigned>(__builtin_clz(ZeroExtend32(value))) & static_cast<unsigned>((sizeof(value) * 8u) - 1u);
#endif
}

/// Returns the number of zero bits before the first set bit, going LSB->MSB.
template<typename T>
ALWAYS_INLINE unsigned CountTrailingZeros(T value)
{
#ifdef _MSC_VER
  if constexpr (sizeof(value) >= sizeof(u64))
  {
    unsigned long index;
    _BitScanForward64(&index, ZeroExtend64(value));
    return index;
  }
  else
  {
    unsigned long index;
    _BitScanForward(&index, ZeroExtend32(value));
    return index;
  }
#else
  if constexpr (sizeof(value) >= sizeof(u64))
    return static_cast<unsigned>(__builtin_ctzl(ZeroExtend64(value)));
  else
    return static_cast<unsigned>(__builtin_ctz(ZeroExtend32(value)));
#endif
}
