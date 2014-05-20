#ifndef JSON_H
#define JSON_H

typedef enum {
    JSON_STRING,
    JSON_NUMBER,
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_TRUE,
    JSON_FALSE,
    JSON_NULL
} json_type;

typedef struct json_t {
    json_type type;
} json_t;

#define json_typeof(j)      j->type
#define json_is_string(j)   (j && j->type == JSON_STRING)
#define json_is_number(j)   (j && j->type == JSON_NUMBER)
#define json_is_object(j)   (j && j->type == JSON_OBJECT)
#define json_is_array(j)    (j && j->type == JSON_ARRAY)
#define json_is_true(j)     (j && j->type == JSON_TRUE)
#define json_is_false(j)    (j && j->type == JSON_FALSE)
#define json_is_null(j)     (j && j->type == JSON_NULL)

int json_destroy(json_t *json);
json_t *json_string(const char *value);
char *json_string_get(json_t *json);
json_t *json_number(long double value);
long double json_number_get(json_t *json);
json_t *json_object();
int json_object_sizeof(json_t *json);
int json_object_set(json_t *json, const char *key, json_t *value);
json_t *json_object_get(json_t *json, const char *key);
char *json_object_get_key(json_t *json, int index);
int json_object_remove(json_t *json, const char *key, int destroy);
json_t *json_array();
int json_array_sizeof(json_t *json);
json_t *json_array_get(json_t *json, int index);
int json_array_set(json_t *json, json_t *value);
json_t *json_true();
json_t *json_false();
json_t *json_null();
char *json_dump(json_t *json);
json_t *json_parse(const char *data, int len);

#endif /* JSON_H */
