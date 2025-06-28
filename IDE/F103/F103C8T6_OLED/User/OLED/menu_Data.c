#include "menu.h" 
#include "Delay.h"
#include "MyRTC.h"
#include <string.h>
#include <math.h>
#include "AD.h"
#include "voltage_mea.h"
#include "W25Q128_SPI1.h"
#include "OLED.h"
extern uint8_t GUODUsn;

#include "MP3_tf.h"
#include "Serial2.h"
#include "Serial3.h"



uint8_t mp3lou=1;

void mp3loudspeaker(void)
{
	   if( mp3lou)	GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_RESET);
	
       else GPIO_WriteBit(GPIOA, GPIO_Pin_12, Bit_SET);
}


void mp3_volume(void)
{
	MP3CMD(  0x06 , 0x00 ,  mp3volume );  //设置音量
}

void mp3up(void)
{	
	if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0){
		GUODUsn=1;
		OLED_Clear();
		OLED_ShowString(0,13,"请开启MP3电源",OLED_6X8);
		OLED_ShowString(0,26,"<--",OLED_6X8);
		OLED_Update();
		while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_11)==0)	
		{	
			OLED_transition(1);
			if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1)==1){
				GUODUsn=1;
				OLED_transition(0);
				return;
			}
		}
	}
	
	MP3_Init();
	
      MP3os();
	
		struct option_class2 option_list[] = {
		{"退出",Function,0},                  //{选项名，需要运行的函数}
		{"返回界面",Function, MP3os},
		{"扬声器",ON_OFF, mp3loudspeaker,&mp3lou},
		{"音量",Number,mp3_volume,&mp3volume},
		{"单曲循环",ON_OFF, RETURN ,&mp3while},
		{"播完即停",ON_OFF, RETURN ,&mp3PLSD},
		{"歌曲数目",Number, RETURN ,&mp3chapterMAX},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);
	
	MP3_Power_OFF();
}


uint8_t chongdian = 0xF1;

void chongdianzhouqi(void)
{
	
	GUODUsn=1;
	int8_t p;
	while(1)
	{
		
		
		  OLED_ShowHexNum(100, 20, chongdian, 4, OLED_6X8);
		  OLED_Update();	

		
		
		OLED_transition(1);

		p = menu_Roll_event();   
	   if( p == 1){ chongdian++;
		OLED_WriteCommand(0xD9);	//设置
		OLED_WriteCommand(chongdian);	//0x00~0xFF
	   }	 
       else if( p == -1){ chongdian--;
		OLED_WriteCommand(0xD9);	//设置
		OLED_WriteCommand(chongdian);	//0x00~0xFF
	   }	
		p=menu_Enter_event();
		if(p)
		{
			GUODUsn=1;
			OLED_transition(0);
			return;
		}	
	}
}

uint8_t fenpin = 0xFE;


void fenpinbi(void)
{
	
	GUODUsn=1;
	int8_t p;
	while(1)
	{
		
		
		  OLED_ShowHexNum(100, 20, fenpin, 4, OLED_6X8);
		  OLED_Update();	

		
		
		OLED_transition(1);

		p = menu_Roll_event();   
	   if( p == 1){ 	   fenpin++;
		OLED_WriteCommand(0xD5);	//设置
		OLED_WriteCommand( fenpin);	//0x00~0xFF
	   }	 
       else if( p == -1){  fenpin--;
		OLED_WriteCommand(0xD5);	//设
		OLED_WriteCommand( fenpin);	//0x00~0xFF
	   }	
		p=menu_Enter_event();
		if(p)
		{
			GUODUsn=1;
			OLED_transition(0);
			return;
		}	
	}
}


extern float RTI,VIN;

