#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "Asserts.h"
#include "ParsingUtils.h"


namespace ParsingUtils
{

#define _HEX_PREFIX_LENGTH        2  // "0x" -- is our prefix

static bool _HasHexPrefix(char* str)
{
    if ( str[0] == '0' && (str[1] == 'x' || str[1] == 'X') )
        return true;
    else
        return false;
}


char* LoadFile(const char* file_path)
{
    FILE* file = fopen(file_path, "rt");
    DEBUG_ASSERT(file != NULL, file_path);
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
    char* file_data = new char[file_size+1];
    memset(file_data, '\0', file_size);

    int charIdx = 0, c = EOF;
    fseek(file, 0, SEEK_SET);
    while ( (c = fgetc(file)) != EOF)  // while not EOF
        file_data[charIdx++] = (char) c;
    
    fclose(file);
    return file_data;
}


char* TrimString(char* str)
{
    char* result = str;
    int len = strlen(str);

    // remove leading whitespace
    for (int i = 0; i < len; i++)
        if (str[i] == ' ' || str[i] == '\t')
        {
            str[i] = '\0';
            result++;
        }
        else
            break;
    
    // remove trailing whitespace
    for (int i = len-1; i > 0; i--)
        if (str[i] == ' ' || str[i] == '\t')
            str[i] = '\0';
        else
            break;

    return result;
}


void ToUpperCase(char* str, int str_len)
{
    for (int i = 0; i < str_len; i++)
        str[i] = (char) toupper(str[i]);
}


bool ValidateStrDigits(char* str, int str_len)
{
    for (int i = 0; i < str_len; i++)
        if (str[i] < '0' || str[i] > '9')
            return false;
    return true;  // all is OK by default
}


bool ValidateStrHex(char* str, int str_len)
{
    //bool result = _true;
    int i = (str_len > _HEX_PREFIX_LENGTH) && _HasHexPrefix(str) ? _HEX_PREFIX_LENGTH : 0;
    for (; i < str_len; i++)
    {
        char ch = (char) toupper(str[i]);
        if ( ((ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F')) == false)
            return false;
    }
    return true;  // all is OK by default
}


uint32_t StrToInt(char* str)
{
    if (_HasHexPrefix(str))
        return (uint32_t) strtol(str, NULL, 16);
    else
        return (uint32_t) strtol(str, NULL, 10);
}


bool StrStripInPlace(char* str, char delim, char** sub1, char** sub2)
{
    char* delimPtr = strchr(str, delim);
    if (strlen(str) > 0 && delimPtr != NULL)  // good case
    {
        *delimPtr = '\0';  // in-place splitting
        *sub1 = ParsingUtils::TrimString(str);
        *sub2 = ParsingUtils::TrimString(delimPtr + 1);  // 'sub2' starts after '\0' where 'delimPtr' points to
        return true;
    }
    else  // bad case
        return false;
}


void StrSplitInPlace(char* str, char delim, StrSplitInPlaceResult* result)
{
    char* delimPtr = NULL;
    result->count = 0;

    while ( (delimPtr = strchr(str, delim)) != NULL )
    {
        *delimPtr = '\0';  // in-place splitting
        result->substrs[result->count++] = ParsingUtils::TrimString(str);
        str = delimPtr + 1;  // 'str' starts after '\0' where 'delimPtr' points to
    }

    result->substrs[result->count++] = ParsingUtils::TrimString(str);
}


void StrCopy(char* dest, char* src, int buf_size)
{
    int srcSize = strlen(src) + 1;  // +1 for a '\0' char
    if (buf_size >= srcSize)
        memcpy(dest, src, srcSize);
    else
    {
        memcpy(dest, src, buf_size);
        dest[buf_size-1] = '\0';
    }
}


char* GetNextLine(char* data)
{
    char* result = NULL;
    char* newLine = strchr(data, '\n');
    if (newLine != NULL)
    {
        int resultLen = (uint32_t)newLine - (uint32_t)data;
        result = new char[resultLen + 1];  // +1 for a '\0' char
        memcpy(result, data, resultLen);
        result[resultLen] = '\0';
    }
    return result;
}


char* RemoveComment(char* str)
{
    char* commentPtr = strstr(str, "//");  // with "//" token
    if (commentPtr != NULL)  // a comment is found
        *commentPtr = '\0';
    // additionally, remove whitespaces
    return TrimString(str);
}

}  // namespace ParsingUtils
