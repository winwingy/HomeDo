#ifndef _FILE_H_
#define _FILE_H_

class File
{
public:
    static INT64 GetFileSize(const string& path);
    static bool FileOrDirExist(const string& path);
    File(const string& path, const string& mode);
    ~File();
    bool IsValid();
    int Read(char* buf, int size, int count);  
    int Write(const char* buf, int size, int count);
    INT64 GetFileSize();
    INT64 ftell();
    int fseek(INT64 nowPos, int origin);

private:
    FILE* fp_;
};
#endif