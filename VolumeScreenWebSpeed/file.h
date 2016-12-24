#ifndef _FILE_H_
#define _FILE_H_

class File
{
public:
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

    vector<string> GetList(const string& listBegin, const string& listEnd);

private:
    File();
    ~File(void);
    bool Init(const string& strFileName, const string& fileNameJob);

    static File* pThis_;
    std::map<std::string, std::string> cache_;
    string configPath_;
};
#endif