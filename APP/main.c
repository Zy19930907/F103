#include "Public.h"

u32 TOUCHTICK,LEDTICK,GUITICK,TESTTICK;

#define ID_FRAMEWIN_0 (GUI_ID_USER + 0x00)
#define ID_BUTTON_0 (GUI_ID_USER + 0x01)
#define ID_BUTTON_1 (GUI_ID_USER + 0x02)
#define ID_LISTVIEW (GUI_ID_USER + 0x03)
u32 listiewhandle;
u32 GuiTask;
u32 Com1Task;
u32 LedTask;
u32 CntTasks;
u32 CanTask;
u32 led0task;

//对话框资源表
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect, "Framewin", ID_FRAMEWIN_0, 0, 0, 480, 800, 0, 0x64, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_0, 80, 20, 150, 50, 0, 0x0, 0 },
	{ BUTTON_CreateIndirect, "Button", ID_BUTTON_1, 235, 20, 150, 50, 0, 0x0, 0 },
	{ LISTVIEW_CreateIndirect,"MULTIEDIT",ID_LISTVIEW,30,85,420,300,0,0,0}
};

void guitask(void)
{
	GUI_TOUCH_Exec();
	WM_Exec();
}

void ledtask(void)
{
	LED1CHANGE;
}

void led0test(void)
{
	LED0CHANGE;
}

void showtasks(void)
{
	u8 i,j;
	_TaskInfo* info;
	_TaskList* list = GetTaskList();
	j = LISTVIEW_GetNumRows(listiewhandle);
	for(i=0;i<list->TaskCnt;i++)
	{
		info = &list->TaskInfos[i];
		LISTVIEW_SetItemText(listiewhandle,0,i,(char*)info->id);
		LISTVIEW_SetItemText(listiewhandle,1,i,(char*)info->TaskName);
		LISTVIEW_SetItemText(listiewhandle,2,i,(char*)info->DelayTime);
		LISTVIEW_SetItemText(listiewhandle,3,i,(char*)info->ExecInterval);
		LISTVIEW_SetItemText(listiewhandle,4,i,(char*)info->ExecTime);
	}
	for(;i<j;i++)
	{
		LISTVIEW_SetItemText(listiewhandle,0,i,"");
		LISTVIEW_SetItemText(listiewhandle,1,i,"");
		LISTVIEW_SetItemText(listiewhandle,2,i,"");
		LISTVIEW_SetItemText(listiewhandle,3,i,"");
		LISTVIEW_SetItemText(listiewhandle,4,i,"");
	}
}

//对话框回调函数
static void _cbDialog(WM_MESSAGE * pMsg) 
{
	WM_HWIN hItem;
	int     NCode;
	int     Id;
	u8 i;
	switch (pMsg->MsgId) 
	{
		case WM_INIT_DIALOG:
			//初始化对话框
			hItem = pMsg->hWin;
			FRAMEWIN_SetTitleHeight(hItem, 30);
			FRAMEWIN_SetText(hItem, "Hello MyOS");
			FRAMEWIN_SetFont(hItem, &GUI_FontHZ24);
			FRAMEWIN_SetTextAlign(hItem, GUI_TA_HCENTER | GUI_TA_VCENTER);
			FRAMEWIN_SetTextColor(hItem, 0x0000FFFF);

			//初始化BUTTON0
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_0);
			BUTTON_SetFont(hItem, &GUI_FontHZ16);
			BUTTON_SetText(hItem, "删除LED1任务");
			
			//初始化BUTTON1
			hItem = WM_GetDialogItem(pMsg->hWin, ID_BUTTON_1);
			BUTTON_SetText(hItem, "创建LED1任务");
			BUTTON_SetFont(hItem, &GUI_FontHZ16);
		
			hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW);
			LISTVIEW_SetFont(hItem,&GUI_FontHZ12);
			LISTVIEW_AddColumn(hItem,30,"ID",GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem,120,"NAME",GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem,60,"DELAY",GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem,60,"ITVAL",GUI_TA_HCENTER | GUI_TA_VCENTER);
			LISTVIEW_AddColumn(hItem,60,"USETIME",GUI_TA_HCENTER | GUI_TA_VCENTER);
			
			LISTVIEW_SetBkColor(hItem,LISTVIEW_CI_UNSEL,GUI_GRAY);
			LISTVIEW_SetGridVis(hItem,1);
			for(i=0;i<MAXTASKCNT;i++)
				LISTVIEW_AddRow(hItem,0);
			listiewhandle = hItem;
			break;
		case WM_NOTIFY_PARENT:
						Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			switch(Id) 
			{
				case ID_BUTTON_0: //BUTTON_0的通知代码，控制LED1
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							DelTask(LedTask);
							break;
						case WM_NOTIFICATION_RELEASED: //按钮被按下并释放
							
							break;	
					}
					break;
				case ID_BUTTON_1: //BUTTON_1的通知代码，控制BEEP
					switch(NCode) 
					{
						case WM_NOTIFICATION_CLICKED:
							LedTask = CreateTask((u8*)"LedTask",500,ledtask);
							break;
						case WM_NOTIFICATION_RELEASED:
							
							break;
					}
					break;
			}
			break;
		default:
			WM_DefaultProc(pMsg);
			break;
	}
}

int main(void)
{
	Stm32_Clock_Init(9);//时钟72MHz
	SysTickInit();//系统时基初始化
	RCC->AHBENR |= (1<<6);//使能CRC时钟
	LED_Init();
	TFTLCD_Init();
	
	my_mem_init(SRAMIN);		//初始化内部内存池
	exfuns_init();				//FATFS 申请内存
	f_mount(fs[0],"0:",1); 		//挂载SD卡  
  	f_mount(fs[1],"1:",1); 		//挂载挂载FLASH.
	font_init();	
	
	TP_Init();				//触摸屏初始化
	BUTTON_SetDefaultSkin(BUTTON_SKIN_FLEX);
	GUI_Init();	  
	GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), _cbDialog, WM_HBKWIN, 0, 0);
	
	LedTask = CreateTask((u8*)"运行指示灯",100,ledtask);
	led0task = CreateTask((u8*)"LED0测试",100,led0test);
	Com1Task = CreateTask((u8*)"串口数据接收",0,COM1Task);
	GuiTask = CreateTask((u8*)"界面刷新",5,guitask);
	CanTask = CreateTask((u8*)"CAN数据接收",0,CANTask);
	CntTasks = CreateTask((u8*)"任务信息显示",1000,showtasks);
	
	for(;;)
	{
		ExecTask();
	}
}
