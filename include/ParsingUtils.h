#ifndef __PARSING_UTILS_H__
#define __PARSING_UTILS_H__

#include <stdint.h>

namespace ParsingUtils
{

const int STR_SPLIT_MAX_SUBSTRINGS = 64;
typedef struct
{
    int count;
    char* substrs[STR_SPLIT_MAX_SUBSTRINGS];
} StrSplitInPlaceResult;

// Allocates a memory chunk on heap and loads into the whole file specified by the arg.
// Returned address must be used for memory deallocation when it is unnecessary.
// @return:
//     address of newly allocated heap chunk; don't forget to release it!
char* LoadFile(const char* file_path);

// It substitutes all leading and trailing whitespace chars with '\0' in a given buffer.
// @return:
//     pointer within 'str' buffer to the beginning of a string.
char* TrimString(char* str);

void ToUpperCase(char* str, int str_len); 
bool ValidateStrDigits(char* str, int str_len);
bool ValidateStrHex(char* str, int str_len);
uint32_t StrToInt(char* str);

// Splits a string pointed by 'str' into two sub-strings making a substitution of 'delim' char
// onto end-of-line char. Also performs a trimming of both sub-strings.
// @arg [out] 'sub1':
//     pointer within 'str' buffer to the beginning of the 1st sub-string.
// @arg [out] 'sub2':
//     pointer within 'str' buffer to the beginning of the 2nd sub-string.
// @return:
//     'true' on success, 'false' if splitting is impossible due to some reason ('sub1' and
//     'sub2' will stay undefined in this case).
bool StrStripInPlace(char* str, char delim, char** sub1, char** sub2);

void StrSplitInPlace(char* str, char delim, StrSplitInPlaceResult* result);
void StrCopy(char* dest, char* src, int buf_size);

// Removes comment from a string pointed by 'str' -- any text after a "//" token (including)
// the token. Also removes any leading/trailing whitespace if any.
// @return:
//     pointer within 'str' buffer to the beginning of a string.
char* RemoveComment(char* str);

// Creates a new char buffer with string from the beginning of 'data' to the last char before
// the 1st occurance of '\n' (within 'data'). New buffer is on heap and must be destroyed.
// @return:
//     pointer to the new char buffer.
char* GetNextLine(char* data);

}  // namespace ParsingUtils

#endif  // __PARSING_UTILS_H__
