#include "StdAfx.h"
#include "config.h"
#include "StringPathHelper.h"

const TCHAR* CONFIG_SET = "Set";
const TCHAR* CONFIG_POWER_ON_START_PROGRESS = "PowerOnStartProgress";
const TCHAR* CONFIG_SET_HOTKEY = "HotKey";
const TCHAR* CONFIG_SET_PROGRESS_HOTKEY = "ProgressHotKey";
const TCHAR* CONFIG_INF_FILENAME = "VolScrConfig.ini";
const TCHAR* CONFIG_INF_FILENAME_JOB = "VolScrConfigJob.ini";

const TCHAR* CONFIG_SET_KILLNAME_BEGIN = "[KillNameBegin]";
const TCHAR* CONFIG_SET_KILLNAME_END = "[KillNameEnd]";

string Config::GetPrivateProfileValue(
    const string& strAppName, const string& strKeyName, const string& strDefault,
    const string& strFileName)
{
    TCHAR valueBuf[1024] = { 0 };
    ::GetPrivateProfileString(strAppName.c_str(), strKeyName.c_str(),
                              strDefault.c_str(), valueBuf, sizeof(valueBuf) - 1,
                              strFileName.c_str());
    return valueBuf;
}


Config::Config(const string& strFileName)
{

}

Config::~Config(void)
{

}

bool Config::UseJobConfig()
{
    char userName[1024];
    DWORD nameLength = 1024;
    BOOL ret = ::GetUserName(userName, &nameLength);
    if (ret)
    {
        string jobName = GetPrivateProfileValue(
            CONFIG_SET, "JobConfigName", "NotConfigName@#$%!##",
            CONFIG_INF_FILENAME_JOB)
        string congfigName = WinControlTool::GetInstance()->
            GetValueFromConfig(CONFIG_SET,
            "JobConfigName", "NotConfigName@#$%!##", CONFIG_INF_FILENAME);
        if (congfigName == userName)
        {
            WinDefine::GetInstance()->useJobConfig_ = true;
        }
    }
}



bool Config::Init()
{
    char szPath[2048] = { 0 };
    GetModuleFileName(NULL, szPath, sizeof(szPath));
    string strPath = StringPathHelper::RemoveOnelastPath(szPath);
    string strPathConfig = strPath + CONFIG_INF_FILENAME;


    string strFileNameAdd(strFileName);
    if (WinDefine::GetInstance()->useJobConfig_)
    {
        string ext;
        strFileNameAdd = RemoveLastExt(strFileNameAdd, &ext);
        strFileNameAdd = strFileNameAdd + JOB_EXT + ext;
    }

    string strFullPath = strPath + "\\" + strFileName;//szFileName;			
    FILE* fp = NULL;
    fopen_s(&fp, strFullPath.c_str(), "r");
    if (fp == NULL)
    {
        UINT iPos = strPath.rfind("\\");
        strPath.erase(iPos);
        strFullPath = strPath + "\\" + strFileName;//szFileName;		
    }
    else
        fclose(fp);
}

string Config::GetValue(const string& strAppName, 
                        const string& strKeyName, const string& strDefault)
{

}

int Config::GetValue(const string& strAppName,
                     const string& strKeyName, int defaultValue)
{

}

void Config::SetValue(const string& strAppName, 
                      const string& strKeyName, const string& value)
{

}

void Config::SetValue(const string& strAppName,
                      const string& strKeyName, int value)
{

}
