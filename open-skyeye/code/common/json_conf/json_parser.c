/*
 * © Copyright 2014 eJim Lee. All Rights Reserved.
 * File:    json_parser.c
 * Email:   lyjforjob@gmail.com
 * Date:    2014-02-11
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "portable/json.h"
#include "json_parser.h"
//#include "json_device.h"

/* compiled by c90 */
#ifdef json_object_object_foreach
#undef json_object_object_foreach
#define json_object_object_foreach(obj, key, val) \
	char *key; struct json_object *val; struct lh_entry *entry; \
	for(entry = json_object_get_object(obj)->head; ({ if(entry) { key = (char*)entry->k; val = (struct json_object*)entry->v; } ; entry; }); entry = entry->next )
#endif

/*
 * [0, "string", "abc"]
 * or
 * ["string", "abc"]
 */
static struct attr_desc *parse_attr_in(struct json_object *attribute, const char *key)
{
#define PARSE_CHECK_TYPE(name, val, type) \
	do{ \
		if(!json_object_is_type(val, json_type_##type)) { \
			fprintf(stderr, "value(%s) of attribute \"%s\" " \
					"is invailed (require a %s)!\n", \
					json_object_to_json_string(val), name, #type); \
			goto err_happend; \
		} \
	}while(0)

    struct attr_desc *attr = attr_desc_create();

    attr->key = strdup(key);

    int array_len = json_object_array_length(attribute);

    if (array_len == 2)
    {
        struct json_object *attr_type = json_object_array_get_idx(attribute, 0);

        PARSE_CHECK_TYPE(key, attr_type, string);
        struct json_object *attr_value = json_object_array_get_idx(attribute, 1);

        PARSE_CHECK_TYPE(key, attr_value, string);
        attr->idx = 0;
        attr->val = strdup(json_object_get_string(attr_value));
        attr->type = strdup(json_object_get_string(attr_type));
    } else if (array_len == 3)
    {
        struct json_object *attr_idx = json_object_array_get_idx(attribute, 0);
        PARSE_CHECK_TYPE(key, attr_idx, int);
        struct json_object *attr_type = json_object_array_get_idx(attribute, 1);

        PARSE_CHECK_TYPE(key, attr_type, string);
        struct json_object *attr_value = json_object_array_get_idx(attribute, 2);

        PARSE_CHECK_TYPE(key, attr_value, string);
        attr->idx = json_object_get_int(attr_idx);
        attr->val = strdup(json_object_get_string(attr_value));
        attr->type = strdup(json_object_get_string(attr_type));
    } else
    {
        fprintf(stderr, "value(%s) of attribute \"%s\" is invailed!\n", json_object_to_json_string(attribute), key);
        goto err_happend;
    }
    return attr;
  err_happend:
    attr_desc_destroy(attr);
    return NULL;
#undef PARSE_CHECK_TYPE
}

/*
 * example1:
 *      "attr": [  // a attr_array
 *              [0, "string", "abc"], // a attr_idx
 *              [1, "integer", "123"],
 *      ]
 *
 * example2:
 * "attr": ["string", "abc"]
 */
static struct attr_desc *parse_attr(struct json_object *attribute, const char *key)
{
    struct attr_desc *list = NULL;

    /* attribute is a array */
    /*
     * attribute look like example1, if first element is array
     * attribute look like example2, if first element is string
     */
    int array_len = json_object_array_length(attribute);

    if (array_len < 1)
    {
        fprintf(stderr, "attribute \"%s\" is empty!\n", key);
        goto err_happend;
    }

    struct json_object *first = json_object_array_get_idx(attribute, 0);
    enum json_type type = json_object_get_type(first);

    if (type == json_type_array)
    {
        /* example1 */
        int i;

        for (i = 0; i < array_len; i++)
        {
            struct json_object *attr_idx = json_object_array_get_idx(attribute, i);

            if (!attr_idx)
            {
                fprintf(stderr, "Get attribute \"%s[%d]\" failed!\n", key, i);
                goto err_happend;
            }
            struct attr_desc *attr = parse_attr_in(attr_idx, key);

            if (!attr)
            {
                fprintf(stderr, "attribute \"%s[%d]\" get a error value!\n", key, i);
                goto err_happend;
            }
            attr_desc_list_insert(&list, attr);
        }
    } else if (type == json_type_string)
    {
        /* example2 */
        struct attr_desc *attr = parse_attr_in(attribute, key);

        if (!attr)
        {
            fprintf(stderr, "attribute \"%s\" get a error value!\n", key);
            goto err_happend;
        }
        attr_desc_list_insert(&list, attr);
    } else
    {
        fprintf(stderr, "value(%s) of attribute \"%s\" is invailed!\n", json_object_to_json_string(attribute), key);
        goto err_happend;
    }

    return list;
  err_happend:
    attr_desc_list_destroy(list);
    return NULL;
}

static struct obj_desc *parse_object(struct json_object *root, const char *name)
{
#define PARSE_CHECK_TYPE(objname, key, val, type) \
	do{ \
		if(!json_object_is_type(val, json_type_##type)) { \
			fprintf(stderr, "object \"%s\" need a \"%s\" " \
					"and value type should be %s\n", objname, key, #type); \
			goto err_happend; \
		} \
	}while(0)

    struct obj_desc *obj = obj_desc_create();

    obj->name = strdup(name);

    json_object_object_foreach(root, key, value)
    {
        /* class and base is object base attribute */
        if (strcmp(key, "class") == 0)
        {
            /* class */
            PARSE_CHECK_TYPE(name, key, value, string);
            obj->cls = strdup(json_object_get_string(value));
            continue;
        }
        if (strcmp(key, "base") == 0)
        {
            /* base */
            PARSE_CHECK_TYPE(name, key, value, string);
            obj->base = strdup(json_object_get_string(value));
            continue;
        }
        /* sub object description by dict and attribute description by array */
        enum json_type type = json_object_get_type(value);

        if (type == json_type_object)
        {
            /* sub object */
            struct obj_desc *subobj = parse_object(value, key);

            if (!subobj)
            {
                fprintf(stderr, "in \"%s\" get a error when parser sub object!\n", name);
                goto err_happend;
            }
            obj_desc_list_insert(&obj->subobj, subobj);
            continue;
        }
        if (type == json_type_array)
        {
            /* attribute */
            struct attr_desc *attr = parse_attr(value, key);

            if (!attr)
            {
                fprintf(stderr, "in \"%s\" get a error when parser attributes!\n", name);
                goto err_happend;
            }
            attr_desc_list_insert(&obj->attrs, attr);
            continue;
        }
        /* wrong */
        fprintf(stderr, "object \"%s\" have a unknown attr \"%s\"\n", name, key);
        goto err_happend;
    }

    return obj;
  err_happend:
    obj_desc_destroy(obj);
    return NULL;
#undef PARSE_CHECK_TYPE
}

static struct obj_desc *parse_object_list(struct json_object *root)
{
    struct obj_desc *list = NULL;

    json_object_object_foreach(root, name, object)
    {
        enum json_type type = json_object_get_type(object);

        if (type == json_type_object)
        {
            /* sub object */
            struct obj_desc *subobj = parse_object(object, name);

            if (!subobj)
            {
                goto err_happend;
            }
            obj_desc_list_insert(&list, subobj);
        } else
        {
            /* wrong */
            fprintf(stderr, "top have a undefined object \"%s\"\n", name);
            goto err_happend;
        }
    }
    return list;
  err_happend:
    obj_desc_list_destroy(list);
    return NULL;
}

#if 1
struct obj_desc *parse_string(const char *str)
{
    struct json_object *root;
    struct json_tokener *tok = json_tokener_new();

    root = json_tokener_parse_ex(tok, str, strlen(str));

    enum json_tokener_error jerr = tok->err;

    json_tokener_free(tok);
    if (jerr != json_tokener_success)
    {
        fprintf(stderr, "json parse failed, and maybe there are some thing wrong\n" "Error: %s\n",
#if defined(_json_c_version_h_) && ((JSON_C_MAJOR_VERSION > 0) || (JSON_C_MAJOR_VERSION == 0 && JSON_C_MINOR_VERSION > 9))
                json_tokener_error_desc(jerr));
#else
                json_tokener_errors[jerr]);
#endif
        return NULL;
    }

    struct obj_desc *list = parse_object_list(root);

    json_object_put(root);
    return list;
}

static char *file_get_string(const char *path)
{
    int fd, ret, len;
    char *buf;

    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        perror("open conf file");
        return NULL;
    }

    len = lseek(fd, 0, SEEK_END);
    buf = (char *) malloc(sizeof (char) * len);

    lseek(fd, 0, SEEK_SET);
    ret = read(fd, buf, len - 1);
    if (ret < 0)
    {
        perror("read conf file");
        return NULL;
    }

    buf[len - 1] = '\0';
    return buf;
}

struct obj_desc *parse_file(const char *path)
{
    char *buf;

    if ((buf = file_get_string(path)) == NULL)
    {
        return NULL;
    }
    return parse_string(buf);
}
#else
struct obj_desc *parse_file(const char *path)
{
    struct json_object *root;

    root = json_object_from_file(path);
    if (!root)
    {
        return NULL;
    }
    struct obj_desc *list = parse_object_list(root);

    json_object_put(root);
    return list;
}
#endif

static void pt(int n)
{
    int i;

    for (i = 0; i < n; i++)
    {
        putchar('\t');
    }
}

static void look_attrlist(struct attr_desc *list, int pos)
{
    struct attr_desc *attr;

    for (attr = list; attr; attr = attr->next)
    {
        pt(pos);
        printf("%s[%d] = %s{%s}\n", attr->key, attr->idx, attr->val, attr->type);
    }
}

void look_objlist(struct obj_desc *list, int pos)
{
    struct obj_desc *obj;

    for (obj = list; obj; obj = obj->next)
    {
        pt(pos);
        printf("%s [base: %s, class: %s]", obj->name, obj->base, obj->cls);
        if (obj->subobj || obj->attrs)
        {
            printf(" {\n");
            if (obj->subobj)
            {
                look_objlist(obj->subobj, pos + 1);
            } else
            {
                look_attrlist(obj->attrs, pos + 1);
            }
            pt(pos);
            printf("}\n");
        } else
        {
            printf("\n");
        }
    }
}
