extern "C"{
#include "bochs.h"
#include <skyeye_mm.h>
#include <skyeye_types.h>
#include <skyeye_class.h>
#include <skyeye_iface.h>
#include <skyeye_x86devs_fin.h>
#include <skyeye_x86devs_intf.h>
#include "check_devices.h"
#include <skyeye_log.h>

//fi_q_t fi_q;
devs_l devs_q;

#if 0
static char* serial_regstr[]={
	"thr","ier","iir","lcr","mcr","lsr","msr","scr"
};
#endif

unsigned int x86_devs_get_devs_num(void){
		//printf("in %s, devs_q.count is %d\n",__FUNCTION__, devs_q.count);
		return devs_q.count;
}

char *x86_devs_get_devs_name(int d_id){
		if(devs_q.devs_d[d_id].name)
			return devs_q.devs_d[d_id].name;
		else
			return NULL;
}

unsigned int x86_devs_get_reg_num(int d_id)
{
	if(devs_q.devs_d[d_id].name){
		return devs_q.devs_d[d_id].regnum;
	}
	else
		return 0;
}

unsigned int x86_devs_get_reg_width(int d_id)
{
	if(devs_q.devs_d[d_id].name)
		return devs_q.devs_d[d_id].regwid;
	else
		return 0;
}

unsigned int x86_devs_get_reg_addr(int d_id, int r_id)
{
	if(devs_q.devs_d[d_id].name){
		if(r_id < devs_q.devs_d[d_id].regnum){
			return (devs_q.devs_d[d_id].regaddr[r_id]);
		}
	}
}

char *x86_devs_get_reg_name_by_id(int d_id, int r_id){

	if(devs_q.devs_d[d_id].name){
		if(r_id < devs_q.devs_d[d_id].regnum){
			//printf("in %s, d_id is %d, r_id is %d,  regname is %s\n", __FUNCTION__, d_id, r_id, devs_q.devs_d[d_id].regname[r_id]);
			return (devs_q.devs_d[d_id].regname[r_id]);
		}
	}
	return NULL;
}

unsigned int x86_devs_get_reg_value_by_id(int d_id, int r_id){
	//printf("in %s, did=%d, r_id=%d\n", __FUNCTION__, d_id, r_id);
	if(devs_q.devs_d[d_id].name){
		//printf("in %s, did=%d, r_id=%d, regby\n", __FUNCTION__, d_id, r_id);
		if(r_id < devs_q.devs_d[d_id].regnum){
			//printf("in %s, d_id is %d, r_id is %d,  value is %d\n", __FUNCTION__, d_id, r_id, devs_q.devs_d[d_id].func(devs_q.devs_d[d_id].th, devs_q.devs_d[d_id].regaddr[r_id], devs_q.devs_d[d_id].regwid));
			//printf("func=0x%x in %s, did=%d\n", devs_q.devs_d[d_id].func, __FUNCTION__, d_id);
			return devs_q.devs_d[d_id].func(devs_q.devs_d[d_id].th, devs_q.devs_d[d_id].regaddr[r_id], devs_q.devs_d[d_id].regwid);
		}
	}
}
#if 1
unsigned int x86_devs_get_reg_large_value_by_id(int d_id, int r_id, int offset){
	printf("in %s, did=%d, r_id=%d\n", __FUNCTION__, d_id, r_id);
	if(devs_q.devs_d[d_id].name){
		printf("in %s, did=%d, r_id=%d, regby\n", __FUNCTION__, d_id, r_id);
		if(r_id < devs_q.devs_d[d_id].regnum && offset < devs_q.devs_d[d_id].regwid){
			//printf("in %s, d_id is %d, r_id is %d,  value is %d\n", __FUNCTION__, d_id, r_id, devs_q.devs_d[d_id].func(devs_q.devs_d[d_id].th, devs_q.devs_d[d_id].regaddr[r_id], devs_q.devs_d[d_id].regwid));
			printf("func=0x%x in %s, did=%d\n", devs_q.devs_d[d_id].func, __FUNCTION__, d_id, offset);
			return devs_q.devs_d[d_id].func(devs_q.devs_d[d_id].th, offset, devs_q.devs_d[d_id].regwid);
		}
	}
}
#endif
#define NO_ADDR -1

void x86_devs_save_fin(int d_id,unsigned int addr, int bitnum, int mode)
{
	int i = 0, j = 0;
	fi_q_t *fi_q;
	xfi_t *new_fi;

	if(devs_q.devs_d[d_id].name)
		fi_q = &(devs_q.devs_d[d_id].fi_q);
	else
		return;

	if(fi_q->count >= FIN_NUM)
		return;

	new_fi = (xfi_t *)skyeye_mm_zero(sizeof(xfi_t));
	
	new_fi->addr = addr;
	new_fi->bitnum = bitnum%32;
	new_fi->mode = mode%3;

	for(i = 0; i <= fi_q->count; i ++){
		if(fi_q->fi[i][0] != NO_ADDR && addr > fi_q->fi[i][0])
			continue;
		else{
			if(i == fi_q->count){
				fi_q->fi[i][0] = addr;
				fi_q->fi[i][1] = (unsigned int)new_fi;
			}else{
				for(j = fi_q->count-1; j >= i; j --){
					fi_q->fi[j+1][0] = fi_q->fi[j][0];
					fi_q->fi[j+1][1] = fi_q->fi[j][1];
				}
				fi_q->fi[i][0] = addr;
				fi_q->fi[i][1] = (unsigned int)new_fi;
			}
			fi_q->count ++;
			break;
		}
	}

#if 0
	for(i = 0; i <= fi_q->count; i ++){
		if(fi_q->fi[i][0] != NO_ADDR && addr > fi_q->fi[i][0])
			continue;
		else
			if(i == fi_q->count){
				fi_q->fi[i][0] = addr;
				fi_q->fi[i][1] = (unsigned int)new_fi;
			}else{
				for(j = fi_q->count-1; j >= i; j --){
					fi_q->fi[j+1][0] = fi_q->fi[j][0];
					fi_q->fi[j+1][1] = fi_q->fi[j][1];
				}
				fi_q->fi[i][0] = addr;
				fi_q->fi[i][1] = (unsigned int)new_fi;
			}
	}
	fi_q->count ++;
#endif

	//printf("in %s, offset is %d, bitnum is %d, mode is %d\n", __FUNCTION__, addr, bitnum, mode );
}


#define GET_FBYTE(x,r)  do{ \
	int offbyte = ((xfi_t*)(fi_q->fi[x][1]))->bitnum /8; \
	offbyte += fi_q->fi[x][0]; \
	r = offbyte; \
}while(0);

