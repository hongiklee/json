#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "json.h"

typedef struct json_string_t {
    json_type type;
    char *value;
} json_string_t;

typedef struct json_number_t {
    json_type type;
    long double value;
} json_number_t;

typedef struct json_object_value_t {
    char *key;
    json_t *value;

    struct json_object_value_t *prev;
    struct json_object_value_t *next;
} json_object_value_t;

typedef struct json_object_t {
    json_type type;
    int size;
    struct json_object_value_t *first;
    struct json_object_value_t *last;
} json_object_t;

typedef struct json_array_value_t {
    json_t *value;

    struct json_array_value_t *prev;
    struct json_array_value_t *next;
} json_array_value_t;

typedef struct json_array_t {
    json_type type;
    int size;
    struct json_array_value_t *first;
    struct json_array_value_t *last;
} json_array_t;

typedef struct json_true_t {
    json_type type;
} json_true_t;

typedef struct json_false_t {
    json_type type;
} json_false_t;

typedef struct json_null_t {
    json_type type;
} json_null_t;

#define json_typeof(j)      j->type
#define json_is_string(j)   (j && j->type == JSON_STRING)
#define json_is_number(j)   (j && j->type == JSON_NUMBER)
#define json_is_object(j)   (j && j->type == JSON_OBJECT)
#define json_is_array(j)    (j && j->type == JSON_ARRAY)
#define json_is_true(j)     (j && j->type == JSON_TRUE)
#define json_is_false(j)    (j && j->type == JSON_FALSE)
#define json_is_null(j)     (j && j->type == JSON_NULL)

#define MIN_DUMP_SIZE   1024

int json_destroy(json_t *json)
{
    if ( ! json)
        return 1;

    switch (json_typeof(json)) {
        case JSON_STRING:
            free(((json_string_t *)json)->value);
            free(json);
            break;
        case JSON_NUMBER:
            free(json);
            break;
        case JSON_OBJECT:
            {
                struct json_object_value_t *current;
                struct json_object_value_t *next;

                current = ((json_object_t *)json)->first;
                while (current) {
                    next = current->next;
                    json_destroy(current->value);
                    free(current->key);
                    free(current);
                    current = next;
                }
                free(json);
            }
            break;
        case JSON_ARRAY:
            {
                struct json_array_value_t *current;
                struct json_array_value_t *next;

                current = ((json_array_t *)json)->first;
                while (current) {
                    next = current->next;
                    json_destroy(current->value);
                    free(current);
                    current = next;
                }
                free(json);
            }
            break;
        case JSON_TRUE:
        case JSON_FALSE:
        case JSON_NULL:
            break;
    }

    return 0;
}

json_t *json_string(const char *value)
{
    json_string_t *string;

    if ( ! value)
        return NULL;

    string = (json_string_t *)malloc(sizeof(json_string_t));
    if ( ! string)
        return NULL;

    string->type = JSON_STRING;
    string->value = strdup(value);

    return (json_t *)string;
}

char *json_string_get(json_t *json)
{
    if ( ! json_is_string(json))
        return NULL;

    return ((json_string_t *)json)->value;
}

json_t *json_number(long double value)
{
    json_number_t *number;

    number = (json_number_t *)malloc(sizeof(json_number_t));
    if ( ! number)
        return NULL;

    number->type = JSON_NUMBER;
    number->value = value;

    return (json_t *)number;
}

long double json_number_get(json_t *json)
{
    if ( ! json_is_number(json))
        return 0;

    return ((json_number_t *)json)->value;
}

json_t *json_object()
{
    json_object_t *obj;

    obj = (json_object_t *)malloc(sizeof(json_object_t));
    if ( ! obj)
        return NULL;

    obj->type = JSON_OBJECT;
    obj->size = 0;
    obj->first = obj->last = NULL;

    return (json_t *)obj;
}

int json_object_sizeof(json_t *json)
{
    if ( ! json_is_object(json))
        return -1;

    return ((json_object_t *)json)->size;
}

static json_object_value_t *json_object_get_value(json_object_t *obj,
        const char *key)
{
    struct json_object_value_t *ov;

    ov = obj->first;
    while (ov) {
        if ( ! strncmp(ov->key, key, strlen(ov->key))) {
            return ov;
        }

        ov = ov->next;
    }

    return NULL;
}

int json_object_set(json_t *json, const char *key, json_t *value)
{
    json_object_value_t *ov = NULL;

    if ( ! json_is_object(json))
        return 1;

    ov = json_object_get_value((json_object_t *)json, key);
    if (ov) {
        ov->value = value;
    } else {
        ov = (json_object_value_t *)malloc(sizeof(json_object_value_t));
        if ( ! ov)
            return 1;

        ov->key = strdup(key);
        ov->value = value;

        if (((json_object_t *)json)->first) {
            ov->prev = ((json_object_t *)json)->last;
            ov->next = NULL;
            ((json_object_t *)json)->last->next = ov;
            ((json_object_t *)json)->last = ov;
            ((json_object_t *)json)->size++;
        } else {
            ov->prev = ov->next = NULL;
            ((json_object_t *)json)->first =
                ((json_object_t *)json)->last = ov;
            ((json_object_t *)json)->size = 1;
        }
    }

    return 0;
}

