#include "fm.h"

FATFS FatFs;
uint8_t fm_inited = 0;
char *txt;
GUI_ListData *fm_flist;
GUI_ListItemData **fm_curfiles;

void fm_init()
{
	if (f_mount(&FatFs, "", 1) == FR_OK) {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, GPIO_PIN_SET);
		fm_inited = 1;
	}
	else
	{
		fm_inited = 0;
		return;
	}
	FIL fil;
	f_open(&fil, "message.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
	uint16_t sz = f_size(&fil);
	txt=malloc((sz + 1)*sizeof(char));
	UINT rd = 0;
	f_read(&fil, txt, sz, &rd); 
	create_files_list();
	f_mount(0, "", 1);
}

void fm_draw()
{
	if(!fm_inited)
	{
		gui_lable("SD card don't inited", 1, 1, 125, 20, 0, 1);
	}
	else
	{
		gui_lable("SD card OK", 0, 0, 128, 20, 0, 1);
		gui_lable_multiline(txt, 1, 21, 125, 42, 0, 1);
	}
}

uint8_t fm_input(uint8_t key)
{
	
}

uint32_t count_files(char *path)
{
	uint32_t count = 0;
	DIR dir;
	FILINFO fno;
	FRESULT res;
	static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
	fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
	
	res = f_opendir(&dir, path);                       /* Open the directory */
	if (res == FR_OK) {
		for (;;) {
			res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fname[0] == '.') continue;             /* Ignore dot entry */
      if (!(fno.fattrib & AM_DIR)) {                 /* It is a file. */
				count ++;
      }
    }
    f_closedir(&dir);
	}
	return count;
}

void create_files_list()
{	
	printf("cre lst\n");
	char path[] = "/";
	uint32_t count = count_files(path);
	printf("count: %d\n", count);
	if(count == 0)
		return;
	
	fm_curfiles = malloc(6*sizeof(GUI_ListItemData));
	fm_curfiles[0] = gui_create_listItem("1.txt", 0, 0, 0, 0);
	fm_curfiles[1] = gui_create_listItem("sdfsdfa.txt", 0, 0, 0, 0);
	fm_curfiles[2] = gui_create_listItem("1dsfas.txt", 0, 0, 0, 0);
	fm_curfiles[3] = gui_create_listItem("1ddd.txt", 0, 0, 0, 0);
	fm_curfiles[4] = gui_create_listItem("5552.txt", 0, 0, 0, 0);
	fm_curfiles[5] = gui_create_listItem("666.txt", 0, 0, 0, 0);


	fm_flist = gui_create_list("sl", 6, fm_curfiles, 0, 0, 128, 64, 0, 0, 0);
	gui_set_curList(fm_flist);
}