int x86_devs_r_plug(int d_id, unsigned int addr, int buf,unsigned int count)
{
	int tmp = *(int*)buf;
	int i;
	int fstart = 0,fend = 0,fcur = 0;
	fi_q_t *fi_q;

	if(devs_q.devs_d[d_id].name)
		fi_q = &(devs_q.devs_d[d_id].fi_q);
	else
		return;

	if(fi_q->fi[0][0] != NO_ADDR){
		GET_FBYTE(0, fstart);	
		GET_FBYTE(fi_q->count -1, fend);	
	}

	if(addr >= fstart && addr <= fend){
		for(i = 0; i < fi_q->count; i++){
			GET_FBYTE(i, fcur);
			if(fcur >= addr && fcur < (addr + count)){
				do{
					switch(((xfi_t*)(fi_q->fi[i][1]))->mode){
						case f_and:
							buf &= ~(1 << (((xfi_t*)(fi_q->fi[i][1]))->bitnum + (fcur - addr)*8));	
							break;
						case f_or:
							buf |= 1 << (((xfi_t*)(fi_q->fi[i][1]))->bitnum + (fcur - addr) * 8);
							break;
						case f_xor:
							buf ^= 1 << (((xfi_t*)(fi_q->fi[i][1]))->bitnum + (fcur - addr) * 8);
							break;
						default:
							skyeye_log(Info_log, __FUNCTION__, "no this falut injection command, please configure this option correct or close this option\n");
					}
					i++;
					if(i >= fi_q->count)
						break;
					GET_FBYTE(i, fcur);
				}while(fcur >= addr && fcur < (addr + count));
			}
		}
	}

	return buf;

#if 0
	if(addr >= fi_q->fi[0][0] && addr <= fi_q->fi[fi_q->count-1][0]){
		for(i = 0; i < fi_q->count; i++){
			if(fi_q->fi[i][0] == addr){
				do{
					switch(((xfi_t*)(fi_q->fi[i][1]))->mode){
						case f_and:
							*(int*)buf &= 1 << ((xfi_t*)(fi_q->fi[i][1]))->bitnum;
							break;
						case f_or:
							*(int*)buf |= 1 << ((xfi_t*)(fi_q->fi[i][1]))->bitnum;
							break;
						case f_xor:
							*(int*)buf ^= 1 << ((xfi_t*)(fi_q->fi[i][1]))->bitnum;
							break;
						default:
							skyeye_log(Info_log, __FUNCTION__, "no this falut injection command, please configure this option correct or close this option\n");
					}
					i++;
				}while((i < fi_q->count) && (fi_q->fi[i][0] == addr));
				break;
			}
		}
	}
#endif

//	printf("in %s, \n", __FUNCTION__);
}

