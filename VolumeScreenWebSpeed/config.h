#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <windows.h>
#include <string>
#include <map>
using std::string;

class Config
{
public:
    Config();
    ~Config(void);

    bool Init(const string& strFileName, const string& fileNameJob);

    string GetValue(
        const string& strAppName, const string& strKeyName, 
        const string& strDefault);

    int GetValue(
        const string& strAppName, const string& strKeyName,
        int defaultValue);

    void SetValue(
        const string& strAppName, const string& strKeyName,
        const string& value);

    void SetValue(
        const string& strAppName, const string& strKeyName,
        int value);

private:
    std::map<std::string, std::string> cache_;
    string configPath_;
};
#endif