/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_parser.h
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#ifndef __JSON_PARSER_H__
#define __JSON_PARSER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <string.h>
#define znew(l) memset(malloc(l), 0, (l))
#define dest(p) free((void*)p)
#define F_UNUSE __attribute__((unused))

    struct attr_desc
    {
        struct attr_desc *next;
        int idx;
        const char *key;
        const char *type;
        const char *val;
    };

    static F_UNUSE struct attr_desc *attr_desc_create()
    {
        return znew(sizeof (struct attr_desc));
    }

    static F_UNUSE void attr_desc_destroy(struct attr_desc *attr)
    {
        dest(attr->key);
        dest(attr->val);
        dest(attr);
    }

    static F_UNUSE void attr_desc_list_insert(struct attr_desc **list, struct attr_desc *attr)
    {
        if (list && *list)
        {
            struct attr_desc *tmp;

            for (tmp = *list; tmp->next; tmp = tmp->next)
            {                           /*do nothing */
            }
            tmp->next = attr;
        } else if (list)
        {
            *list = attr;
        } else
        {
            /* nothing */
        }
    }

    static F_UNUSE void attr_desc_list_destroy(struct attr_desc *list)
    {
        struct attr_desc *tmp = list, *next;

        while (tmp)
        {
            next = tmp->next;
            attr_desc_destroy(tmp);
            tmp = next;
        }
    }

    struct obj_desc
    {
        /* public */
        const char *name;
        const char *base;
        const char *cls;
        struct attr_desc *attrs;
        struct obj_desc *subobj;
        /* private */
        struct obj_desc *next;
    };

    static F_UNUSE struct obj_desc *obj_desc_create()
    {
        return znew(sizeof (struct obj_desc));
    }

    static void obj_desc_list_destroy(struct obj_desc *list);
    static F_UNUSE void obj_desc_destroy(struct obj_desc *obj)
    {
        dest(obj->name);
        dest(obj->base);
        dest(obj->cls);
        attr_desc_list_destroy((void *) obj->attrs);
        obj_desc_list_destroy((void *) obj->subobj);
        dest(obj);
    }

    static F_UNUSE void obj_desc_list_insert(struct obj_desc **list, struct obj_desc *obj)
    {
        if (list && *list)
        {
            struct obj_desc *tmp;

            for (tmp = *list; tmp->next; tmp = tmp->next)
            {                           /*do nothing */
            }
            tmp->next = obj;
        } else if (list)
        {
            *list = obj;
        } else
        {
            /* nothing */
        }
    }

    static F_UNUSE void obj_desc_list_destroy(struct obj_desc *list)
    {
        struct obj_desc *tmp = list, *next;

        while (tmp)
        {
            next = tmp->next;
            obj_desc_destroy(tmp);
            tmp = next;
        }
    }

    void look_objlist(struct obj_desc *list, int pos);
    struct obj_desc *parse_string(const char *str);
    struct obj_desc *parse_file(const char *path);

#ifdef __cplusplus
}
#endif

#endif