void System_voltage(void)
{
	
	GUODUsn=1;
		int8_t p;
		uint16_t ADValue , ADrefint ;
	float voltage;
	while(1)
	{
	
		ADValue = AD_GetValue(ADC_Channel_8);
		ADrefint = AD_GetValue(ADC_Channel_17);
	

	  OLED_Clear();		
		
		OLED_ShowFloatNum(5, 2, VIN, 2, 3, OLED_6X8);
		
		OLED_ShowNum(5, 15, ADrefint, 4, OLED_6X8);
		
		OLED_ShowNum(5, 25, ADValue, 4, OLED_6X8);
		
		voltage = ((float)ADValue * VIN ) / 4095  * 2 ;
		
		OLED_ShowFloatNum(64, 25, voltage, 2, 2, OLED_6X8);
		
		voltage = ((float)ADrefint * VIN ) / 4095;//得到电压

		
		OLED_ShowFloatNum(64, 2, voltage, 2, 2, OLED_6X8);

		
		ADValue = AD_GetValue(ADC_Channel_9);
		
	    float voltage = ((float)ADValue * VIN ) / 4095 ;

		voltage = voltage*RTI;
		
		OLED_ShowNum(5, 35, ADValue, 4, OLED_6X8);
		OLED_ShowFloatNum(64, 35, voltage , 2,2, OLED_6X8);
		
		OLED_Update();	

		
		OLED_transition(1);

		
		p = menu_Roll_event();   
	   if( p == 1){ VIN=VIN+0.001;
	   }	 
       else if( p == -1){  VIN=VIN-0.001;
	   }	
		p=menu_Enter_event();
		if ( p ){
			GUODUsn=1;
			OLED_transition(0);
			return;
		}	
	}
}

//这里是菜单子项内容
void System_information(void)
{
	struct option_class2 option_list[] = {
		{"- 返回",Function,0},
		{"-----STM32C8T6-----",Display},                  //{选项名，需要运行的函数}
		{"- RAM: 20K",Display},
		{"- FLASH: 64K",Display},
		{"------W25Q128------",Display},
		{"- 储存:16MB",Display},
		{"-------------------",Display},
		{"- 软件版本V1.15",Display},
		{"- BiliBili 时光久泽",Display},
		{"- 系统电压",Function,System_voltage},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);

	
}


#include "inv_mpu.h"            //mpu6050陀螺仪传感器
#include "MPU6050.h"
extern float Pitch,Roll,Yaw;          //定义3个储存角度的变量
extern short gyroX,gyroY,gyroZ,accelX,accelY,accelZ; 
extern uint8_t TLYPW , taiwan ; 

void Detection_angle(void)
{
	if(taiwan)
	{	
		GUODUsn=1;
		OLED_Clear();
		OLED_ShowString(0, 52, "已被抬腕亮屏占用", OLED_6X8);
		OLED_Update();
		while(GUODUsn){OLED_transition(1);}
		delay_ms(500);
		GUODUsn=1;
		OLED_transition(0);
		return;
	}
	
		int8_t p ,k=1;
		GUODUsn=1;
			
		short up=0;
	
	
	    if(!TLYPW)
		{
			
			MPU6050_DMP_Init();//陀螺仪初始化
	
		}
		TLYPW=1;
	
		OLED_Clear();
	
		OLED_ShowASCII(0, 0, "Pitch:", OLED_6X8);
		OLED_ShowASCII(0, 9," Roll:", OLED_6X8);
		OLED_ShowASCII(0, 18," Yaw :", OLED_6X8);
		OLED_ShowASCII(0, 27, "GX:", OLED_6X8);
		OLED_ShowASCII(0, 36,"GY:", OLED_6X8);
		OLED_ShowASCII(0, 45,"GZ:", OLED_6X8);
		OLED_ShowASCII(0, 54,"AX:", OLED_6X8);
		OLED_ShowASCII(64, 45,"AY:", OLED_6X8);
		OLED_ShowASCII(64, 54,"AZ:", OLED_6X8);
	
		OLED_Update();
	
	while(1)
	{
		//直接把陀螺仪数据全部掏出来
		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw,&gyroX,&gyroY,&gyroZ,&accelX,&accelY,&accelZ);	

		if(k>0)
		{
			OLED_ShowFloatNum(36, 0, Pitch, 3, 1, OLED_6X8);
			OLED_ShowFloatNum(36, 9, Roll, 3, 1, OLED_6X8);
			OLED_ShowFloatNum(36, 18, Yaw, 3, 1, OLED_6X8);
			OLED_UpdateArea(36, 0, 36, 27);
		}
		if(k>1)
		{	if(gyroX>up) up=gyroX;			
			OLED_ShowSignedNum(18, 27, (int32_t)up, 5, OLED_6X8);		
			OLED_ShowSignedNum(18, 36, (int32_t)gyroY, 5, OLED_6X8);
			OLED_ShowSignedNum(18, 45, (int32_t)gyroZ, 5, OLED_6X8);
			OLED_UpdateArea(18, 27, 36, 36);
		}
		if(k>2)
		{
			
			OLED_ShowSignedNum(18, 54, (int32_t)accelX, 5, OLED_6X8);		
			OLED_ShowSignedNum(82, 45, (int32_t)accelY, 5, OLED_6X8);
			OLED_ShowSignedNum(82, 54, (int32_t)accelZ, 5, OLED_6X8);
			OLED_UpdateArea(82, 45, 36, 18);
		}
		
		OLED_ClearArea(82, 0, 46, 45);
		OLED_DrawRectangle(87,0,41,41,OLED_UNFILLED);
		OLED_DrawLine(108,0,108,40);
		OLED_DrawLine(87,20,127,20);
		OLED_DrawRectangle(107+Pitch/2,19+Roll/2,3,3,OLED_FILLED);		
		OLED_UpdateArea(87, 0, 41, 41);
		
		
//		OLED_Update();
//		OLED_Clear();

		OLED_transition(1);
	   p = menu_Roll_event();   
	   if( p == 1){ 
			k++;

	   }	 
       else if( p == -1){ 
			k=0;

	   }	
		p=menu_Enter_event();
		if(p==1){

			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}
		else if ( p==2 ){
		
			mpu6050_write_reg(PWR_MGMT_1, 0X80);	//复位MPU6050
			TLYPW=0;
			
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	

	}
}


