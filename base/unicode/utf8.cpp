/*
 * Unitex
 *
 * Copyright (C) 2001-2017 Université Paris-Est Marne-la-Vallée <unitex@univ-mlv.fr>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
 *
 */
/**
 * @file      utf8.cpp
 * @brief     Functions to handle UTF-8
 *
 * @author    cristian.martinez@univ-paris-est.fr (martinec)
 *
 * @attention Do not include this file directly, rather include the
 *            base/common.h header file to gain this file's functionality
 *
 * @note      Use cpplint.py tool to detect style errors:
 *            `cpplint.py --linelength=120 `utf8.cpp`
 *
 * @date      September 2016
 */
/* ************************************************************************** */
// Header for this file
#include "base/unicode/utf8.h"
/* ************************************************************************** */
namespace unitex {
/* ************************************************************************** */

/**
 * @def      U_MAX_WIDTH_UTF8
 * @brief    Maximal numbers of UTF8 bytes that a unichar can encode
 * @see      U_MAX_VALUE
 */
#define U_MAX_WIDTH_UTF8 3

/**
 * Test if the code point is valid
 */
#define u_has_valid_utf8_width(n) UNITEX_LIKELY(n > 0 && n <= U_MAX_WIDTH_UTF8)

/**
 * UTF-8 is variable-Width (1 to 4 bytes)
 *
 * Length    Code Points              1st Byte            Mask  2nd Byte  3rd Byte  4th Byte  i
 *   0     0xD800..0xDFFF    80..BF   128..191  00000000  0x00  ******* ill-formed *******    0
 *   1     0x0000..0x007F    00..7F   000..127  0xxxxxxx  0x7f                                1
 *   2     0x0080..0x07FF    C2..DF   194..223  110xxxxx  0x1f  80..BF                        2
 *   3     0x0800..0x0FFF    E0..E0   224..224  1110xxxx  0x0f  A0..BF    80..BF              3
 *   3     0x1000..0xCFFF    E1..EC   225..236  1110xxxx  0x0f  80..BF    80..BF              4
 *   3     0xD000..0xD7FF    ED..ED   237..237  1110xxxx  0x0f  80..9F    80..BF              5
 *   3     0xE000..0xFFFF    EE..EF   238..239  1110xxxx  0x0f  80..BF    80..BF              6
 *   4    0x10000..0x3FFFF   F0..F0   240..240  11110xxx  0x07  90..BF    80..BF    80..BF    7
 *   4    0x40000..0xFFFFF   F1..F3   241..243  11110xxx  0x07  80..BF    80..BF    80..BF    8
 *   4   0x100000..0x10FFFF  F4..F4   244..244  11110xxx  0x07  80..8F    80..BF    80..BF    9
 *
 *   1st Byte between C0 and C1 (to try to encode ASCII characters with two bytes
 *   instead of one) or between F5 and FF (to try to encode numbers larger than
 *   the maximal unicode codepoint) are also assigned to the index 0 and thus
 *   considered invalid
 */
const u_info_utf8_t kUTF8ByteInfo[] = {
  {0,0x00,{0x0000,  0x0000  },{{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000}}},  // 0
  {1,0x7f,{0x0000,  0x007F  },{{0x0000,0x007F},{0x0000,0x0000},{0x0000,0x0000},{0x0000,0x0000}}},  // 1
  {2,0x1f,{0x0080,  0x07FF  },{{0x00C2,0x00DF},{0x0080,0x00BF},{0x0000,0x0000},{0x0000,0x0000}}},  // 2
  {3,0x0f,{0x0080,  0x0FFF  },{{0x00E0,0x00E0},{0x00A0,0x00BF},{0x0080,0x00BF},{0x0000,0x0000}}},  // 3
  {3,0x0f,{0x1000,  0xCFFF  },{{0x00E1,0x00EC},{0x0080,0x00BF},{0x0080,0x00BF},{0x0000,0x0000}}},  // 4
  {3,0x0f,{0xD000,  0xD7FF  },{{0x00ED,0x00ED},{0x0080,0x009F},{0x0080,0x00BF},{0x0000,0x0000}}},  // 5
  {3,0x0f,{0xE000,  0xFFFF  },{{0x00EE,0x00EF},{0x0080,0x00BF},{0x0080,0x00BF},{0x0000,0x0000}}},  // 6
  {4,0x07,{0x10000, 0x3FFFF },{{0x00F0,0x00F0},{0x0090,0x00BF},{0x0080,0x00BF},{0x0080,0x00BF}}},  // 7
  {4,0x07,{0x40000, 0xFFFFF },{{0x00F1,0x00F3},{0x0080,0x00BF},{0x0080,0x00BF},{0x0080,0x00BF}}},  // 8
  {4,0x07,{0x100000,0x10FFFF},{{0x00F4,0x00F4},{0x0080,0x00BF},{0x0080,0x00BF},{0x0080,0x00BF}}},  // 9
};

/**
 *
 */
const uint8_t kUTF8ByteInfoIndex[] = {
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 000-015
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 016-031
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 032-047
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 048-063
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 064-079
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 080-095
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 096-111
                     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  // 112-127
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 128-143
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 144-159
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 160-175
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 176-191
                     0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 192-207
                     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,  // 208-223
                     3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 6,  // 224-239
                     7, 8, 8, 8, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; // 240-255

/**
 * @def      kUTF8ReplacementChar
 * @brief    The unicode replacement character used when decoding byte sequences
 *           that cannot be successfully converted
 * @see      U_REPLACEMENT_CHAR
 */
const uint8_t kUTF8ReplacementChar[] = {0xEF, 0xBF, 0xBD};
/* ************************************************************************** */
int u_encode_utf8(const unichar* source, char* destination) {
  const unichar* it = source;
  register int pos = 0;

  // loop till the end of string
  while (*it != '\0') {
    // ASCII characters lower than 0x80
    // 1-byte unicode codepoint
    if (*it <= 0x7F) {
      destination[pos++] = (char) *it;
    }
    // 2-bytes unicode codepoints
    else if (*it <= 0x7FF) {
      destination[pos++] = (char) (0xC0 | (*it >> 6));
      destination[pos++] = (char) (0x80 | (*it & 0x3F));
    }
    // 3-bytes unicode codepoints
    // Even if such behavior should be treated as an encoding error,
    // note that this allows the encoding of the code points between
    // 0xD800 and 0xDFFF, i.e. the code points for UTF-16 surrogates
    else if (*it <= 0xFFFF) {
      destination[pos++] = (char) (0xE0 | (*it >> 12));
      destination[pos++] = (char) (0x80 | ((*it >> 6) & 0x3F));
      destination[pos++] = (char) (0x80 | (*it & 0x3F));
    }
    // 4-bytes unicode codepoints
    else if (*it <= 0x10FFFF) {
      destination[pos++] = (char) (0xF0 | (*it  >> 18));
      destination[pos++] = (char) (0x80 | ((*it >> 12) & 0x3F));
      destination[pos++] = (char) (0x80 | ((*it >> 6)  & 0x3F));
      destination[pos++] = (char) (0x80 | (*it & 0x3F));
    }
    // if the codepoint is invalid encode it as the replacement char
    else {
      destination[pos++] = (char) kUTF8ReplacementChar[0];
      destination[pos++] = (char) kUTF8ReplacementChar[1];
      destination[pos++] = (char) kUTF8ReplacementChar[2];
    }
    // advance the character pointer
    ++it;
  }

  // indicate the end of the string
  destination[pos] = '\0';

  // return the length of the destination string
  return pos;
}

int u_decode_utf8(const char* source, unichar* destination) {
  const char* it = source;
  register int pos = 0;

  // loop till the end of string
  while (*it != '\0') {
    // 1-byte encodes UTF-8 information
    uint8_t encoded_index  = kUTF8ByteInfoIndex[static_cast<uint8_t>(*it)];
    //
    uint8_t encoded_width  = kUTF8ByteInfo[encoded_index].width;
    //
    uint8_t encoded_offset = encoded_width - 1u;
    // set the replacement char (0xFFFD) as default value
    unichar value = U_REPLACEMENT_CHAR;

    if (u_has_valid_utf8_width(encoded_width)) {
      // 1-byte unicode codepoint
      value = *it & kUTF8ByteInfo[encoded_index].mask;

      // n-byte following unicode codepoints
      for (uint8_t i=0u; i < encoded_offset && *(it+1) != '\0'; ++i, ++it) {
         value = (value<<6) | ((*(it+1)) & 0x3F);
      }
    } else {
      it += encoded_offset;
    }

    // unichar = decoded value
    destination[pos++] = u_replace_if_invalid(value);

    // advance the character pointer
    ++it;
  }

  // indicate the end of the string
  destination[pos] = '\0';

  // return the length of the destination string
  return pos;
}

int u_strnicmp(const unichar* s1, const unichar* s2, size_t n) {
  const unichar*  it1 = s1;
  const unichar*  it2 = s2;
  const u_info_t* info1 = '\0';
  const u_info_t* info2 = '\0';

  while(n) {
    if(*it1 == '\0' || *it2 == '\0') {
      break;
    }

    info1 = u_info(*it1);
    info2 = u_info(*it2);

    // 0: s1 and s2 have simple case folding
    // 1: s2 have full case folding
    // 2: s1 have full case folding
    // 3: s1 and s2 have both full case folding
    int fold_expands = 2 * u_has_flag_fold_expands(info1) +
                           u_has_flag_fold_expands(info2);

    // simple case folding compare (where string lengths don't change)
    if(UNITEX_LIKELY(fold_expands == 0)) {
      if(*it1 + info1->variant[U_CASE_FOLD] !=
         *it2 + info2->variant[U_CASE_FOLD]) {
        break;
      }
    // s1 or s2 have full case folding
    } else if (fold_expands == 1 || fold_expands == 2) {
      int index  = fold_expands == 1 ? info2->variant[U_CASE_FOLD] :
                                       info1->variant[U_CASE_FOLD];
      const unichar** it_unfold = fold_expands == 1 ? &it1 : &it2;
      const unichar*  it_fold   = &kUSpecialVariants[index + 1];
      size_t length = kUSpecialVariants[index];
      int strings_not_equal = u_strnicmp(*it_unfold,it_fold,length);
      if (strings_not_equal) return strings_not_equal;
      (*it_unfold) += (length-1);
    // both of strings have full case folding
    } else {
      if (info1->variant[U_CASE_FOLD] !=
          info2->variant[U_CASE_FOLD]) {
        break;
      }
    }

    ++it1;
    ++it2;
    --n;
  }

  return n == 0 ? 0 : ((int32_t) *it1 - (int32_t) *it2);
}

int u_stricmp(const unichar* s1, const unichar* s2) {
  const unichar*  it1 = s1;
  const unichar*  it2 = s2;
  const u_info_t* info1 = '\0';
  const u_info_t* info2 = '\0';

  for(;;) {
    if(*it1 == '\0' || *it2 == '\0') {
      break;
    }

    info1 = u_info(*it1);
    info2 = u_info(*it2);

    // 0: s1 and s2 have simple case folding
    // 1: s2 have full case folding
    // 2: s1 have full case folding
    // 3: s1 and s2 have both full case folding
    int fold_expands = 2 * u_has_flag_fold_expands(info1) +
                           u_has_flag_fold_expands(info2);

    // simple case folding compare (where string lengths don't change)
    if(UNITEX_LIKELY(fold_expands == 0)) {
      if(*it1 + info1->variant[U_CASE_FOLD] !=
         *it2 + info2->variant[U_CASE_FOLD]) {
        break;
      }
    // s1 or s2 have full case folding
    } else if (fold_expands == 1 || fold_expands == 2) {
      int index  = fold_expands == 1 ? info2->variant[U_CASE_FOLD] :
                                       info1->variant[U_CASE_FOLD];
      const unichar** it_unfold = fold_expands == 1 ? &it1 : &it2;
      const unichar*  it_fold   = &kUSpecialVariants[index + 1];
      size_t length = kUSpecialVariants[index];
      int strings_not_equal = u_strnicmp(*it_unfold,it_fold,length);
      if (strings_not_equal) return strings_not_equal;
      (*it_unfold) += (length-1);
    // both of strings have full case folding
    } else {
      if (info1->variant[U_CASE_FOLD] !=
          info2->variant[U_CASE_FOLD]) {
        break;
      }
    }

    ++it1;
    ++it2;
  }

  return ((int32_t) *it1 - (int32_t) *it2);
}
/* ************************************************************************** */
}  // namespace unitex
