#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using std::string;
using std::wstring;
using std::vector;

class StringPathHelper
{
public:
    StringPathHelper();
    static string RemovePathExt(const string& pathString);
    static string RemoveOnelastPath(const string& pathString);
    static string GetPathLastPart(const string& path);
    static string IntToString(int inValue);
    static int StringToInt(const string& valueStr);
    static void SplitStringBySign(const string& stringIn,
                           const string& sign, vector<string>* result);
    static string ToUpperString(const string& strLower);
    static void ReplaceString(
        string& orc, const string& findWhat, const string& replaceTo);
};
