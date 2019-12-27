#ifndef API_H
#define API_H

#include "lime.h"
#include "module.h"

/**
 * A helper data structure that allows to return either a result value or an
 * exception.
 */
typedef struct {
    /** A flag indicating if this result is a success or failure. */
    bool failure;
    union {
        /** The value of this result if it is a success. */
        LimeValue value;
        /** The value of this result if it is a failure. */
        LimeValue exception;
    };
} LimeResult;

/**
 * Pops a value from the datastack or returns an exception value if the datastack is
 * empty.
 * @param stack The current stack of the environment.
 * @param variable A pointer to a memory location where the popped value should be stored.
 */
#define LIME_STACK_POP_VALUE(stack, variable) \
if (*(stack)->datastack == NULL) {\
    return lime_exception((stack), "stack underflow in function '%s'", __FUNCTION__);\
}\
*(variable) = (*(stack)->datastack)->list.head;\
*(stack)->datastack = (*(stack)->datastack)->list.tail;

/**
 * Assigns the provided 'value' to the given 'temporary' variable and checks 
 * whether the computation of this value was successful (in which case the 
 * result value is assigned to the provided 'variable') or not (in which case
 * the exception is returned).
 * @param variable A memory location where the resulting value should be stored.
 * @param temporary A memory location where the value should be temporarily stored.
 * @param A value of type 'LimeResult'. 
 */
#define LIME_TRY_USE_TEMPORARY(variable, temporary, value) \
*(temporary) = (value);\
if ((temporary)->failure) {\
    return (temporary)->exception;\
} else {\
    *(variable) = (temporary)->_LIME_RESULT_VALUE_FIELD_NAME;\
}

/**
 * Creates a new temporary variable, assigns the provided 'value' to it and 
 * checks whether the computation of this value was successful (in which case 
 * the result value is assigned to the provided 'variable') or not (in which 
 * case the exception is returned).
 * @param variable A memory location where the resulting value should be stored.
 * @param temporary A name for the temporary variable.
 * @param A value of type 'LimeResult'. 
 */
#define LIME_TRY_USE_FRESH_TEMPORARY(variable, temporary, value) {\
    LimeResult temporary;\
    LIME_TRY_USE_TEMPORARY(variable, &temporary, value);\
}

/**
 * Checks whether the computation of the provided value was successful (in 
 * which case the result value is assigned to the provided 'variable') or 
 * not (in which case the exception is returned).
 * A temporary variable named '_result__LINE__' (where __LINE__ is the current
 * line number) is used for this purpose. That is, neither the provided variable
 * nor the value should contain an identifier that is equally named.
 * @param variable A memory location where the resulting value should be stored.
 * @param A value of type 'LimeResult'. 
 */
#define LIME_TRY(variable, value) LIME_TRY_USE_FRESH_TEMPORARY(variable, CONCAT(_result, __LINE__), value)

/**
 * The name of the 'value' field for the 'LimeResult' type.
 * @internal
 */
#define _LIME_RESULT_VALUE_FIELD_NAME value

/**
 * The name of the 'type' field for the 'LimeValue' type.
 * @internal
 */
#define _LIME_VALUE_TYPE_FIELD_NAME type

/**
 * Asserts that the given value has the provided type and throws an exception
 * if this is not the case.
 * @param value The value whose type should be checked.
 * @param type The expected type.
 */
#define LIME_ASSERT_TYPE(value, type) \
if (((value) == NULL && (type) != LimeListValue) || ((value) != NULL && (value)->_LIME_VALUE_TYPE_FIELD_NAME != (type))) {\
    return lime_exception((stack), "argument of illegal type '%s' in function '%s' (expected type '%s')", lime_value_type(value), __FUNCTION__, lime_type_name(type));\
}

/**
 * Returns a designated initializer for a static string constant. 
 * @param string The string literal.
 * @return A designated initializer for the type 'LimeValue'.
 */
#define LIME_STRING_STATIC_CONSTANT(string) {\
    .type = LimeStringValue,\
    .location = NULL,\
    .hash = 0,\
    .symbol = {\
        .length = sizeof(string) - 1,\
        .bytes = string\
    }\
}

