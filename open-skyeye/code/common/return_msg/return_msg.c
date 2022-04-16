
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#ifdef __WIN32__
#include <windows.h>
#endif

#include <return_msg.h>
#include <Python.h>

char *get_bin_env_path_2(char *path_name)
{
    char *path = getenv("CONFIG_PATH");
    char *name = "\\config.txt";
    char config_path[1024] = { 0 };
    char str[100][1024] = { 0 };
    char ret[1024] = { "SKYEYEBIN" };
    FILE *fp;
    int count = 0;
    int i;
    unsigned int j;
    int len = strlen(path_name);

    if (path != NULL)
    {
        strcat(config_path, path);
        strcat(config_path, name);
        fp = fopen(config_path, "r");
        while (fgets(str[count], 1024, fp) != NULL)
        {
            count++;
        }
        for (i = 0; i < count; i++)
        {
            if (strncmp(str[i], path_name, strlen(path_name)) == 0 && str[i][len] == ':')
            {
                memset(ret, 0, sizeof (ret));
                for (j = len + 3; j < strlen(str[i]); j++)
                {
                    if (str[i][j] != '\n')
                    {
                        ret[j - len - 3] = str[i][j];
                    }
                }
            }
        }
        fclose(fp);
    }
    return getenv(ret);
}

char *SetSKYEYEBIN_2()
{
#ifndef __WIN32__
    setenv("SKYEYEBIN", SKYEYE_BIN, 1);
#endif
    return get_bin_env_path_2("BIN");

}

int SKYEYE_return_msg(return_info_t level, char *msg)
{
    char cmd_str[128] = { 0 };
    char new_path[1024] = { 0 };
    char *skyeye_bin = SetSKYEYEBIN_2();

    // Call Python Script
    PyGILState_STATE gstate;

    gstate = PyGILState_Ensure();
    printf("env skyeyebin: %s\n", skyeye_bin);

    sprintf(new_path, "sys.path.append('%s')\n", skyeye_bin);
    PyRun_SimpleString(new_path);

    PyRun_SimpleString("from re_grpc_command import *\n");
    sprintf(cmd_str, "RSE_log_report(%d, '''%s''')\n", level, msg);
    PyRun_SimpleString(cmd_str);

    PyGILState_Release(gstate);

    return 0;
}
