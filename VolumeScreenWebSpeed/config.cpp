#include "StdAfx.h"
#include "config.h"

#include <memory>
#include <assert.h>
#include "StringPathHelper.h"

string GetPrivateProfileValue(
    const string& strAppName, const string& strKeyName, const string& strDefault,
    const string& strFileName)
{
    TCHAR valueBuf[1024] = { 0 };
    ::GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(),
                              strDefault.c_str(), valueBuf, 
                              sizeof(valueBuf) - 1, strFileName.c_str());
    return valueBuf;
}

Config::Config()
    : cache_()
    , configPath_()
{

}

Config::~Config(void)
{

}

bool IsUseJobConfig(const string& jobCongfigFile)
{
    char userName[1024];
    DWORD nameLength = 1024;
    BOOL ret = ::GetUserName(userName, &nameLength);
    if (ret)
    {
        string jobName = GetPrivateProfileValue(
            "Set", "JobConfigName", "NotConfigName@#$%!##",
            jobCongfigFile);

        if (jobName == userName)
        {
            return true;
        }
    }
    return false;
}

bool FileExist(const string& path)
{
    FILE* fp = nullptr;
    fopen_s(&fp, path.c_str(), "r");
    if (fp)
        fclose(fp);

    return !!fp;
}

bool Config::Init(const string& strFileName, const string& fileNameJob)
{
    char szPath[2048] = { 0 };
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    string strPath = StringPathHelper::RemoveOnelastPath(szPath);
    string strPathConfig = strPath + strFileName;
    if (!FileExist(strPathConfig))
    {
        strPathConfig =
            StringPathHelper::RemoveOnelastPath(strPath) + strFileName;
        if (!FileExist(strPathConfig))
        {
            assert(0 && "Config File NotExist");
            return false;
        }
    }

    string strPathConfigJob = strPath + fileNameJob;
    if (IsUseJobConfig(strPathConfigJob))
    {
        configPath_ = strPathConfigJob;
    }
    else
    {
        configPath_ = strPath + strFileName;
    }
    return true;
}

string Config::GetValue(const string& strAppName, 
                        const string& strKeyName, const string& strDefault)
{
    return GetPrivateProfileValue(
        strAppName, strKeyName, strDefault, configPath_);
}

int Config::GetValue(const string& strAppName,
                     const string& strKeyName, int defaultValue)
{
    string value =  GetPrivateProfileValue(
        strAppName, strKeyName,StringPathHelper::IntToString(defaultValue), 
        configPath_);
    return StringPathHelper::StringToInt(value);
}

void Config::SetValue(const string& strAppName, 
                      const string& strKeyName, const string& value)
{
    assert(0);
}

void Config::SetValue(const string& strAppName,
                      const string& strKeyName, int value)
{
    assert(0);
}