/**
 * Assigns the provided 'value' to the given 'temporary' variable and checks 
 * whether the computation of the provided value was successful (in which 
 * case the result value is pushed to the datastack) or not (in which case 
 * the exception is returned).
 * @param stack The stack of the current environment.
 * @param temporary A memory location where the value should be temporarily stored.
 * @param A value of type 'LimeResult'.
 */
#define LIME_STACK_PUSH_VALUE_USE_TEMPORARY(stack, temporary, value) LIME_TRY_USE_TEMPORARY((stack)->datastack, (temporary), lime_list((stack), (value), *((stack)->datastack)))

/**
 * Creates a new temporary variable, assigns the provided 'value' to it and 
 * checks whether the computation of the provided value was successful (in 
 * which case the result value is pushed to the datastack) or not (in which 
 * case the exception is returned).
 * @param stack The stack of the current environment.
 * @param temporary A name for the temporary variable.
 * @param A value of type 'LimeResult'.
 */
#define LIME_STACK_PUSH_VALUE_USE_FRESH_TEMPORARY(stack, temporary, value) {\
    LimeResult temporary;\
    LIME_STACK_PUSH_VALUE_USE_TEMPORARY(stack, &temporary, value);\
}

/**
 * Checks whether the computation of the provided value was successful (in 
 * which case the result value is pushed to the datastack) or not (in which 
 * case the exception is returned).
 * A temporary variable named '_result__LINE__' (where __LINE__ is the current
 * line number) is used for this purpose. That is, neither the provided stack
 * nor the value should contain an identifier that is equally named.
 * @param stack The stack of the current environment.
 * @param A value of type 'LimeResult'.
 */
#define LIME_STACK_PUSH_VALUE(stack, value) LIME_STACK_PUSH_VALUE_USE_FRESH_TEMPORARY(stack, CONCAT(_result, __LINE__), value)

/**
 * The path to the boot image as defined by the CLI.
 */
extern const char *lime_settings_boot_path;

/**
 * The path to the kernel library as defined by the CLI.
 */
extern const char *lime_settings_kernel_path;

/**
 * The level of verbosity as defined by the CLI.
 */
extern u64 lime_settings_verbosity;

/**
 * A preallocated sentinel value which can be used for any purpose where it is
 * required to pass dummy data that is not used in any meaningful way.
 * 
 * A common example of this value's application is its use as the default argument
 * for the 'lime_map_get_or_else' function to check if the provided key was present
 * in the map.
 */
extern const LimeValue lime_sentinel_value;

/**
 * A preallocated string exception which is used whenever the finalization of
 * a native library failed.
 */
extern const LimeValue lime_exception_finalization_failure;

/**
 * A preallocated string exception which is used whenever there is not enough 
 * heap memory available.
 */
extern const LimeValue lime_exception_out_of_heap;

/**
 * Enters the provided function in the dictionary of the current environment.
 * @param stack The current stack of the environment.
 * @param name The name of the function.
 * @param library The library value to which the function belongs. This value may
 * be 'NULL' if the function belongs to no native library.
 * @param function The function which should be entered.
 * @return Either an exception or 'NULL' if no exception occurred.
 */
extern LimeValue lime_register_function(LimeStack stack, char *name, LimeValue library, LimeFunction function);

/**
 * Prints the given value after the provided message.
 * @param value The value to print.
 * @param message The message to print.
 * @param ... The data which is used to format the message.
 */
extern void lime_value_debug(LimeValue value, char *message, ...);

/**
 * Triggers a run of the garbage collector. 
 * @param stack The current stack of the environment.
 * @return Either an exception or 'NULL' if no exception occurred.
 */
extern LimeValue lime_collect_garbage(LimeStack stack);

/**
 * Tokenizes the provided string by separating values at white space characters.
 * @param stack The current stack of the environment.
 * @param string The string which should be tokenized.
 * @return Either a list of all tokens or an exception.
 */
extern LimeResult lime_tokens(LimeStack stack, LimeValue string);

