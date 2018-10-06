#include "TaskManger.h"
#include <string.h>

_TaskManger TaskManger;
_TaskList TaskList;

char *itoa(u32 val, char *buf, unsigned radix)
{
	char *p;
	char *firstdig;
	char temp;               
	unsigned   digval;         
	p = buf;     
	firstdig = p;     
	do{        
		digval = (unsigned)(val % radix);        
		val /= radix;               
		if (digval > 9)            
			*p++ = (char)(digval - 10 + 'a');         
		else            
			*p++ = (char)(digval + '0');          
	}while(val > 0);       
	*p-- = '\0';             
	do{        
		temp = *p;        
		*p = *firstdig;        
		*firstdig = temp;        
		--p;        
		++firstdig;            
	}while(firstdig < p);      
	return buf;
}

u32 CreateTask(u8 *TaskName,u16 delay,void(*taskFunction)(void))
{
	u8 i,j=0;
	for(i=0;i<MAXTASKCNT;i++)
	{
		//任务已创建
		if(TaskManger.Tasks[i].TaskFunction == taskFunction)
			return i;
		//任务格子已占用
		if((TaskManger.Flag >> i) & 0x01)
			continue;
		
		for(j=0;j<TASKNAMELEN;j++)
		{
			TaskManger.Tasks[i].TaskName[j] = '\0';
		}
		
		strcpy((char*)TaskManger.Tasks[i].TaskName,(char*)TaskName);
//		j = 0;
//		do{
//			TaskManger.Tasks[i].TaskName[j] = *TaskName++;
//			j++;
//		}while(*TaskName != '\0');
		
		TaskManger.Tasks[i].DelayTime = delay;
		TaskManger.Tasks[i].MaxUseTime = 0;
		TaskManger.Tasks[i].Tick = 0;
		TaskManger.Tasks[i].ExecInterval = 0;
		TaskManger.Tasks[i].UseTime = 0;
		TaskManger.Tasks[i].TaskFunction = taskFunction;
		TaskManger.Flag |= (1<<i);
		
		return i;
	}
	return 0xFFFFFFFF;
}

//删除任务
void DelTask(u32 taskid)
{
	TaskManger.Flag &= ~(1 << taskid);
	TaskManger.Tasks[taskid].DelayTime = 0;
	TaskManger.Tasks[taskid].MaxUseTime = 0;
	TaskManger.Tasks[taskid].Tick = 0;
	TaskManger.Tasks[taskid].ExecInterval = 0;
	TaskManger.Tasks[taskid].UseTime = 0;
	TaskManger.Tasks[taskid].TaskFunction = 0;
}


//统计当前执行的所有任务
_TaskList* GetTaskList(void)
{
	u8 i,j=0;
	_Task* Task;
	_TaskInfo* info;
	TaskList.TaskCnt = 0;
	
	for(i=0;i<MAXTASKCNT;i++)
	{
		if((TaskManger.Flag >> i) & 0x01)
		{
			TaskList.TaskCnt++;
			info = &TaskList.TaskInfos[j];
			Task = &TaskManger.Tasks[i];
			info->TaskName = Task->TaskName;
			itoa(j+1,(char*)info->id,10);
			itoa(Task->DelayTime,(char*)info->DelayTime,10);
			itoa(Task->UseTime,(char*)info->ExecTime,10);
			itoa(Task->ExecInterval,(char*)info->ExecInterval,10);
			if(Task->UseTime >= 1)
				info->warn = 1;
			else
				info->warn = 0;
			j++;
		}
	}
	return &TaskList;
}

//执行任务列表中的任务
void ExecTask(void)
{
	u8 i;
	_Task* Task;
	TaskManger.Tick = SYS_TICK;
	for(i=0;i<MAXTASKCNT;i++)
	{
		if(!((TaskManger.Flag >> i) & 0x01))
			continue;
		
		Task = &TaskManger.Tasks[i];
		if(MsTickDiff(Task->Tick) >= Task->DelayTime)
		{
			Task->ExecInterval = MsTickDiff(Task->Tick);//计算上次执行任务到本次执行任务的时间间隔
			Task->Tick = SYS_TICK;
			Task->TaskFunction();//执行任务函数
			Task->UseTime = MsTickDiff(Task->Tick);//计算本次执行任务用时
			Task->Tick = SYS_TICK;
		}
	}
	TaskManger.AllTaskUseTime = MsTickDiff(TaskManger.Tick);
}
