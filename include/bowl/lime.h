#ifndef BOWL_H
#define BOWL_H

#include "common.h"

/**
 * An enumeration of all value types that exist in bowl.
 */
typedef enum {
    /** Indicates a value of type 'symbol'. */
    BowlSymbolValue    = 0,
    /** Indicates a value of type 'list'. */
    BowlListValue      = 1,
    /** Indicates a value of type 'function'. */
    BowlNativeValue    = 2,
    /** Indicates a value of type 'map'. */
    BowlMapValue       = 3,
    /** Indicates a value of type 'boolean'. */
    BowlBooleanValue   = 4,
    /** Indicates a value of type 'number'. */
    BowlNumberValue    = 5,
    /** Indicates a value of type 'string'. */
    BowlStringValue    = 6,
    /** Indicates a value of type 'library'. */
    BowlLibraryValue   = 7,
    /** Indicates a value of type 'vector'. */
    BowlVectorValue    = 8,
    /** Indicates a value of type 'exception'. */
    BowlExceptionValue = 9
} BowlValueType;

/**  
 * The type for all bowl values.
 * 
 * Since bowl values reside in the heap, this type is a pointer to the actual 
 * data structure.
 */
typedef struct bowl_value *BowlValue;

/**
 * The type of a single stack frame of bowl.
 * 
 * A stack frame is used to store temporary values and global registers such as 
 * the callstack, datastack and the dictionary. 
 * 
 * Moreover, a stack frame always holds a reference to its predecessor, thus 
 * leading to a chain of stack frames that represent the actual stack. The top-
 * most (or rather the initial) stack frame may be 'NULL'. If this reference 
 * is not correctly set up (e.g. setting it to 'NULL' although there is another
 * stack frame already in use), the garbage collector may decide to collect 
 * objects which would be alive otherwise.
 * 
 * The three registers may be used for arbitrary purposes as temporary variables. 
 * Any value that resides in one of these registers is managed by the garbage 
 * collector. Therefore, it is best to initialize them to 'NULL' when creating
 * new stack frames.
 * 
 * The references of the dictionary, the callstack and the datastack are managed
 * by the garbage collector as well. In general, they are references to registers
 * that live in the enclosing scope. That is, changing them may cause different
 * behavior of future instructions in the same scope. This is most likely to be 
 * desired when working with the datastack (e.g. retrieving arguments from the 
 * datastack and pushing result values onto it).
 */
typedef struct bowl_stack_frame BowlStackFrame;

/**
 * The actual data structure of a single stack frame.
 * @see BowlStackFrame
 */
struct bowl_stack_frame {
    /**
     * A pointer to the previous stack frame or 'NULL' if there is no previous
     * stack frame.
     */
    BowlStackFrame *previous;
    /**
     * A set of general purpose registers which are managed by the garbage
     * collector.
     */
    BowlValue registers[3];
    /** The dictionary of the current scope. */
    BowlValue *dictionary;
    /** The callstack of the current scope. */
    BowlValue *callstack;
    /** The datastack of the current scope. */
    BowlValue *datastack;
};

/**
 * An alias for a pointer to a single stack frame. 
 * 
 * Since stack frames hold a reference to their previous stack frame, a pointer
 * to a stack frame basically forms a single linked list and thus can be seen 
 * as a stack.
 * @see BowlStackFrame
 */
typedef BowlStackFrame *BowlStack;

/**
 * A convenient macro which handles the stack allocation of new stack frames. 
 * @param stack A pointer to the previous stack frame or 'NULL' if this is the 
 * first stack frame.
 * @param a The initial value of the first register or 'NULL' otherwise.
 * @param b The initial value of the second register or 'NULL' otherwise.
 * @param c The initial value of the third register or 'NULL' otherwise.
 * @return A designated initializer for the type 'BowlStackFrame'.
 */
#define BOWL_ALLOCATE_STACK_FRAME(stack, a, b, c) {\
    .previous = (stack),\
    .registers = { (a), (b), (c) },\
    .dictionary = (stack)->dictionary,\
    .callstack = (stack)->callstack,\
    .datastack = (stack)->datastack\
}

/** 
 * A macro for empty stack frames. 
 * @param stack A pointer to the previous stack frame or 'NULL' if there is none.
 * @return A designated initializer for the type 'BowlStackFrame'.
 */
