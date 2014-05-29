#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"

#define TEST_JSON_DATA                      \
    "[\n"                                   \
    "    {\n"                               \
    "        \"id\"     : 1,\n"             \
    "        \"name\"   : \"Jone Doe\",\n"  \
    "        \"sex\"    : \"male\",\n"      \
    "        \"height\" : 181.53\n"         \
    "    },\n"                              \
    "    {\n"                               \
    "        \"id\"     : 2,\n"             \
    "        \"name\"   : \"Jane Doe\",\n"  \
    "        \"sex\"    : \"female\",\n"    \
    "        \"height\" : 167.81\n"         \
    "    }\n"                               \
    "]\n"

int main(int argc, char **argv)
{
    char *dumps;
    json_t *arr, *obj, *par;

    // array test
    arr = json_array();
    json_array_set(arr, json_string("apple"));
    json_array_set(arr, json_string("banana"));
    json_array_set(arr, json_string("cherry"));
    json_array_set(arr, json_string("date"));
    json_array_set(arr, json_string("elderberry"));
    json_array_set(arr, json_string("fig"));
    json_array_set(arr, json_string("grape"));
    dumps = json_dump(arr);
    printf("Array : %s\n", dumps);
    free(dumps);

    // object test
    obj = json_object();
    json_object_set(obj, "Dummy", json_string("Dummy"));
    json_object_set(obj, "String", json_string("Value"));
    json_object_set(obj, "Real", json_number(0.123456789123456789L));
    json_object_set(obj, "Null", json_null());
    json_object_set(obj, "True", json_true());
    json_object_set(obj, "False", json_false());
    json_object_set(obj, "Array", arr);
    json_object_remove(obj, "Dummy", 1);
    dumps = json_dump(obj);
    printf("Object : %s\n", dumps);
    free(dumps);
    json_destroy(obj);

    // parding test
    par = json_parse(TEST_JSON_DATA, strlen(TEST_JSON_DATA));
    dumps = json_dump(par);
    printf("Parsing : %s\n", dumps);
    free(dumps);
    json_destroy(par);

    return 0;
}
