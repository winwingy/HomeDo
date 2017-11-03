#include "stdafx.h"
#include <ShlObj.h>
#include "KugouTaskDo.h"
#include "../tool/StringPathHelper.h"
#include "../tool/config.h"
#include "../tool/file.h"

namespace
{
	class KugouConfig : public Config
	{
	public:
		static KugouConfig* GetShared()
		{
			if (!pThisKugou_)
			{
				pThisKugou_ = new KugouConfig;
				pThisKugou_->InitKugouPath();
			}
			return pThisKugou_;
		}

		bool InitKugouPath()
		{
			char buf[1024] = { 0 };
			BOOL ret = SHGetSpecialFolderPath(NULL, buf, CSIDL_APPDATA, FALSE);
			if (!ret)
				return false;

			string appPath(buf);
			appPath += "\\KuGou8\\KuGou.ini";
			if (!File::FileOrDirExist(appPath))
				return false;

			configPath_ = appPath;
			return true;
		}

	private:
		KugouConfig(){}
		~KugouConfig(){}
		static KugouConfig* pThisKugou_;
	};

	KugouConfig* KugouConfig::pThisKugou_ = nullptr;
}


KugouTaskDo::KugouTaskDo()
: TaskDo()
, isTaskOpen_(false)
{

}


KugouTaskDo::~KugouTaskDo()
{
}

bool KugouTaskDo::initTask()
{
	isTaskOpen_ = !!Config::GetShared()->GetValue("TaskDo", "KugouTask", 0);
	return isTaskOpen_;
}

bool KugouTaskDo::canDoTask(const tstring& strfullPath)
{	
	if (!isTaskOpen_)
		return false;

	if (strfullPath.find("KUGOU.EXE") != string::npos)
	{
		return true;
	}
	return false;
}

bool KugouTaskDo::beginLaunchTask()
{
	if (!isTaskOpen_)
		return false;

	unsigned int nowTime = static_cast<unsigned int>(time(NULL));
	KugouConfig::GetShared()->SetValue("Option", "PopDate", nowTime);
	return true;
}

bool KugouTaskDo::afterLaunchTask()
{
	if (!isTaskOpen_)
		return false;

	return true;

}
