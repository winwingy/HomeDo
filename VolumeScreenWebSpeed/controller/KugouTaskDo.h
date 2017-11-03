#pragma once
#include "TaskDo.h"
class KugouTaskDo : public TaskDo
{
public:
	KugouTaskDo();
	~KugouTaskDo();

	virtual bool initTask() override;
	virtual bool canDoTask(const tstring& strfullPath) override;

	virtual bool beginLaunchTask() override;

	virtual bool afterLaunchTask() override;

private:
	bool isTaskOpen_;
};

