#include "StdAfx.h"
#include "config.h"

#include <memory>
#include <assert.h>
#include "StringPathHelper.h"

File* File::pThis_ = nullptr;

namespace
{
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

}


File* File::GetShared()
{
    if (!pThis_)
    {
        pThis_ = new File;
        pThis_->Init("VolScrConfig.ini", "VolScrConfig_job.ini");
    }
    return pThis_;
}

File::File()
    : cache_()
    , configPath_()
{

}

File::~File(void)
{

}

bool ConfigFileExist(const string& strFileName, string* configDir)
{
    if (!configDir)
    {
        return false;
    }
    char szPath[2048] = { 0 };
    GetModuleFileName(nullptr, szPath, sizeof(szPath));
    string moduleDir = StringPathHelper::RemoveOnelastPath(szPath);
    string strPathConfig = moduleDir + strFileName;
    if (!FileExist(strPathConfig))
    {
        moduleDir = StringPathHelper::RemoveOnelastPath(moduleDir);
        strPathConfig = moduleDir + strFileName;
        if (!FileExist(strPathConfig))
        {
            assert(0 && "Config File NotExist");
            return false;
        }
    }
    *configDir = moduleDir;
    return true;
}

bool File::Init(const string& strFileName, const string& fileNameJob)
{
    string configDir;
    if (!ConfigFileExist(strFileName, &configDir) &&
        !ConfigFileExist(fileNameJob, &configDir))
        return false;

    string strPathConfigJob = configDir + fileNameJob;
    if (IsUseJobConfig(strPathConfigJob))
    {
        configPath_ = strPathConfigJob;
    }
    else
    {
        configPath_ = configDir + strFileName;
    }
    return true;
}

string File::GetValue(const string& strAppName,
                        const string& strKeyName, const string& strDefault)
{
    return GetPrivateProfileValue(
        strAppName, strKeyName, strDefault, configPath_);
}

int File::GetValue(const string& strAppName,
                     const string& strKeyName, int defaultValue)
{
    string value = GetPrivateProfileValue(
        strAppName, strKeyName, StringPathHelper::IntToString(defaultValue),
        configPath_);
    return StringPathHelper::StringToInt(value);
}

void File::SetValue(const string& strAppName,
                      const string& strKeyName, const string& value)
{
    assert(0);
}

void File::SetValue(const string& strAppName,
                      const string& strKeyName, int value)
{
    assert(0);
}

vector<string> File::GetList(const string& listBegin, const string& listEnd)
{
    char szBuffer[2048] = { 0 };
    fread(szBuffer, 2047, 1, fp);
    fclose(fp);
}


