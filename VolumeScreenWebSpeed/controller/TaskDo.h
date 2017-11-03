#pragma once
class TaskDo
{
public:
	TaskDo();
	~TaskDo();

	virtual bool initTask(){ return true; };

	virtual bool canDoTask(const tstring& strfullPath){ return false; }

	virtual bool beginLaunchTask(){ return true; }

	virtual bool afterLaunchTask(){ return true; }

	virtual bool beginExitTask(){ return true; }

	virtual bool afterExitTask(){ return true; }
};

