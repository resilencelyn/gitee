#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lic.h"
#include "skyeye_internal.h"

char allow_cpu_list[2048];
int verify_status = 0;

void split(char **arr, char *str, const char *del)
{
    char *s = strtok(str, del);

    while (s != NULL)
    {
        *arr++ = s;
        s = strtok(NULL, del);
    }
}
int check_cpu(char *cpuname)
{
    if (strstr(allow_cpu_list, "ALL") != NULL || strstr(allow_cpu_list, cpuname) != NULL)
        return 1;
    else
        return 0;
}

int license_verify()
{
    char conf_dir[512] = { 0 };
    FILE *fp;
    char buffer[512] = { 0 };
    char *arr[3];
    char *skyeye_bin = get_bin_env_path("BIN");
    char temp[2][128] = { 0 };
    int i, ret = 0;

#ifndef __WIN32__
    return 3;
#else
    strcpy(conf_dir, skyeye_bin);
    strcat(conf_dir, "\\..\\..\\..\\info\\license\\lic_conf.dat");
    if ((access(conf_dir, 0)) == -1)
    {
        printf("The license configuration file was not found\n");
        skyeye_log(Error_log, __FUNCTION__, "The license configuration file was not found\n");
        return 8;
    }
    fp = fopen(conf_dir, "r");
    fread(buffer, 512, 1, fp);
    fclose(fp);
    split(arr, buffer, "\n");
    if (strstr(arr[0], "network") != NULL)
    {
        for (i = 1; i < 3; i++)
            sscanf(arr[i], "%*[^:]:%s", temp[i - 1]);
        ret = network_lic_validation(temp[0], atoi(temp[1]));

    } else if (strstr(arr[0], "local") != NULL)
    {
        sscanf(arr[1], "%*[^:]:%s", temp[0]);
        ret = local_lic_validation(temp[0]);
    } else
    {
        ret = dog_lic_validation();
    }
    return ret;
#endif
}