void x86_devs_clear_fin(int d_id, unsigned int addr, int bitnum, int mode)
{
	xfi_t *tmp;
	xfi_t *del;
	int i = 0,j = 0;

	fi_q_t *fi_q;

	if(devs_q.devs_d[d_id].name)
		fi_q = &(devs_q.devs_d[d_id].fi_q);
	else 
		return;

	if(fi_q->count){
		for(i = 0; i < fi_q->count; i ++){
			if(((xfi_t*)(fi_q->fi[i][1]))->addr != addr){
				continue;
			}
			else
				if(((xfi_t*)(fi_q->fi[i][1]))->bitnum == bitnum && ((xfi_t*)(fi_q->fi[i][1]))->mode == mode){
					fi_q->fi[i][0] = NO_ADDR;
					skyeye_free((void*)fi_q->fi[i][1]);
					for( j = i; j < fi_q->count - 1; j ++){
						fi_q->fi[j][0] = fi_q->fi[j+1][0];
						fi_q->fi[j+1][0] = NO_ADDR;
						fi_q->fi[j][1] = fi_q->fi[j+1][1];
						fi_q->fi[j+1][1] = NULL;
					}
					fi_q->count --;
				}
		}
	}

#if 0
	if(fi_q->count){
		for(i = 0; i < fi_q->count; i ++){
			if(((xfi_t*)(fi_q->fi[i][1]))->addr != addr)
				continue;
			else
				if(((xfi_t*)(fi_q->fi[i][1]))->bitnum == bitnum){
					fi_q->fi[i][0] = NO_ADDR;
					free((void*)(fi_q->fi[i][1]));
					for( j = i; j < fi_q->count; j ++){
						fi_q->fi[j][0] = fi_q->fi[j+1][0];
						fi_q->fi[j][1] = fi_q->fi[j+1][1];
					}
					fi_q->fi[j][0] = NO_ADDR;
					fi_q->fi[j][1] = NO_FI;
				}
		}
		fi_q->count --;
	}
#endif

//	printf("in %s, offset is %d, bitnum is %d, mode is %d\n", __FUNCTION__, addr, bitnum, mode);

}

fi_q_t *x86_devs_get_fin(int d_id)
{
	fi_q_t *fi_q;

	if(devs_q.devs_d[d_id].name)
		fi_q = &(devs_q.devs_d[d_id].fi_q);

	return fi_q;
}


devs_l *x86_devs_get_devices(void)
{
	return &devs_q;
}

unsigned int x86_add_device(char *name, unsigned int (*r_func)(void *, unsigned int offset, unsigned int len), void *th, int reg_num, int reg_wid, char *regname[], unsigned int *regaddr)
{
	int namelen;
	int i;
	devices_des *tmp = &(devs_q.devs_d[devs_q.count]);
	fi_q_t *fi_q = &(devs_q.devs_d[devs_q.count].fi_q);

	tmp->name = (char*)skyeye_mm_zero(strlen(name) + 1);
	strncpy(tmp->name, name, strlen(name));
	tmp->func = r_func;
	tmp->th = th;
	tmp->regnum = reg_num;
	tmp->regwid = reg_wid;
	tmp->regname = regname;
	tmp->regaddr = regaddr;

	//printf("in %s, reg_num is %d, reg_wid is %d, r_func=0x%x\n", __FUNCTION__, reg_num, reg_wid, r_func);
	#if 0
	for(i = 0; i < reg_num; i ++ ){
		printf("%d:%s = %d\n", regaddr[i], regname[i], r_func(th, regaddr[i], 1));	
	
	}
	#endif
	for( i = 0; i < FIN_NUM; i ++ ){
		fi_q->fi[i][0] = NO_ADDR;
	}

#if 0
	printf("#################in %s, name is %s, func is 0x%x, th is 0x%x\n", __FUNCTION__, name, r_func, th);
#endif

#if 0
	if(strlen(name) < 6)
		namelen = strlen(name);
	else
		namelen = 6;

	if(!strncmp(name, "serial", namelen)){
		tmp->regname = serial_regstr;
	}
#endif

	devs_q.count ++;
	//printf("in %s, devs_q.count is %d\n", __FUNCTION__, devs_q.count);
	return devs_q.count-1;
}

void x86_list_devices(void)
{
	int i;

	for( i = 0; i < DEVS_NUM; i ++ ){
		devices_des *tmp = &(devs_q.devs_d[i]);

#if 0
		if(tmp->name){
			printf("dev name is %s\n", tmp->name);
			printf("offset %d is 0x%x\n",i, tmp->func((void*)(tmp->th), i, 1));
		}
#endif
	}

}
}
