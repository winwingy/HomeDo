#include "stdafx.h"
#include "TaskDoMgr.h"
#include "TaskDo.h"

TaskDoMgr* TaskDoMgr::pThis_ = nullptr;

TaskDoMgr* TaskDoMgr::instance()
{
	if (!pThis_)
	{
		pThis_ = new TaskDoMgr;
	}
	return pThis_;
}


TaskDoMgr::TaskDoMgr()
: taskDoList_()
{
}


TaskDoMgr::~TaskDoMgr()
{
}

void TaskDoMgr::addTaskDo(const std::shared_ptr<TaskDo>& taskDo)
{
	assert(taskDo.get());
	if (taskDoList_.find(taskDo.get()) == taskDoList_.end())
	{
		taskDoList_.insert(std::make_pair(taskDo.get(), taskDo));
	}
	else
	{
		assert(0 && "repeat add!");
	}	
}

void TaskDoMgr::removeTaskDo(const std::shared_ptr<TaskDo>& taskDo)
{
	assert(taskDo.get());
	if (taskDoList_.find(taskDo.get()) == taskDoList_.end())
	{
		assert(0 && "not find!");
		
	}
	else
	{
		taskDoList_.erase(taskDo.get());
	}
}

bool TaskDoMgr::beginLaunchTask(const tstring& strfullPath)
{
	for (auto iter = taskDoList_.begin(); iter != taskDoList_.end(); ++iter)
	{
		if (iter->second->canDoTask(strfullPath))
		{
			iter->second->beginLaunchTask();
		}
	}
	return true;
}

bool TaskDoMgr::afterLaunchTask(const tstring& strfullPath)
{
	for (auto iter = taskDoList_.begin(); iter != taskDoList_.end(); ++iter)
	{
		if (iter->second->canDoTask(strfullPath))
		{
			iter->second->afterLaunchTask();
		}
	}
	return true;
}

bool TaskDoMgr::beginExitTask(const tstring& strfullPath)
{
	return true;
}

bool TaskDoMgr::afterExitTask(const tstring& strfullPath)
{
	return true;
}
