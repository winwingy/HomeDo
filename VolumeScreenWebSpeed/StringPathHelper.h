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

};
