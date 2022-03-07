#include "fat.h"
#include "esp_spiffs.h"
void * blblopen(struct _lv_fs_drv_t * drv, const char * path, lv_fs_mode_t mode)
{
	FILE* fp;
	if(mode==LV_FS_MODE_WR)
		fp=fopen(path, "wb");
	else
		fp=fopen(path, "rb");
	return fp;
}
lv_fs_res_t blblclose(struct _lv_fs_drv_t * drv, void * file_p)
{
	if(file_p==NULL)
		return LV_FS_RES_FS_ERR; 
	return fclose(file_p);
}
lv_fs_res_t blblread(struct _lv_fs_drv_t * drv, void * file_p, void * buf, uint32_t btr, uint32_t * br)
{	 
	if(file_p==NULL)
		return LV_FS_RES_FS_ERR; 
	*br=fread(buf, 1, btr, file_p);
	return LV_FS_RES_OK;
}
lv_fs_res_t blblwrite(struct _lv_fs_drv_t * drv, void * file_p, const void * buf, uint32_t btw, uint32_t * bw)
{
	if(file_p==NULL)
		return LV_FS_RES_FS_ERR; 
	*bw= fwrite(buf,1,btw,file_p);
	return LV_FS_RES_OK;
}
lv_fs_res_t blblseek(struct _lv_fs_drv_t * drv, void * file_p, uint32_t pos, lv_fs_whence_t whence)
{
	if(file_p==NULL)
		return LV_FS_RES_FS_ERR; 
	return fseek(file_p, pos, whence);
}
lv_fs_res_t blbltell(struct _lv_fs_drv_t * drv, void * file_p, uint32_t * pos_p)
{
	if(file_p==NULL)
	return LV_FS_RES_FS_ERR; 
	*pos_p=ftell(file_p);
	return LV_FS_RES_OK;
}

void fsf_init()
{
    static lv_fs_drv_t drv;                   /*Needs to be static or global*/
    lv_fs_drv_init(&drv);                     /*Basic initialization*/
    drv.letter = 'S';                         /*An uppercase letter to identify the drive */
    drv.ready_cb = NULL;               /*Callback to tell if the drive is ready to use */
    drv.open_cb = blblopen;                 /*Callback to open a file */
    drv.close_cb = blblclose;               /*Callback to close a file */
    drv.read_cb = blblread;                 /*Callback to read a file */
    drv.write_cb = blblwrite;               /*Callback to write a file */
    drv.seek_cb = blblseek;                 /*Callback to seek in a file (Move cursor) */
    drv.tell_cb = blbltell;                 /*Callback to tell the cursor position  */
    
    drv.dir_open_cb = NULL;         /*Callback to open directory to read its content */
    drv.dir_read_cb = NULL;         /*Callback to read a directory's content */
    drv.dir_close_cb = NULL;       /*Callback to close a directory */
    
    drv.user_data = NULL;             /*Any custom data if required*/
    
    lv_fs_drv_register(&drv);                 /*Finally register the drive*/
}