extern uint8_t OLED_DisplayBuf[8][128];		//拿来吧你

uint8_t Nodes_Cache[8][128];				//计算缓存

void Update_Display(void)					//上传屏幕
{
	memcpy(OLED_DisplayBuf, Nodes_Cache, 1024);
}

void Point_life(uint8_t X, uint8_t Y)		//写点生
{
	Nodes_Cache[(Y/8)][X] |= (0x01 << (Y%8));
}

void Point_death(uint8_t X, uint8_t Y)		//写点死
{
	Nodes_Cache[(Y/8)][X] &= ~(0x01 << (Y%8));
}

uint8_t CountPointRound(uint8_t X, uint8_t Y)		//统计点周围细胞数量
{
	return ( 
	OLED_GetPoint(X-1, Y-1) + 	OLED_GetPoint(X  , Y-1) + 	OLED_GetPoint(X+1, Y-1) + 
	OLED_GetPoint(X-1, Y  ) + 								OLED_GetPoint(X+1, Y  ) + 
	OLED_GetPoint(X-1, Y+1) + 	OLED_GetPoint(X  , Y+1) + 	OLED_GetPoint(X+1, Y+1)
	);
}

void OLED_Rotation_C_P(int8_t CX, int8_t CY, float* PX, float* PY, int16_t Angle)//旋转点
{
	float Theta = (3.14 / 180) * Angle;
	float Xd = *PX - CX;
	float Yd = *PY - CY;
	
	*PX = (Xd) * cos(Theta) - (Yd) * sin(Theta) + CX;// + 0.5;
	*PY = (Xd) * sin(Theta) + (Yd) * cos(Theta) + CY;// + 0.5;
}

void Game_Of_Life_Turn(void)		//刷新游戏回合
{
	uint8_t x, y;
	uint8_t Count;
	
	for(y = 0; y < 64; y ++)
	{
		for(x = 0; x < 128; x ++)
		{
			Count = CountPointRound(x, y);
			if(OLED_GetPoint(x, y))
			{
				if(Count < 2 || Count > 3)
				{
					Point_death(x, y);
				}
			}
			else
			{
				if(Count == 3)
				{
					Point_life(x, y);
				}
			}
		}
	}	
	Update_Display();
}

void Game_Of_Life_Seed(int16_t seed)		//游戏初始化种子
{
	srand(seed);
	for(uint8_t i = 0; i < 8; i ++)
		for(uint8_t j = 0; j < 128; j ++)
	{
		Nodes_Cache[i][j] = rand();
	}
	Update_Display();
}

