#ifndef __TASKMANGER_H
#define __TASKMANGER_H

#include "Public.h"

#define MAXTASKCNT		6
#define TASKNAMELEN		31

typedef struct
{
	u8 TaskName[TASKNAMELEN];
	u32 Tick;
	u32 UseTime;
	u32 MaxUseTime;
	u32 ExecInterval;
	u32 DelayTime;
	void(*TaskFunction)(void);
}_Task;

typedef struct
{
	u32 Flag;
	u32 Tick;
	u32 AllTaskUseTime;
	u32 MaxAllTaskUseTime;
	_Task Tasks[MAXTASKCNT];
}_TaskManger;

typedef struct
{
	u8 *TaskName;
	u8 id[4];
	u8 ExecTime[16];
	u8 DelayTime[16];
	u8 ExecInterval[16];
	u8 warn;
}_TaskInfo;

typedef struct
{
	u8 TaskCnt;
	_TaskInfo TaskInfos[MAXTASKCNT];
}_TaskList;

u32 CreateTask(u8 *TaskName,u16 delay,void(*taskFunction)(void));
void DelTask(u32 taskid);
void ExecTask(void);
void ShowTasks(void);
_TaskList* GetTaskList(void);

#endif