json_t *json_object_get(json_t *json, const char *key)
{
    struct json_object_value_t *ov;

    ov = json_object_get_value((json_object_t *)json, key);
    if (ov) {
        return ov->value;
    }

    return NULL;
}

char *json_object_get_key(json_t *json, int index)
{
    int i;
    struct json_object_value_t *ov;

    if ( ! json_is_object(json))
        return NULL;

    if (json_object_sizeof(json) < index)
        return NULL;

    ov = ((json_object_t *)json)->first;
    for (i = 0; i < index; i++) {
        if (ov == NULL)
            return NULL;
        ov = ov->next;
    }

    return ov->key;
}

int json_object_remove(json_t *json, const char *key, int destroy)
{
    json_object_t *obj;
    struct json_object_value_t *ov; 

    if ( ! json_is_object(json))
        return 1;

    obj = (json_object_t *)json;

    if (obj->size <= 0)
        return 1;

    ov = obj->first;
    while (ov) {
        if ( ! strncmp(ov->key, key, strlen(ov->key))) {
            if (ov->prev) {
                if (ov->next) {
                    ov->prev->next = ov->next;
                    ov->next->prev = ov->prev;
                } else {
                    ov->prev->next = NULL;
                    obj->last = ov->prev;
                }
            } else {
                if (ov->next) {
                    ov->next->prev = NULL;
                    obj->first = ov->next;
                } else {
                    obj->first= NULL;
                    obj->last = NULL;
                }
            }

            if (destroy) {
                json_destroy(ov->value);
            }

            obj->size--;
            free(ov->key);
            free(ov);

            return 0;
        }

        ov = ov->next;
    }

    return 1;
}

json_t *json_array()
{
    json_array_t *array;

    array = (json_array_t *)malloc(sizeof(json_array_t));
    if ( ! array)
        return NULL;

    array->type = JSON_ARRAY;
    array->size = 0;
    array->first = array->last = NULL;

    return (json_t *)array;
}

int json_array_sizeof(json_t *json)
{
    if ( ! json_is_array(json))
        return -1;

    return ((json_array_t *)json)->size;
}

json_t *json_array_get(json_t *json, int index)
{
    int i, size;
    
    json_array_value_t *array_value;

    if ( ! json_is_array(json))
        return NULL;

    size = json_array_sizeof(json);
    if (size <= 0 || size < index)
        return NULL;

    array_value = ((json_array_t *)json)->first;
    for (i = 0; i < size; i++) {
        if (i == index)
            return array_value->value;
        array_value = array_value->next;
    }

    return NULL;
}

int json_array_set(json_t *json, json_t *value)
{
    json_array_value_t *array_value = NULL;

    if ( ! json_is_array(json))
        return 1;
    if ( ! value)
        return 1;

    array_value = (json_array_value_t *)malloc(sizeof(json_array_value_t));
    if ( ! array_value)
        return 1;

    array_value->value = value;

    if (((json_array_t *)json)->first) {
        array_value->prev = ((json_array_t *)json)->last;
        array_value->next = NULL;
        ((json_array_t *)json)->last->next = array_value;
        ((json_array_t *)json)->last = array_value;
        ((json_array_t *)json)->size++;
    } else {
        array_value->prev = array_value->next = NULL;
        ((json_array_t *)json)->first =
            ((json_array_t *)json)->last = array_value;
        ((json_array_t *)json)->size = 1;
    }

    return 0;
}

json_t *json_true()
{
    static json_type type = JSON_TRUE;
    return (json_t *)&type;
}

json_t *json_false()
{
    static json_type type = JSON_FALSE;
    return (json_t *)&type;
}

json_t *json_null()
{
    static json_type type = JSON_NULL;
    return (json_t *)&type;
}

static int append_string(const char *src, int len, char **dst, int *size)
{
    int dlen;

    if ( ! src || ! dst || ! size)
        return 0;
   
    dlen = strlen(*dst) + len;

    if (*size <= dlen) {
        *dst = realloc(*dst, *size + len + MIN_DUMP_SIZE);
        if ( ! dst) {
            return 0;
        }
        *size += len + MIN_DUMP_SIZE;
    }

    strncat(*dst, src, len);

    return len;
}

