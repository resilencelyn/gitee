#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef __MINGW32__
#include <windows.h>
#else
#include <fcntl.h>
#endif
#include<stdbool.h>
#include "lic.h"
#include "skyeye_internal.h"
#include "dog_api.h"
#include "dog_vcode.h"

/*surper dog code */
#define CUSTOM_FEATURE                     1022
#define CUSTOM_FILEID                         1 //DOG_FILEID_RW
#define DEMO_MEMBUFFER_SIZE                 128

#define NO_DLL_ERROR                        17
#define DOG_LOGIN_OK                        18
#define DOG_NOT_FOUND_ERROR                 19
#define DOG_FEATURE_NOT_FOUND_ERROR         20
#define DOG_INV_VCODE_ERROR                 21
#define DOG_LOCAL_API_ERROR                 22
#define DOG_FEATURE_EXPIRED_ERROR           23
#define DOG_FEATURE_ERROR                   24
#define Read_Data_Failed_ERROR              25

char Lincense_Data_Buff[DEMO_MEMBUFFER_SIZE];

int dog_lic_validation()
{
#ifndef __MINGW32__
    return 3;
#else
    int ret = 0;
    int i = 0;
    char surper_dog_dlldir[512] = { 0 };
    char *skyeye_bin = get_bin_env_path("BIN");
    dog_status_t status;
    dog_handle_t handle;
    dog_size_t fsize;

    /*surper dog code */
    strcpy(surper_dog_dlldir, skyeye_bin);
    strcat(surper_dog_dlldir, "\\..\\..\\..\\third_party\\dog_windows_3159223.dll");
    if ((access(surper_dog_dlldir, F_OK)) == -1)
    {
        skyeye_log(Error_log, __FUNCTION__, "The file %s has not been found!\n", surper_dog_dlldir);
        return NO_DLL_ERROR;
    }
    HMODULE surper_dog_dll = LoadLibrary(surper_dog_dlldir);

    Dog_Login_t Dog_Login = (Dog_Login_t) GetProcAddress(surper_dog_dll, "dog_login");
    Dog_Logout_t Dog_Logout = (Dog_Logout_t) GetProcAddress(surper_dog_dll, "dog_logout");
    Dog_Read_t Dog_Read = (Dog_Read_t) GetProcAddress(surper_dog_dll, "dog_read");
    Dog_Get_Size_t Dog_Get_Size = (Dog_Get_Size_t) GetProcAddress(surper_dog_dll, "dog_get_size");

    status = Dog_Login(CUSTOM_FEATURE, (dog_vendor_code_t *) vendor_code, &handle);
    switch (status)
    {
        case DOG_STATUS_OK:
            skyeye_log(Info_log, __FUNCTION__, "Surper Dog Login Success!\n");
            break;
        case DOG_FEATURE_NOT_FOUND:
            skyeye_log(Error_log, __FUNCTION__, "login to default feature failed\n");
            ret = DOG_FEATURE_NOT_FOUND_ERROR;
            break;
        case DOG_NOT_FOUND:
            skyeye_log(Error_log, __FUNCTION__, "no SuperDog with vendor code DEMOMA found\n");
            ret = DOG_NOT_FOUND_ERROR;
            break;
        case DOG_INV_VCODE:
            skyeye_log(Error_log, __FUNCTION__, "invalid vendor code\n");
            ret = DOG_INV_VCODE_ERROR;
            break;
        case DOG_LOCAL_COMM_ERR:
            skyeye_log(Error_log, __FUNCTION__, "communication error between API and local SuperDog License Manager\n");
            ret = DOG_LOCAL_API_ERROR;
            break;
        case DOG_FEATURE_EXPIRED:
            skyeye_log(Error_log, __FUNCTION__, "feature can't be used\n");
            ret = DOG_FEATURE_EXPIRED_ERROR;
            break;
        default:
            skyeye_log(Error_log, __FUNCTION__, "login to default feature failed with status error\n");
            ret = DOG_FEATURE_ERROR;
            break;
    }
    if (status)
    {
        return ret;
    }
    Dog_Get_Size(handle, CUSTOM_FILEID, &fsize);
    status = Dog_Read(handle, CUSTOM_FILEID, 0, fsize, &Lincense_Data_Buff[0]);

    if (status)
    {
        skyeye_log(Error_log, __FUNCTION__, "Surper Dog License Data Error!\n");
        return Read_Data_Failed_ERROR;
    } else
    {
        strcpy(allow_cpu_list, (const char *) Lincense_Data_Buff);
        skyeye_log(Info_log, __FUNCTION__, "Surper Dog License Data Success!\n");
        return DOG_LOGIN_OK;
    }
#endif
}

const bool_t check_usb_lic_connect()
{
#ifndef __MINGW32__
    return False;
#else
    char surper_dog_dlldir[512] = { 0 };
    char *skyeye_bin = get_bin_env_path("BIN");
    dog_status_t status;
    dog_handle_t handle;

    /*surper dog code */
    strcpy(surper_dog_dlldir, skyeye_bin);
    strcat(surper_dog_dlldir, "\\..\\..\\..\\third_party\\dog_windows_3159223.dll");
    if ((access(surper_dog_dlldir, F_OK)) == -1)
    {
        skyeye_log(Error_log, __FUNCTION__, "The file %s has not been found!\n", surper_dog_dlldir);
        return False;
    }
    HMODULE surper_dog_dll = LoadLibrary(surper_dog_dlldir);

    Dog_Login_t Dog_Login = (Dog_Login_t) GetProcAddress(surper_dog_dll, "dog_login");
    Dog_Logout_t Dog_Logout = (Dog_Logout_t) GetProcAddress(surper_dog_dll, "dog_logout");
    Dog_Read_t Dog_Read = (Dog_Read_t) GetProcAddress(surper_dog_dll, "dog_read");
    Dog_Get_Size_t Dog_Get_Size = (Dog_Get_Size_t) GetProcAddress(surper_dog_dll, "dog_get_size");

    status = Dog_Login(CUSTOM_FEATURE, (dog_vendor_code_t *) vendor_code, &handle);
    /*
     * if(status == DOG_STATUS_OK)
     * {
     * //printf("surper dog link success!\n");
     * return true;
     * } else
     * {
     * //printf("surper dog link error!\n");
     * return false;
     * }
     */
    if (status == DOG_NOT_FOUND)
    {
        //printf("surper dog link error!\n");
        return False;
    } else
    {
        //printf("surper dog link success!\n");
        return True;
    }

#endif
}
