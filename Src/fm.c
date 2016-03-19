#include "fm.h"

FATFS FatFs;
uint8_t fm_inited = 0;
char *txt;
GUI_ListData *fm_flist;
GUI_ListItemData **fm_curfiles;
char file_buff[100];

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
	create_files_list();
	//f_mount(0, "", 1);
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

void fm_handle_file_open(uint16_t id, uint32_t arg, uint8_t act)
{
	memset(file_buff, '\0', sizeof(file_buff));
	FIL fil;
	f_open(&fil, fm_curfiles[id]->text, FA_OPEN_EXISTING | FA_READ);
	UINT rd = 0;
	f_read(&fil, file_buff, 99, &rd); 
	gui_showMessage(file_buff);
	f_close(&fil);
}

void create_files_list()
{	
	printf("cre lst\n");
	char path[] = "/";
	uint32_t count = count_files(path);
	printf("count: %d\n", count);
	if(count == 0)
		return;
	
	uint32_t ci = 0;
	fm_curfiles = malloc(count*sizeof(GUI_ListItemData));

	FRESULT res;
  FILINFO fno;
  DIR dir;
  int i;
  char *fn;   /* This function assumes non-Unicode configuration */
  static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
  fno.lfname = lfn;
  fno.lfsize = sizeof(lfn);
	printf("cre lst\n");
	res = f_opendir(&dir, path);											 /* Open the directory */
	if (res == FR_OK) {
		i = strlen(path);
		for (;;) {
			res = f_readdir(&dir, &fno);									 /* Read a directory item */
			if (res != FR_OK || fno.fname[0] == 0) break;	/* Break on error or end of dir */
			if (fno.fname[0] == '.') continue;						 /* Ignore dot entry */
			fn = *fno.lfname ? fno.lfname : fno.fname;
			if (fno.fattrib & AM_DIR) {										/* It is a directory */
				//sprintf(&path[i], "/%s", fn);
				//path[i] = 0;
				printf("fldr\n");
				if (res != FR_OK) break;
			} else {																			 /* It is a file. */
				char *bbb = malloc((strlen(fn) + 1) * sizeof(char));
				fm_curfiles[ci] = gui_create_listItem(bbb, 0, 0, 0, 0);
				strcpy(bbb, fn);
				printf("kek %s\n", fn);
				ci++;
			}
		}
		f_closedir(&dir);
	}
	

	fm_flist = gui_create_list("sl", count, fm_curfiles, 0, 0, 128, 64, &fm_handle_file_open, 0, 0);
	gui_set_curList(fm_flist);
}
