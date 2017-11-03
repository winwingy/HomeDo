#include "stdafx.h"
#include "config.h"

#include "StringPathHelper.h"
#include "file.h"

Config* Config::pThis_ = nullptr;

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

	void SetPrivateProfileValue(
		const string& strAppName, const string& strKeyName, const string& strValue,
		const string& strFileName)
	{
		::WritePrivateProfileString(strAppName.c_str(), strKeyName.c_str(),
			strValue.c_str(), strFileName.c_str());
	}

	bool IsUserJobConfigByUserName(const string& jobCongfigFile)
	{
		char userName[1024];
		DWORD nameLength = 1024;
		BOOL ret = ::GetComputerName(userName, &nameLength);
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

    bool IsUseJobConfig(const string& jobCongfigFile)
    {
       return IsUserJobConfigByUserName(jobCongfigFile);
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
        if (!File::FileOrDirExist(strPathConfig))
        {
            moduleDir = StringPathHelper::RemoveOnelastPath(moduleDir);
            strPathConfig = moduleDir + strFileName;
            if (!File::FileOrDirExist(strPathConfig))
            {
                assert(0 && "Config File NotExist");
                return false;
            }
        }
        *configDir = moduleDir;
        return true;
    }
}

Config* Config::GetShared()
{
    if (!pThis_)
    {
        pThis_ = new Config;
        pThis_->Init("VolScrConfig.ini", "VolScrConfig_job.ini");
    }
    return pThis_;
}

Config::Config()
    : cache_()
    , configPath_()
{

}

Config::~Config(void)
{

}

string Config::GetConfigPath()
{
    return configPath_;
}

bool Config::Init(const string& strFileName, const string& fileNameJob)
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

string Config::GetValue(const string& strAppName,
                        const string& strKeyName, const string& strDefault)
{
    return GetPrivateProfileValue(
        strAppName, strKeyName, strDefault, configPath_);
}

int Config::GetValue(const string& strAppName,
                     const string& strKeyName, int defaultValue)
{
    string value = GetPrivateProfileValue(
        strAppName, strKeyName, StringPathHelper::IntToString(defaultValue),
        configPath_);
    return StringPathHelper::StringToInt(value);
}

void Config::SetValue(const string& strAppName,
                      const string& strKeyName, const string& strValue)
{
	SetPrivateProfileValue(strAppName, strKeyName, strValue, configPath_);
}

void Config::SetValue(const string& strAppName,
                      const string& strKeyName, int value)
{
	tstring strValue = StringPathHelper::IntToString(value);
	SetPrivateProfileValue(strAppName, strKeyName, strValue, configPath_);
}

bool Config::GetList(const string& listBegin, const string& listEnd,
                     vector<string>* listText)
{
    do 
    {
        File file(configPath_, "r");
        if (!file.IsValid())
            break;

        INT64 fileSize = file.GetFileSize();
        string text;
        text.resize(static_cast<size_t>(fileSize));
        if (text.empty())
            break;
        
        int readed = file.Read(&text[0], 1, static_cast<size_t>(fileSize));
        if (readed <= 0)
            break;

        string::size_type beg = text.find(listBegin);
        string::size_type end = text.find(listEnd);
        string target;
        if (beg != string::npos && end != string::npos)
        {
			beg += listBegin.length();
            if (beg < end)
            {
                target.assign(text, beg, end - beg);
            }
        }
        if (target.empty())
            break; 

        StringPathHelper::SplitStringBySign(target, "\n", listText);
        return true;
    } while (0);

    return false;
}


