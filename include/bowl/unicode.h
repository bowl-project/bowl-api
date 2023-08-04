#ifndef UNICODE_H
#define UNICODE_H

/**
 * The corresponding source of this header file is based on the flexible and economical UTF-8 decoder from Björn Höhrmann.
 * Copyright (c) 2008-2010 Bjoern Hoehrmann <bjoern@hoehrmann.de>
 * See http://bjoern.hoehrmann.de/utf-8/decoder/dfa/ for details.
 */

#include "common.h"

/** Represents a successful decoding state of a codepoint. */
#define UNICODE_UTF8_STATE_ACCEPT ((u32) 0)

/** Represents an erroneous decoding state of a codepoint. */
#define UNICODE_UTF8_STATE_REJECT ((u32) 12)

/** The codepoint of the unicode replacement character. */
#define UNICODE_REPLACEMENT_CHARACTER ((u32) 0xFFFD);

/** The unicode replacement character in its UTF-8 encoded form. */
extern u8 unicode_utf8_replacement_character[3];

/**
 * Decodes a unicode codepoint using the provided byte.
 * @param state A pointer to the current state of the decoding (initially this should be 'UNICODE_UTF8_STATE_ACCEPT')
 * @param codepoint A pointer to the resulting codepoint.
 * @param byte The next byte in the UTF-8 byte sequence.
 * @return u32 The state after decoding the provided byte. If this equals 'UNICODE_UTF8_STATE_ACCEPT', the codepoint can 
 * be accessed via 'codepoint'. If this equals 'UNICODE_UTF8_STATE_REJECT', the byte sequence is malformed and any further
 * call to this function will not change this state.
 */
u32 unicode_utf8_decode(u32 *state, u32 *codepoint, u32 byte);

/**
 * Encodes the unicode codepoint using UTF-8 and stores the bytes at the provided memory location.
 * @param codepoint The unicode codepoint to encode.
 * @param bytes The memory location.
 * @return u64 The number of bytes which were written to the memory location or the value 0 if the codepoint cannot be
 * represented with UTF-8. In this case three bytes representing the unicode replacement character are written to the 
 * memory location.
 */
u64 unicode_utf8_encode(u32 codepoint, u8 *bytes);

/**
 * Counts the number of unicode codepoints in the provided UTF-8 encoded byte-sequence.
 * @param bytes The UTF-8 encoded byte sequence.
 * @param length The length of the byte sequence in number of bytes.
 * @return u64 The number of unicode codepoints, (u64) -1 if the byte sequence is malformed or (u64) -2 if the
 * UTF-8 byte sequence is incomplete.
 */
u64 unicode_utf8_count(u8 *bytes, u64 length);

/**
 * Checks whether the provided codepoint is a space character. 
 * @param codepoint The codepoint to check.
 * @return bool Whether the codepoint corresponds to the unicode class of space characters. 
 */
bool unicode_is_space(u32 codepoint);

/**
 * Escapes the provided byte sequence and returns the corresponding codepoint. If the first codepoint in 
 * the byte sequence is not a backslash, this first codepoint is returned. If the sequence is empty the 
 * value 0 is returned. If the byte sequence is malformed or incomplete, the unicode replacement character
 * is returned.
 * @param bytes A reference to the byte sequence.
 * @param length The length of the byte sequence.
 * @param codepoint A pointer where the resulting codepoint is stored.
 * @return u64 The number of bytes that were decoded.
 */
u64 unicode_utf8_escape_sequence(u8 *bytes, u64 length, u32 *codepoint);

/**
 * Escapes the provided unicode string and returns the corresponding codepoint. If the first codepoint in
 * the string is not a backslash, this first codepoint is returned. If the string is empty, the value 0 is
 * returned. If the escape sequence is invalid, the unicode replacement character is returned.
 * @param codepoints The unicode string.
 * @param length The length of the unicode string.
 * @param codepoint The memory location where the codepoint should be stored.
 * @return u32 The number of codepoints that were used for the escape sequence.
 */
u64 unicode_escape_sequence(u32 *codepoints, u64 length, u32 *codepoint);

/**
 * Tries to decode a whole codepoint.
 * @param bytes The bytes to decode.
 * @param length The number of bytes available.
 * @param state The decoding state.
 * @param codepoint The memory location where the codepoint should be stored.
 * @return u64 The number of bytes that were decoded.
 */
u64 unicode_utf8_decode_codepoint(u8 *bytes, u64 length, u32 *state, u32 *codepoint);

/**
 * Converts the provided C string (i.e., ASCII encoded and null-terminated) to an unicode string.
 * @param string The C string.
 * @return u32* The unicode string.
 */
u32 *unicode_from_string(char *string);

/**
 * Converts the provided unicode string to a null-terminated C string using UTF-8.
 * @param codepoints The codepoints of the unicode string.
 * @param length The number of codepoints.
 * @return char* The null-terminated C string.
 */
char *unicode_to_string(u32 *codepoints, u64 length);

#endif