#define BOWL_EMPTY_STACK_FRAME(stack) {\
    .previous = (stack),\
    .registers = { NULL, NULL, NULL },\
    .dictionary = NULL,\
    .callstack = NULL,\
    .datastack = NULL\
}

/**
 * The interface of a native function for bowl.
 * 
 * A native function always takes the current stack as argument and returns 
 * an exception or 'NULL' if there was no exception. 
 * 
 * Arguments and result values of bowl itself must be passed by setting the
 * datastack accordingly.
 */
typedef BowlValue (*BowlFunction)(BowlStack stack);

/**
 * The type of a native library handle. 
 * 
 * On unsupported platforms this type defaults to the generic 'void*' type.
 */
#if defined(OS_UNIX)
    typedef void *BowlLibraryHandle;
#elif defined(OS_WINDOWS)
    typedef HINSTANCE BowlLibraryHandle;
#else
    typedef void *BowlLibraryHandle;
#endif

/**
 * The actual data structure of a bowl value.
 * @see BowlValue
 */
struct bowl_value {
    /** 
     * The type of this value.
     * @see BowlValueType
     */
    BowlValueType type;
    /** 
     * The real location of this value.
     * 
     * This field is used by the garbage collector to mark the new location 
     * of a value after it has been relocated by it.
     */
    BowlValue location;
    /** 
     * The hash of this value.
     * 
     * A value of '0' indicates that the hash of this value is not yet 
     * computed. 
     */
    u64 hash;

    union {
        /**
         * The data which is related to values of type 'symbol'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'symbol'.
         */
        struct {
            /** The length of this symbol. */
            u64 length;
            /** 
             * The bytes of this symbol. 
             * 
             * This array contains exactly 'length' bytes and is allocated along
             * with the instance of this value.
             */
            u8  bytes[];
        } symbol;

        /**
         * The data which is related to values of type 'number'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'number'.
         */
        struct {
            /** The IEEE 754 encoded value of this value with double precision. */
            double value;
        } number;

        /**
         * The data which is related to values of type 'boolean'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'boolean'.
         */
        struct {
            /** This value's boolean value. */
            bool value;
        } boolean;

        /**
         * The data which is related to values of type 'string'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'string'.
         */
        struct {
            /** The length of this string. */
            u64 length;
            /** 
             * The bytes of this string. 
             * 
             * This array contains exactly 'length' bytes and is allocated along
             * with the instance of this value.
             */
            u8  bytes[];
        } string;

        /**
         * The data which is related to values of type 'list'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'list'.
         */
        struct {
            /** The length of this list. */
            u64 length;
            /** The head of this list. */
            BowlValue head;
            /** 
             * The tail of this list.
             * 
             * This field may contain 'NULL' in case of the empty list, which
             * is always represented as the 'NULL' pointer.
             */
            BowlValue tail;
        } list;

        /**
         * The data which is related to values of type 'map'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'map'.
         */
        struct {
            /** The number of elements that this map contains. */
            u64 length;
            /** The number of buckets that this map contains. */
            u64 capacity;
            /**
             * The buckets of this map.
             * 
             * A bucket is represented as a list of where all odd elements (those
             * with an even index) correspond to keys and all even elements (those
             * with an odd index) correspond to values.
             */
            BowlValue buckets[];
        } map;

        /**
         * The data which is related to values of type 'function'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'function'.
         */
        struct {
            /** The library value which contains this function or 'NULL' if there is none */
            BowlValue library;
            /** The function pointer to the native function. */
            BowlFunction function;
        } function;

        /**
         * The data which is related to values of type 'library'.
         * 
         * Only access this member if the type of this value is equal to
         * type 'library'.
         */
        struct {
            /** The handle of the dynamic library. */
            BowlLibraryHandle handle;
            /** The length of this library's name. */
            u64 length;
            /** 
             * The bytes of this library's name. 
             * 
             * This array contains exactly 'length' bytes and is allocated along
             * with the instance of this value.
             */
            u8  bytes[];
        } library;

        struct {
            /** The length of this vector. */
            u64 length;
            /** This vector's array of elements. */
            BowlValue elements[];
        } vector;

        struct {
            /** The exception which originally caused this one or 'NULL'. */
            BowlValue cause;
            /** The message of this exception. */
            BowlValue message;
        } exception;
    };
};

#endif
