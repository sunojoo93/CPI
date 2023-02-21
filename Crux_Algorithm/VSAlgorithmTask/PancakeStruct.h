#pragma once

struct stPancakeInfo
{
	__int64 id;
	char plant[20];
	char process[20];
	char wc[20];
	char material[20];
	char grade[20];
	char color[20];
	__int64 lot;
	char type[20];
	int line;
	__int64 barcode;
	double size_mm;
	char input_time[100];
	char align_start[100];
	char align_end[100];
	char color_start[100];
	char color_end[100];
	char main_start[100];
	char main_end[100];
	char classify_time[100];
	char align_recipe[10000];
	char color_recipe[10000];
	char main_recipe[10000];
	char judge_recipe[10000];
};

struct stPancakeDefect 
{
	__int64 id;
	bool exception;
	int ITEM_NO;
	char ITEM_NM[100];
	char MIC_CODE[100];
	int defect_no;
	char img_name[100];
	int pixel_start_x;
	int pixel_start_y;
	int pixel_end_x;
	int pixel_end_y;
	double nm_start_x;
	double nm_start_y;
	double nm_end_x;
	double nm_end_y;
	double def_size;
	int pixel_num;
	char defect_code[100];
	char color_code[100];
		// 2022.06.20~ MDJ Table º¯°æÀ¸·Î ÀÎÇÏ¿© ¼öÁ¤
	double ColorL;
	double ColorDegree;
	double ColorLength;
	double MVper; 
};