/*
 * © Copyright 2014 zyuming. All Rights Reserved.
 * File:    json_file.c
 * Email:   zyumingfit@gmail.com
 * Date:    2014-02-12
 */
#include <stdio.h>

/*
 * Judge whether there is json configure file
 */
int check_current_dir(void)
{
    FILE *fp;

    fp = fopen("skyeye.json", "r");
    if (fp == NULL)
    {
        return -1;
    } else
        fclose(fp);
    return 1;
}