void Game_Of_Life_Play(void)		//游戏开始
{
	GUODUsn=1;
	
	uint8_t x1 = 8, x2 = 16, y1 = 32, y2 = 32;
	int8_t shift = -1;
	
	
	OLED_Clear();
	uint8_t i, j;
	for (j = 0; j < 8; j ++)				//遍历8页
	{
		for (i = 0; i < 128; i ++)			//遍历128列
		{
			Nodes_Cache[j][i] = 0x00;	//将显存数组数据全部清零
		}
	}

	
//	Game_Of_Life_Seed(1);		//放置种子
	
	int8_t p;
	while(1)
	{
		Game_Of_Life_Turn();
		
		if(shift > 0) {y2 += menu_Roll_event()*8;}
		else {x2 += menu_Roll_event()*8;}
		x2 %= 128;
		y2 %= 64;
		OLED_DrawLine(x1, y1, x2, y2);
		if((x2 - x1) > 1) {x1 += (x2 - x1) / 8 + 1;}
		else if((x2 - x1) < -1) {x1 += (x2 - x1) / 8 - 1;}
		else {x1 = x2;}
		if((y2 - y1) > 1) {y1 += (y2 - y1) / 2 +1;}
		else if((y2 - y1) < -1) {y1 += (y2 - y1) / 2 - 1;}
		else{y1 = y2;}
		
//		OLED_Rotation_C_P(64, 32, &px, &py, Angle);
//		OLED_DrawLine(64, 32, px+0.5, py+0.5);
		
		OLED_Update();
		OLED_transition(1);

		//Delay_ms(100);
		
		p=menu_Enter_event();
		if(p==1) {shift = -shift;}	
		else if ( p==2 ){
			GUODUsn=1;
			OLED_transition(0);
			return;
		}	
	}
}


void Power_OFF(void)
{
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_SET);
	
	
	OLED_Clear();				//清空显存数组
	OLED_Update();				//更新显示，清屏
	
	OLED_WriteCommand(0xAE);	//0xAF开启显示，0xAE为休眠
	
	OLED_WriteCommand(0xAD);	//设置充电泵
	OLED_WriteCommand(0x8A);    //0x8B为开启，0x8A为关闭充电泵
	
		/*使能WKUP引脚*/
//	PWR_WakeUpPinCmd(ENABLE);						//使能位于PA0的WKUP引脚，WKUP引脚上升沿唤醒待机模式
//	PWR_EnterSTANDBYMode();						//STM32进入停止模式，并等待指定的唤醒事件（WKUP上升沿或RTC闹钟）
	
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
	SystemInit();


	
	OLED_WriteCommand(0xAD);	//设置充电泵
	OLED_WriteCommand(0x8B);    //0x8B为开启，0x8A为关闭充电泵
	
	OLED_WriteCommand(0xAF);	//0xAF开启显示，0xAE为休眠
	
	GPIO_WriteBit(GPIOB, GPIO_Pin_5, Bit_RESET);
	
}


void error(void)
{
	int8_t p;
	GUODUsn=1;
	
		OLED_Clear();	
		OLED_ShowString(39, 16, "待开发中", OLED_6X8);
		OLED_ShowString(0, 52, "当然你也可以自己来写", OLED_6X8);
		OLED_Update();
	while(1)
	{

		
		
       OLED_transition(1);
	   p = menu_Roll_event();   
	   if(p){ 

			GUODUsn=1;
			OLED_transition(0);
			return;
	   }	 
	   p = menu_Enter_event();
	   if(p){			
			GUODUsn=1;
			OLED_transition(0);
			return;
	   }
	}
}
//	/**********************************************************/

uint8_t liangdu = 0xFF;

void particulars(void)
{
		OLED_WriteCommand(0x81);	//设置对比度
		OLED_WriteCommand(liangdu);	//0x00~0xFF
}

uint8_t fanse = 0 ;

void OLEDfanse(void)
{
	if(fanse)	
	{
		OLED_WriteCommand(0xA7);
	}
	else
	{
		OLED_WriteCommand(0xA6);
	}
}