static int dump(json_t *json, char **buf, int *size)
{
    int i;
    int len;

    if (json == NULL)
        return 1;

    switch (json_typeof(json)) {
        case JSON_STRING:
            {
                char *tmp;
                char *value = json_string_get(json);

                tmp = (char *)malloc(strlen(value) + 3);
                if ( ! tmp)
                    return 1;
                len = sprintf(tmp, "\"%s\"", value);
                append_string(tmp, len, buf, size);
                free(tmp);
            }
            break;
        case JSON_NUMBER:
            {
                char tmp[32] = {0, };
                long double value = json_number_get(json);
                len = sprintf(tmp, "%.17Lg", value);
                append_string(tmp, len, buf, size);
            }
            break;
        case JSON_OBJECT:
            {
                int obj_size;
                char *key;

                obj_size = json_object_sizeof(json);

                append_string("{", 1, buf, size);
                for (i = 0; i < obj_size; i++) {
                    key = json_object_get_key(json, i);
                    if ( ! key)
                        continue;
                    json_t *j = json_object_get(json, key);
                    if ( ! j)
                        continue;

                    append_string(key, strlen(key), buf, size);
                    append_string(":", 1, buf, size);
                    dump(j, buf, size);
                    if (i + 1 < obj_size)
                        append_string(", ", 2, buf, size);
                }
                append_string("}", 1, buf, size);
            }
            break;
        case JSON_ARRAY:
            {
                int array_size;

                array_size = json_array_sizeof(json);

                append_string("[", 1, buf, size);
                for (i = 0; i < array_size; i++) {
                    json_t *j = json_array_get(json, i);
                    if ( ! j)
                        continue;

                    dump(j, buf, size);
                    if (i + 1 < array_size)
                        append_string(", ", 2, buf, size);
                }
                append_string("]", 1, buf, size);
            }
            break;
        case JSON_TRUE:
            append_string("true", 4, buf, size);
            break;
        case JSON_FALSE:
            append_string("false", 5, buf, size);
            break;
        case JSON_NULL:
            append_string("null", 4, buf, size);
            break;
    }

    return 0;
}

char *json_dump(json_t *json)
{
    int size = MIN_DUMP_SIZE;
    char *output = (char *)malloc(size);
    if ( ! output)
        return NULL;
    output[0] = '\0';

    dump(json, &output, &size);

    return output;
}

json_t *json_parse(const char *data, int len)
{
    int i, start;

    for (start = 0; start < len; start++) {
        if ( ! isspace(data[start]))
            break;
    }
    for (i = len - 1; i >= start; i++) {
        if ( ! isspace(data[i]))
            break;
        len--;
    }
    if (start + 1 > len)
        goto error;

    if (data[start] == '"') {
        char *value;
        json_t *json;
        for (i = start + 1; i < len; i++) {
            if (data[i] == '"' && data[i - 1] != '\\') {
                if ((i - start) > 1) {
                    value = strndup(data + start + 1, i - start - 1);
                    if ( ! value)
                        goto error;
                    json = json_string(value);
                    free(value);
                    return json;
                } else {
                    return json_string("");
                }
            }
        }
    } else if (data[start] == '-'
            || (data[start] >= '0' && data[start] <= '9')) {
        char *parsed;
        long double nv;
        nv = strtold(data, &parsed);
        i = parsed - data - 1;
        if (i > len)
            goto error;
        return json_number(nv);
    } else if (data[start] == '[' || data[start] == '{') {
        int j;
        int quotation, bracket, brace, done;
        char *key = NULL;
        json_t *root, *child;

        quotation = bracket = brace = done = 0;
        if (data[start] == '[')
            root = json_array();
        else if (data[start] == '{')
            root = json_object();
        else
            goto error;

        done = start + 1;
        for (i = done; i < len; i++) {
            if (data[i - 1] != '\\' && data[i] == '"') {
                quotation = ! quotation;
            }
            if (quotation)
                continue;
            if (data[i] == '[') {
                bracket++;
            } else if (data[i] == '{') {
                brace++;
            } else if (data[i] == ':') {
                if (data[start] == '{' && ! bracket && ! brace) {
                    for (j = done; j < i; j++, done++)
                        if ( ! isspace(data[j]))
                            break;
                    for (j = i - 1; j >= done; j--) {
                        if ( ! isspace(data[j]))
                            break;
                    }
                    key = strndup(&data[done], j - done + 1);
                    if ( ! key)
                        goto error;
                    done = i + 1;
                }
            } else if (data[i] == ',' || data[i] == ']' || data[i] == '}') {
                if (data[start] == '[' && ! bracket && ! brace) {
                    //printf("[%.*s]\n", i - done, &data[done]);
                    child = json_parse(&data[done], i - done);
                    if (child)
                        json_array_set(root, child);
                    done = i;
                } else if (data[start] == '{' && ! bracket && ! brace) {
                    if (key) {
                        //printf("key %s {%.*s}\n", key, i - done, &data[done]);
                        child = json_parse(&data[done], i - done);
                        if (child)
                            json_object_set(root, key, child);
                        free(key);
                        key = NULL;
                    } else {
                        goto error;
                    }
                    done = i;
                }
                if (data[i] == ',' && ! bracket && ! brace)
                    done++;
            }

            if (data[i] == ']') {
                bracket--;
            } else if (data[i] == '}') {
                brace--;
            }
        }

        if (data[start] == '[')
            bracket++;
        if (data[start] == '{')
            brace++;

        if (bracket || brace)
            goto error;

        return root;
    } else if (len - start >= 4 && ! strncasecmp(data + start, "true", 4)) {
        return json_true();
    } else if (len - start >= 5 && ! strncasecmp(data + start, "false", 5)) {
        return json_false();
    } else if (len - start >= 4 && ! strncasecmp(data + start, "null", 4)) {
        return json_null();
    }

error:
    fprintf(stderr, "parse error : %.*s\n", len - start, data + start);
    return NULL;
}
