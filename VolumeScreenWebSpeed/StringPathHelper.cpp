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
        return string(pathString, 0, pos);
    }
    return pathString;
}

string StringPathHelper::RemoveOnelastPath(const string& pathString)
{
    string ret(pathString);
    if (!ret.empty() && (ret[ret.size() - 1] == '\\'))
    {
        ret.erase(ret.size() - 1);
    } 
    string::size_type pos = ret.rfind('\\');
    if (pos != string::npos)
    {
        ret = string(ret, 0, pos);
    }
    if (!ret.empty() && (ret[ret.size() -1] != '\\'))
    {
        ret += '\\';
    }
    return ret;
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

void StringPathHelper::SplitStringBySign(
    const string& stringIn, const string& sign, vector<string>* result)
{
    string::size_type posBeg = 0;
    string::size_type posEnd = -1;
    while ((posEnd = stringIn.find(sign, posBeg)) != -1)//""  a++b++c a++b++ ++b++C++ a++++b
    {
        string child(stringIn, posBeg, posEnd - posBeg);
        if (!child.empty())
        {
            result->push_back(child);
        }
        posBeg = posEnd + sign.length();
    }

    string childOut(stringIn, posBeg, stringIn.length() - posBeg);
    if (!childOut.empty())
    {
        result->push_back(childOut);
    }
}

string StringPathHelper::ToUpperString(const string& strLower)
{
    string strRet;
    for (string::const_iterator iter = strLower.begin(); iter != strLower.end();
         ++iter)
    {
        strRet.push_back(toupper(*iter));
    }
    return strRet;
}

void StringPathHelper::ReplaceString(
    string& orc, const string& findWhat, const string& replaceTo)
{
    string::size_type posBeg = 0;
    string::size_type posEnd = -1;
    while ((posEnd = orc.find(findWhat, posBeg)) != string::npos)
    {
        orc.replace(posEnd, findWhat.length(), replaceTo);
        posBeg = posEnd + findWhat.length();
    }
}

