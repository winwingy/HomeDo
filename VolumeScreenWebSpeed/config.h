#ifndef _CONFIG_H_
#define _CONFIG_H_
#include <windows.h>
#include <string>
#include <map>
using std::string;

class Config
{
public:
    Config(const string& strFileName);
    ~Config(void);


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
    bool Init();
    std::map<std::string, std::string> cache_;
};
#endif