void shoudt(void)
{
	int8_t p;
	GUODUsn=1;
	
	OLED_Clear();	
	OLED_DrawRectangle(0, 0, 128, 64, OLED_FILLED);
    OLED_Update();	
	
	while(1)
	{
		
		while(GUODUsn) {
			OLED_Update();	
			OLED_transition(1);}
       
			
		OLED_WriteCommand(0x81);	//设置对比度
		OLED_WriteCommand(0xFF);	//0x00~0xFF

		p=menu_Roll_event();
		if(p){
			
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}
		p=menu_Enter_event();
		if(p){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}

}


extern uint8_t Cursor;

void System_Cursor1(void){Cursor=0;}
void System_Cursor2(void){Cursor=1;}
void System_Cursor3(void){Cursor=2;}


void System_Cursor(void)
{
	struct option_class2 option_list[] = {
		{"- 返回",1,0},               
		{"- 反色",1, System_Cursor1},                  //{选项名，需要运行的函数}
		{"- 矩形",1, System_Cursor2},
		{"- 指针",1, System_Cursor3},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);
}

uint8_t taiwan=0;

void TAIWAN(void)
{
	if(taiwan)
	{
		OLED_Clear();	
		OLED_ShowString(39, 16, "正在配置", OLED_6X8);
		OLED_Update();
		GUODUsn=1;
		
		while(GUODUsn){
			OLED_transition(1);
		}		
		if(!TLYPW)
		{			
			MPU6050_DMP_Init();//陀螺仪初始化	
		}
			TLYPW=1;
		
			return;
	}
	else
	{	
			mpu6050_write_reg(PWR_MGMT_1, 0X80);	//复位MPU6050
			TLYPW=0;
			
			return;
	}
		
}

#include "inv_mpu_dmp_motion_driver.h"
void TAIWANLIANGP(void)
{
	OLED_Clear();				//清空显存数组
	OLED_Update();				//更新显示，清屏
	
	OLED_WriteCommand(0xAE);	//0xAF开启显示，0xAE为休眠
	
	OLED_WriteCommand(0xAD);	//设置充电泵
	OLED_WriteCommand(0x8A);    //0x8B为开启，0x8A为关闭充电泵
	
	gyroX=0;
	
	short gyro[3];
	unsigned long timestamp ;

	
	while( gyroX < 12000 )
	{
//		MPU6050_DMP_Get_Data(&Pitch,&Roll,&Yaw,&gyroX,&gyroY,&gyroZ,&accelX,&accelY,&accelZ);
		
		mpu_get_gyro_reg(gyro,&timestamp);
		gyroX = gyro[0];
		
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)==1)break;		
	}
	

	OLED_WriteCommand(0xAD);	//设置充电泵
	OLED_WriteCommand(0x8B);    //0x8B为开启，0x8A为关闭充电泵
	
	OLED_WriteCommand(0xAF);	//0xAF开启显示，0xAE为休眠

}

uint8_t dgh=1;
void DGHmode(void)
{
	if(dgh)
	{
		OLED_Clear();	
		OLED_ShowString(0, 16, "仅在抬腕亮屏关闭时有效", OLED_6X8);
		OLED_Update();
		GUODUsn=1;
		while(GUODUsn){
			OLED_transition(1);
		}		
		delay_ms(500);
		return;
	}
}



void System_settings(void)
{
	struct option_class2 option_list[] = {
		{"- 返回",Function,0},
		{"- 屏幕亮度",Number, particulars , &liangdu},                  //{选项名，需要运行的函数}
		{"- 光标样式",Function, System_Cursor},
		{"- 反色显示",ON_OFF , OLEDfanse , &fanse},
		{"- 信息",Function,System_information},
		{"- 预充电周期",Function,chongdianzhouqi},
		{"- 显示时钟分频比",Function,fenpinbi},
		{"- 抬腕亮屏",ON_OFF , TAIWAN ,&taiwan},
		{"- 低功耗模式",ON_OFF , DGHmode  ,&dgh},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);
}


