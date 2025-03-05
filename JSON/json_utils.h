/**
 * @file json_utils.h
 * @brief JSON wrapper library using Jansson.
 *
 * This library provides an easy-to-use API for working with JSON data
 * using the Jansson library.
 *
 */

#ifndef JSON_UTILS_H
#define JSON_UTILS_H

#include <jansson.h>

/** Success return code */
#define JSON_SUCCESS 0
/** Failure return code */
#define JSON_ERROR   1

#ifdef __cplusplus
extern "C"
{
#endif

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Load JSON data from a file.
     * @param[in] filename Path to the JSON file.
     * @param[out] root Pointer to store the loaded JSON object.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_load_from_file(const char *filename, json_t **root);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a JSON object from a JSON structure.
     * @param[in] root JSON object.
     * @param[in] tag Key name.
     * @param[out] obj Pointer to store the retrieved object.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_get_object(json_t *root, const char *tag, json_t **obj);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve an integer value from a JSON object.
     * @param[in] obj JSON object.
     * @param[in] tag Key name.
     * @param[out] val Pointer to store the retrieved integer.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_get_integer(json_t *obj, const char *tag, int *val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a boolean value from a JSON object.
     * @param[in] obj JSON object.
     * @param[in] tag Key name.
     * @param[out] val Pointer to store the retrieved boolean.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_get_boolean(json_t *obj, const char *tag, int *val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a string value from a JSON object.
     * @param[in] obj JSON object.
     * @param[in] tag Key name.
     * @param[out] val Pointer to store the retrieved string.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_get_string(json_t *obj, const char *tag, const char **val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a JSON array.
     * @param[in] obj JSON object.
     * @param[in] tag Key name.
     * @param[out] array Pointer to store the retrieved array.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_get_array(json_t *obj, const char *tag, json_t **array);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve an integer value from a JSON array.
     * @param[in] array JSON array.
     * @param[in] index Index of the element.
     * @param[out] val Pointer to store the retrieved integer.
     * @return JSON_SUCCESS on success, JSON_ERROR on failure.
     */
    int json_get_integer_from_array(json_t *array, size_t index, int *val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a string value from a JSON array.
     * @param[in] array JSON array.
     * @param[in] index Index of the element.
     * @param[out] val Pointer to store the retrieved string.
     * @return JSON_SUCCESS on success, JSON_ERROR on failure.
     */
    int json_get_string_from_array(json_t *array, size_t index, const char **val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a boolean value from a JSON array.
     * @param[in] array JSON array.
     * @param[in] index Index of the element.
     * @param[out] val Pointer to store the retrieved boolean (1 for true, 0 for false).
     * @return JSON_SUCCESS on success, JSON_ERROR on failure.
     */
    int json_get_boolean_from_array(json_t *array, size_t index, int *val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Retrieve a JSON object from a JSON array.
     * @param[in] array JSON array.
     * @param[in] index Index of the element.
     * @param[out] obj Pointer to store the retrieved object.
     * @return JSON_SUCCESS on success, JSON_ERROR on failure.
     */
    int json_get_object_from_array(json_t *array, size_t index, json_t **obj);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Set an integer value in a JSON object.
     * @param[in,out] obj JSON object.
     * @param[in] tag Key name.
     * @param[in] val Integer value to set.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_set_integer(json_t *obj, const char *tag, int val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Set a boolean value in a JSON object.
     * @param[in,out] obj JSON object.
     * @param[in] tag Key name.
     * @param[in] val Boolean value to set.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_set_boolean(json_t *obj, const char *tag, int val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Set a string value in a JSON object.
     * @param[in,out] obj JSON object.
     * @param[in] tag Key name.
     * @param[in] val String value to set.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_set_string(json_t *obj, const char *tag, const char *val);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Create a new JSON object.
     * @return Pointer to a new JSON object.
     */
    json_t *json_create_object(void);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Save JSON data to a file.
     * @param[in] root JSON object.
     * @param[in] filename Path to the file.
     * @return SUCCESS on success, FAIL on failure.
     */
    int json_save_to_file(json_t *root, const char *filename);

    //-------------------------------------------------------------------------------------------------
    /**
     * @brief Free a JSON object.
     * @param[in] obj JSON object to free.
     */
    void json_free(json_t *obj);

#ifdef __cplusplus
}
#endif

#endif  // JSON_UTILS_H
