#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "json.h"

int main(int argc, char **argv)
{
    char *dumps;

    // array test
    json_t *arr = json_array();
    json_array_set(arr, json_number(123456789));
    json_array_set(arr, json_true());
    json_array_set(arr, json_false());
    json_array_set(arr, json_null());

    // object test
    json_t *obj = json_object();
    json_object_set(obj, "Dummy", json_string("HelloWorld"));
    json_object_set(obj, "String", json_string("Value"));
    json_object_set(obj, "Real", json_number(0.123456789123456789L));
    json_object_set(obj, "NULL", json_null());
    json_object_set(obj, "Array", arr);

    dumps = json_dump(obj);
    puts(dumps);
    free(dumps);

    json_object_remove(obj, "Dummy", 1);

    dumps = json_dump(obj);
    puts(dumps);
    free(dumps);
    json_destroy(obj);

    char *test_json = "                [ { \"id\": 0, \"guid\": \"fca06a58-4c30-4e88-92c9-e7c4cfb53996\", \"isActive\": true, \"balance\": \"$2,582.86\", \"picture\": \"http://placehold.it/32x32\", \"age\": 28, \"eyeColor\": \"blue\", \"name\": \"Charity Dickerson\", \"gender\": \"female\", \"company\": \"DADABASE\", \"email\": \"charitydickerson@dadabase.com\", \"phone\": \"+1 (877) 549-3987\", \"address\": \"762 Plymouth Street, Bloomington, Arizona, 1609\", \"about\": \"Quis ea commodo nulla labore. Quis pariatur ut elit ea do ad mollit ipsum. Ad magna reprehenderit fugiat sit mollit eu deserunt amet mollit consequat. Proident ea elit Lorem officia sit amet deserunt et minim fugiat ipsum est esse. Eiusmod incididunt deserunt incididunt irure aliqua. Nulla Lorem exercitation eu qui commodo ex proident velit.\", \"registered\": \"2014-05-06T14:05:54 -09:00\", \"latitude\": 82.014554, \"longitude\": -24.227151, \"tags\": [ \"sit\", \"nisi\", \"eu\", \"do\", \"deserunt\", \"amet\", \"sit\" ], \"friends\": [ { \"id\": 0, \"name\": \"Chaney Beck\" }, { \"id\": 1, \"name\": \"Brooke Hoffman\" }, { \"id\": 2, \"name\": \"Diaz Obrien\" } ], \"greeting\": \"Hello, Charity Dickerson! You have 2 unread messages.\", \"favoriteFruit\": \"banana\" } ]        ";
    json_t *test = json_parse(test_json, strlen(test_json));
    dumps = json_dump(test);
    puts(test_json);
    puts(dumps);
    printf("Test is %s\n", strcmp(test_json, dumps) ? "NG" : "OK");
    free(dumps);
    json_destroy(test);

    return 0;
}