/**
 * Allocates memory for the provided value type including any additional bytes.
 * Value type dependent fields are not initialized by this function. That is,
 * to ensure that the garbage collector can work correctly, it is required to
 * initialize all structure members before any other allocation.
 * @param stack The current stack of the environment.
 * @param type The type of the value.
 * @param additional The number of additional bytes which should be allocated.
 * @return Either the allocated value or the exception.
 */
extern LimeResult lime_allocate(LimeStack stack, LimeValueType type, u64 additional);

/**
 * Creates an exact copy of the provided value.
 * @param stack The current stack of the environment.
 * @param value The value which should be cloned.
 * @return Either the copy of the provided value or the exception.
 */
extern LimeResult lime_value_clone(LimeStack stack, LimeValue value);

/**
 * Deletes the specified key from the provided map.
 * @param stack The stack of the current environment.
 * @param map The map whose key should be deleted.
 * @param key The key to delete.
 * @return Either the map where the provided key is not present anymore or an exception.
 */
LimeResult lime_map_delete(LimeStack stack, LimeValue map, LimeValue key);

/**
 * Merges the two provided maps into a new one.
 * @param stack The stack of the current environment.
 * @param a The first map.
 * @param b The second map.
 * @return Either a map which contains all the keys of the two provided ones or an exception.
 */
LimeResult lime_map_merge(LimeStack stack, LimeValue a, LimeValue b);

/** 
 * Retrieves the value from the provided map which is associated with the specified
 * key or returns a default value if there is no value associated with the key.
 * @param map A value of type 'map'.
 * @param key An arbitrary value which represents the key.
 * @param otherwise An arbitrary default value. The 'lime_sentinel_value' may be used
 * to check if there is a value associated with the provided key.
 * @return Either the associated value or the default value.
 */
extern LimeValue lime_map_get_or_else(LimeValue map, LimeValue key, LimeValue otherwise);

/**
 * Tests whether the second argument is a subset of the first one.
 * @param superset A value of type 'map'.
 * @param subset A value of type 'map'.
 * @return Whether or not the second argument is a subset of the first one.
 */
extern bool lime_map_subset_of(LimeValue superset, LimeValue subset);

/**
 * Inserts the value at the specified key in the provided map. 
 * If there is already a value associated with this key, the old value will 
 * be overwritten by the new one.
 * @param stack The current stack of the environment.
 * @param map The map in which the value should be inserted.
 * @param key The key which should be associated with the value.
 * @param value The value which should be inserted.
 * @return A copy of the provided map where the specified key is associated 
 * with the value.
 */
extern LimeResult lime_map_put(LimeStack stack, LimeValue map, LimeValue key, LimeValue value);

/**
 * Generates a null-terminated string of the provided value by allocating
 * a fresh copy on the heap. That is, the returned must be freed by the user
 * if it is no longer used.
 * @param value A value of type 'string'.
 * @return A null-terminated string whose logical value is equal to that of the
 * provided value.
 */
extern char *lime_string_to_null_terminated(LimeValue value);

/**
 * Checks if the specified library is currently loaded.
 * @param path The file path to the library.
 * @return Whether or not the specified library is currently loaded.
 */
extern bool lime_library_is_loaded(char *path);

/**
 * Computes the hash of the provided value.
 * @param value The value to hash.
 * @return The hash of the provided value.
 */
extern u64 lime_value_hash(LimeValue value);

/**
 * Tests whether the two provided values are equal.
 * @param a The first value.
 * @param b The second value.
 * @return Whether or not the two values are equal.
 */
extern bool lime_value_equals(LimeValue a, LimeValue b);

/**
 * Computes the actual byte size of the provided value.
 * This takes any variable sized members into account and is therefore at 
 * least 'sizeof(struct lime_value)'.
 * @param value The value whose byte size should be computed.
 * @return The hash of the value. 
 */
extern u64 lime_value_byte_size(LimeValue value);

/**
 * Prints the string representation of the provided value into the specified
 * stream.
 * @param stream The output stream to use.
 * @param value The value to print.
 */
extern void lime_value_dump(FILE *stream, LimeValue value);

