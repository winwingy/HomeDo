#pragma once
class TaskDo;
class TaskDoMgr
{
public:
	static TaskDoMgr* instance();

	void addTaskDo(const std::shared_ptr<TaskDo>& taskDo);
	void removeTaskDo(const std::shared_ptr<TaskDo>& taskDo);
	bool beginLaunchTask(const tstring& strfullPath);
	bool afterLaunchTask(const tstring& strfullPath);

	bool beginExitTask(const tstring& strfullPath);
	bool afterExitTask(const tstring& strfullPath);

private:
	TaskDoMgr();
	~TaskDoMgr();
	static TaskDoMgr* pThis_;
	std::map<TaskDo*,std::shared_ptr<TaskDo>> taskDoList_;
	std::map<int, int> aa;
};