void skmwx(void)
{
	int8_t p;
	GUODUsn=1;
	
		OLED_Clear();	
		OLED_ShowImage(32, 0, 64, 64 , bug);
		OLED_Update();	
	
	while(1)
	{
		
       OLED_transition(1);
		p=menu_Enter_event();
		if(p==1){
			
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}

void skmzfb(void)
{
	int8_t p;
	GUODUsn=1;
	
		OLED_Clear();	
		OLED_ShowImage(32, 0, 64, 64 , bug);
		OLED_Update();	
	
	while(1)
	{
				
       OLED_transition(1);
		p=menu_Enter_event();
		if(p==1){
			
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}




void dibug(void)
{
	int8_t p ,i=1 ,x=20,y=20;
	GUODUsn=1;
	
	
	while(1)
	{
		
		
		OLED_Clear();	
		OLED_ShowImage(x, y, 32, 32 , bug);
		OLED_Update();	

	




		
       OLED_transition(1);
	   p = menu_Roll_event();   
	   if( p == 1){ 
		   
		if(i) x++;
		else  y++;
				
				
	   }	 
       else if( p == -1){ 
		if(i) x--;
		else  y--;

	   }	
		p=menu_Enter_event();
		if(p==1){
			
			i = !i;
			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}


void cosjiaod(void)
{
	int8_t p,i=0;
	GUODUsn=1;
	float cos1=0,tpp,tpp1;	
		OLED_Clear();
	while(1)
	{
		
		tpp=asin(cos1);
		tpp1=acos(cos1);
		
		OLED_ShowFloatNum(10, 10, cos1, 1, 3, OLED_6X8);
		OLED_ShowFloatNum(10, 30, tpp/3.141592*180, 2, 9, OLED_6X8);
		OLED_ShowFloatNum(10, 40, tpp1/3.141592*180, 2, 9, OLED_6X8);
		OLED_Update();	




		
       OLED_transition(1);
	   p = menu_Roll_event();   
	   if( p == 1){
			if(i)  cos1+=0.01;
		    else	cos1+=0.001;


	   }	 
       else if( p == -1){ 
		   
			if(i)  cos1-=0.01;
		    else	cos1-=0.001;

	   }	
		p=menu_Enter_event();
		if(p==1){
			
			i=!i;			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}


void W25Q128Flash(void)
{
	int8_t p;
	GUODUsn=1;
	OLED_Clear();
	
	uint8_t SChinese[24];
	uint32_t Addr_offset=0;//汉字的偏移地址
    uint16_t dld=0;


	while(1)
	{
		
	    OLED_Clear();
		for(u8 i=0;i<4;i++)
		{
			for(u8 o=0;o<10;o++)
			{
				W25Q128_ReadData( Addr_offset , SChinese , 24 );			
				OLED_ShowImage(o*12, i*12, 12, 12, SChinese);
				OLED_UpdateArea(o*12,i*12,12,12);  
				Addr_offset+=24;
				OLED_ShowHexNum(0,48,Addr_offset,6,OLED_6X8);
				OLED_ShowNum(0,56,Addr_offset,8,OLED_6X8);
				OLED_UpdateArea(0,48,48,16); 
				delay_ms(dld);			
			}
		}
       OLED_transition(1);
		
		
	   p = menu_Roll_event();   
	   if( p == 1){ 
			dld+=10;
		   
	   }	 
       else if( p == -1){ 
		   if(dld) dld-=10;
		   
	   }	
		p=menu_Enter_event();
		if(p==1){
			
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}

#include "video_app.h"
void VIDEO_write(void);

void VIDEO(void)
{
//	Serial3_Init();
//	Serial2_Init();
	
	struct option_class2 option_list[] = {
		{"- 退出",Function,RETURN},
		{"- Bad_apple",Function,VIDEO_Bad_apple},                  //{选项名，需要运行的函数}
		{"- 只因你太美",Function,VIDEO_CXUK},
		{"- 烧录",Function,VIDEO_write},
		{"- 串流",Function,VIDEO_streaming},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);
	
//	Serial3_Power_OFF();
//	Serial2_Power_OFF();

}


void VIDEO_write(void)
{	
	struct option_class2 option_list[] = {
		{"- 返回",Function,RETURN},                
		{"- 烧录Bad_apple",Function,VIDEO_write_Bad_apple},
		{"- 烧录只因你太美",Function,VIDEO_write_CXUK},
		{"- 读取自定义",Function,VIDEO_custom},
		{"- 烧录自定义",Function,VIDEO_write_custom},
		{"- 擦除所有视频",Function,VIDEO_erase},
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);

}



void volt(void)
{
	
	
	struct option_class2 option_list[] = {
		{"- 退出",Function,RETURN},                  //{选项名，需要运行的函数}
		{"- 电压表",Function,voltage_mt},
		{"- 示波器",Function,error },
		{".."}								//结尾标志,方便计算数量
	};
	run_menu2(option_list);

}





void xxxxxxx(void)
{
	int8_t p;
	GUODUsn=1;
	
	while(1)
	{
		
		
		
	




		
       OLED_transition(1);
	   p = menu_Roll_event();   
	   if( p == 1){ 


	   }	 
       else if( p == -1){ 


	   }	
		p=menu_Enter_event();
		if(p==1){
			
			GUODUsn=1;
			OLED_transition(0);
			
			//进入某某函数//
			
			GUODUsn=1;
			
		}
		else if ( p==2 ){
		
			GUODUsn=1;
			OLED_transition(0);
			return;
			
		}	
	}
}





