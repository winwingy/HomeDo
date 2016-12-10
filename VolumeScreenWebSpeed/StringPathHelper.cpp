#include "StdAfx.h"
#include "StringPathHelper.h"

StringPathHelper::StringPathHelper()
{

}

string StringPathHelper::RemovePathExt(const string& pathString)
{
    string::size_type pos = pathString.rfind('.');
    if (pos != string::npos)
    {
        return string(pathString, pos, pathString.size()-pos);
    }
    return pathString;
}

string StringPathHelper::RemoveOnelastPath(const string& pathString)
{
    string::size_type pos = pathString.rfind('\\');
    if (pos != string::npos)
    {
        return string(pathString, pos, pathString.size() - pos);
    }
    return pathString;
}

string StringPathHelper::IntToString(int inValue)
{
    char szBuf[20] = { 0 };
    _itoa_s(inValue, szBuf, 10);
    return szBuf;
}

int StringPathHelper::StringToInt(const string& valueStr)
{
    return atoi(valueStr.c_str());
}
