#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <windows.h>
#include <string>
#include <map>
using std::string;

class Config
{
public:
	static Config* GetShared();

    string GetConfigPath();

    string GetValue(
        const string& strAppName, const string& strKeyName, 
        const string& strDefault);

    int GetValue(
        const string& strAppName, const string& strKeyName,
        int defaultValue);

    void SetValue(
        const string& strAppName, const string& strKeyName,
		const string& strValue);

    void SetValue(
        const string& strAppName, const string& strKeyName,
        int value);

    bool GetList(const string& listBegin, const string& listEnd, 
                 vector<string>* listText);

protected:  
	Config();
	~Config(void);
	bool Init(const string& strFileName, const string& fileNameJob);
	static Config* pThis_;
	string configPath_;

private:    
    std::map<std::string, std::string> cache_;    
};
#endif