/**
 * @file    json_utils.c
 * @brief   JSON wrapper library using Jansson.
 *
 * This library provides an easy-to-use API for working with JSON data
 * using the Jansson library.
 *
 */

#include "json_utils.h"

//-------------------------------------------------------------------------------------------------
/**
 * @brief Load JSON data from a file.
 * @param[in] filename Path to the JSON file.
 * @param[out] root Pointer to store the loaded JSON object.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_load_from_file(const char *filename, JSON_OBJ **root)
{
    json_error_t error;
    *root = json_load_file(filename, 0, &error);
    if (!*root)
    {
        fprintf(stderr, "Error loading JSON: %s\n", error.text);
        return JSON_ERROR;
    }
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a JSON object from a JSON structure.
 * @param[in] root JSON object.
 * @param[in] tag Key name.
 * @param[out] obj Pointer to store the retrieved object.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_object(JSON_OBJ *root, const char *tag, JSON_OBJ **obj)
{
    *obj = json_object_get(root, tag);
    if (!json_is_object(*obj))
    {
        fprintf(stderr, "Failed to get JSON object for tag: %s\n", tag);
        return JSON_ERROR;
    }
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve an integer value from a JSON object.
 * @param[in] obj JSON object.
 * @param[in] tag Key name.
 * @param[out] val Pointer to store the retrieved integer.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_integer(JSON_OBJ *obj, const char *tag, int *val)
{
    JSON_OBJ *json = json_object_get(obj, tag);
    if (!json_is_integer(json))
    {
        fprintf(stderr, "Failed to get JSON integer for tag: %s\n", tag);
        return JSON_ERROR;
    }
    *val = json_integer_value(json);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a boolean value from a JSON object.
 * @param[in] obj JSON object.
 * @param[in] tag Key name.
 * @param[out] val Pointer to store the retrieved boolean.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_boolean(JSON_OBJ *obj, const char *tag, int *val)
{
    JSON_OBJ *json = json_object_get(obj, tag);
    if (!json_is_boolean(json))
    {
        fprintf(stderr, "Failed to get JSON boolean for tag: %s\n", tag);
        return JSON_ERROR;
    }
    *val = json_boolean_value(json);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a string value from a JSON object.
 * @param[in] obj JSON object.
 * @param[in] tag Key name.
 * @param[out] val Pointer to store the retrieved string.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_string(JSON_OBJ *obj, const char *tag, const char **val)
{
    JSON_OBJ *json = json_object_get(obj, tag);
    if (!json_is_string(json))
    {
        fprintf(stderr, "Failed to get JSON string for tag: %s\n", tag);
        return JSON_ERROR;
    }
    *val = json_string_value(json);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a JSON array.
 * @param[in] obj JSON object.
 * @param[in] tag Key name.
 * @param[out] array Pointer to store the retrieved array.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_array(JSON_OBJ *obj, const char *tag, JSON_OBJ **array)
{
    *array = json_object_get(obj, tag);
    if (!json_is_array(*array))
    {
        fprintf(stderr, "Failed to get JSON array for tag: %s\n", tag);
        return JSON_ERROR;
    }
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve an integer value from a JSON array.
 * @param[in] array JSON array.
 * @param[in] index Index of the element.
 * @param[out] val Pointer to store the retrieved integer.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_integer_from_array(JSON_OBJ *array, size_t index, int *val)
{
    if (!json_is_array(array))
    {
        fprintf(stderr, "Provided JSON is not an array.\n");
        return JSON_ERROR;
    }

    JSON_OBJ *element = json_array_get(array, index);
    if (!json_is_integer(element))
    {
        fprintf(stderr, "Failed to get JSON integer at index %zu\n", index);
        return JSON_ERROR;
    }

    *val = json_integer_value(element);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a string value from a JSON array.
 * @param[in] array JSON array.
 * @param[in] index Index of the element.
 * @param[out] val Pointer to store the retrieved string.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_string_from_array(JSON_OBJ *array, size_t index, const char **val)
{
    if (!json_is_array(array))
    {
        fprintf(stderr, "Provided JSON is not an array.\n");
        return JSON_ERROR;
    }

    JSON_OBJ *element = json_array_get(array, index);
    if (!json_is_string(element))
    {
        fprintf(stderr, "Failed to get JSON string at index %zu\n", index);
        return JSON_ERROR;
    }

    *val = json_string_value(element);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve a boolean value from a JSON array.
 * @param[in] array JSON array.
 * @param[in] index Index of the element.
 * @param[out] val Pointer to store the retrieved boolean (1 for true, 0 for false).
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_boolean_from_array(JSON_OBJ *array, size_t index, int *val)
{
    if (!json_is_array(array))
    {
        fprintf(stderr, "Provided JSON is not an array.\n");
        return JSON_ERROR;
    }

    JSON_OBJ *element = json_array_get(array, index);
    if (!json_is_boolean(element))
    {
        fprintf(stderr, "Failed to get JSON boolean at index %zu\n", index);
        return JSON_ERROR;
    }

    *val = json_boolean_value(element);
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Retrieve an object from a JSON array.
 * @param[in] array JSON array.
 * @param[in] index Index of the element.
 * @param[out] obj Pointer to store the retrieved object.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_get_object_from_array(JSON_OBJ *array, size_t index, JSON_OBJ **obj)
{
    if (!json_is_array(array))
    {
        fprintf(stderr, "Provided JSON is not an array.\n");
        return JSON_ERROR;
    }

    *obj = json_array_get(array, index);
    if (!json_is_object(*obj))
    {
        fprintf(stderr, "Failed to get JSON object at index %zu\n", index);
        return JSON_ERROR;
    }

    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Set an integer value in a JSON object.
 * @param[in,out] obj JSON object.
 * @param[in] tag Key name.
 * @param[in] val Integer value to set.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_set_integer(JSON_OBJ *obj, const char *tag, int val)
{
    return (json_object_set_new(obj, tag, json_integer(val)) == 0) ? JSON_SUCCESS : JSON_ERROR;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Set a boolean value in a JSON object.
 * @param[in,out] obj JSON object.
 * @param[in] tag Key name.
 * @param[in] val Boolean value to set.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_set_boolean(JSON_OBJ *obj, const char *tag, int val)
{
    return (json_object_set_new(obj, tag, json_boolean(val)) == 0) ? JSON_SUCCESS : JSON_ERROR;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Set a string value in a JSON object.
 * @param[in,out] obj JSON object.
 * @param[in] tag Key name.
 * @param[in] val String value to set.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_set_string(JSON_OBJ *obj, const char *tag, const char *val)
{
    return (json_object_set_new(obj, tag, json_string(val)) == 0) ? JSON_SUCCESS : JSON_ERROR;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Create a new JSON object.
 * @return Pointer to a new JSON object.
 */
JSON_OBJ *json_create_object()
{
    return json_object();
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Save JSON data to a file.
 * @param[in] root JSON object.
 * @param[in] filename Path to the file.
 * @return JSON_SUCCESS on success, JSON_ERROR on failure.
 */
int json_save_to_file(JSON_OBJ *root, const char *filename)
{
    if (json_dump_file(root, filename, JSON_INDENT(4)) != 0)
    {
        fprintf(stderr, "Failed to save JSON to file: %s\n", filename);
        return JSON_ERROR;
    }
    return JSON_SUCCESS;
}

//-------------------------------------------------------------------------------------------------
/**
 * @brief Free a JSON object.
 * @param[in] obj JSON object to free.
 */
void json_free(JSON_OBJ *obj)
{
    json_decref(obj);
}