/**
 * Computes a string representation of the provided value by dynamically 
 * allocating memory. The resulting memory address and size is stored inside 
 * the provided arguments. The returned memory has to be freed by the user if
 * it is no longer needed.
 * @param value The value whose string representation should be computed.
 * @param buffer A reference to a memory location where the resulting pointer
 * to the memory will be stored.
 * @param length A reference to a memory location where the resulting size
 * will be stored.
 */
extern void lime_value_show(LimeValue value, char **buffer, u64 *length);

/** 
 * Returns the length of the provided value. 
 * That is, the type of the value must be either a 'string', 'map', 'list' or
 * 'symbol'.
 * @param value The value whose length should be returned.
 * @return The length of the value.
 */
extern u64 lime_value_length(LimeValue value);

/**
 * Returns a string representation of the value's type.
 * @param value The value whose type's string representation should be returned.
 * @return The string representation of the value's type.
 */
extern char *lime_value_type(LimeValue value);

/**
 * Returns a string representation of the provided type.
 * @param type The type whose string representation should be returned.
 * @return The string representation of the type.
 */
extern char *lime_type_name(LimeValueType type);

/**
 * Creates a new string exception on basis of the message and its format data.
 * @param stack The current stack of the environment.
 * @param message The message which may contain format specifiers.
 * @param ... The variable number of format data.
 * @return Either the string exception or any other exception which arose while
 * creating it (e.g. in case of a heap overflow).
 */
extern LimeValue lime_exception(LimeStack stack, char *message, ...);

/**
 * The constructor for symbol values. 
 * @param stack The current stack of the environment.
 * @param bytes The byte data of this symbol.
 * @param length The length of the byte data.
 * @return Either an exception (e.g. in case of a heap overflow) or the symbol.
 */
extern LimeResult lime_symbol(LimeStack stack, u8 *bytes, u64 length);

/**
 * The constructor for string values. 
 * @param stack The current stack of the environment.
 * @param bytes The byte data of this string.
 * @param length The length of the byte data.
 * @return Either an exception (e.g. in case of a heap overflow) or the string.
 */
extern LimeResult lime_string(LimeStack stack, u8 *bytes, u64 length);

/**
 * The constructor for native function values. 
 * @param stack The current stack of the environment.
 * @param library The shared library where the function pointer originates from, which
 * may be 'NULL' if this function pointer does not originate from a shared library.
 * @param function The function pointer. 
 * @return Either an exception (e.g. in case of a heap overflow) or the function value.
 */
extern LimeResult lime_function(LimeStack stack, LimeValue library, LimeFunction function);

/**
 * The constructor for list values. 
 * @param stack The current stack of the environment.
 * @param head The head of the list.
 * @param tail The tail of the list, which may be 'NULL' in case of an empty list.
 * @return Either an exception (e.g. in case of a heap overflow) or the list value.
 */
extern LimeResult lime_list(LimeStack stack, LimeValue head, LimeValue tail);

/**
 * The constructor for map values. 
 * @param stack The current stack of the environment.
 * @param capacity The number of buckets this map should have.
 * @return Either an exception (e.g. in case of a heap overflow) or the map value.
 */
extern LimeResult lime_map(LimeStack stack, u64 capacity);

/**
 * The constructor for number values. 
 * @param stack The current stack of the environment.
 * @param value The IEEE 754 encoded value with double precision.
 * @return Either an exception (e.g. in case of a heap overflow) or the number value.
 */
extern LimeResult lime_number(LimeStack stack, double value);

/**
 * The constructor for library values. 
 * @param stack The current stack of the environment.
 * @param path The path to the shared library.
 * @return Either an exception (e.g. in case of a heap overflow) or the library value.
 */
extern LimeResult lime_library(LimeStack stack, char *path);

/**
 * The constructor for boolean values. 
 * @param stack The current stack of the environment.
 * @param value The boolean value. 
 * @return Either an exception (e.g. in case of a heap overflow) or the boolean value.
 */
extern LimeResult lime_boolean(LimeStack stack, bool value);

#endif
