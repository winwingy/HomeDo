#include "stdafx.h"
#include "file.h"
#include <sys\stat.h>
#include <io.h>

bool File::FileOrDirExist(const string& path)
{
    return (_access(path.c_str(), 0) == 0);
}

File::File(const string& path, const string& mode)
{
    fp_ = nullptr;
    fopen_s(&fp_, path.c_str(), mode.c_str());
}

File::~File()
{
    if (fp_)
    {
        fclose(fp_);
        fp_ = nullptr;
    }
}

bool File::IsValid()
{
    return !!fp_;
}

int File::Read(char* buf, int size, int count)
{
    return fread(buf, size, count, fp_);
}

int File::Write(const char* buf, int size, int count)
{
    return ::fwrite(buf, size, count, fp_);
}

INT64 File::GetFileSize(const string& path)
{
    struct _stat fileStruct = { 0 };
    int res = _stat(path.c_str(), &fileStruct);
    return fileStruct.st_size;
}

INT64 File::GetFileSize()
{
    INT64 nowPos = ftell();
    fseek(0, SEEK_END);
    INT64 size = ftell();
    fseek(nowPos, SEEK_SET);
    return size;
}

int File::fseek(INT64 nowPos, int origin)
{
    return ::fseek(fp_, static_cast<int>(nowPos), origin);
}

INT64 File::ftell()
{
    return ::ftell(fp_);
}
