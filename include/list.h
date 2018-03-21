/**
 * @defgroup list List Module
 * @ingroup data_strucutre
 * 
 * @todo fill in the main comment block
 * @{
 */

#ifndef _LIST_H_
#define _LIST_H_

#include <stdint.h>
#include "project_settings.h"

#include "library.h"

#ifdef USE_MODULE_SUBSYS
#include "subsys.h"
#endif

/**
 * defines a function pointer called sort_fn_t with two inputs and one output uint8_t, 
 * which is used to determine whether or not the new item being added into the linked list should go before or after the current item in the list	
 *
 * The function should return 1 when the first item should go before the second item (i.e. sort(first_item, second_item)) 
 *
 */
typedef uint8_t(*list_sort_fn_t)(void*, void*);

typedef uint8_t(*list_identify_fn_t)(void*, void*);

typedef struct list_link_t {
    void* item_ptr; ///< item_ptr
    struct list_link_t* next; ///< next
    struct list_link_t* previous; ///< previous
} list_link_t;

/**
 * The struct list_t is for a doubly-linked list of generic items.
 * Elements of type list_link_t are used, each containing an item pointer, and pointers to next and previous links.
 * first and last list_link_t elements are used to keep track of the ends of the list. length and array_length 
 * are used for the current length and maximum declared length of the list, equal to the maximum number of elements.
 * The links list_link_t pointer points to an array of link elements. item_array points to an item array of size item_size.
 * sort_fn and identify_fn function pointers are included, so the user can specify a sorting function and identification 
 * function, so that sorting can be performed. working[2], unlinked[2], and unused[2] arrays of link pointers are not
 * strictly necessary, but provide faster operation for interrupt optimization. Working refers to the last elements modified
 * while unlinked can refer to new elements not yet inserted into the list, and unused contains pointers to elements which
 * can be overwritten immediately.
 *
 */
typedef struct list_t {
    list_link_t* first; ///< first
    list_link_t* last; ///< last
    uint16_t length; ///< length
    uint16_t array_length; ///< array_length
    list_link_t* links; ///< links
    uint16_t* item_array; ///< item_array
    uint16_t item_size; ///< item_size
    list_sort_fn_t sort_fn; ///< sort_fn
    list_identify_fn_t identify_fn; ///< identify_fn
    // the following overhead members are only needed
    // to make the list module faster

    list_link_t * working[2]; ///< working
    list_link_t * unlinked[2]; ///< unlinked
    uint16_t unlinked_count; ///< count of unlinked list items
    //list_link_t * unused[2]; ///< unused Commented because feature not fully implemented
} list_t;

#define LIST_ALLOCATE(t, list_name,  max_elements)\
    t CAT2(list_name, _array)[max_elements];\
    list_link_t CAT2(list_name, _links)[max_elements];\
    list_t list_name

#define LIST_INIT(list_name, max_elements, item_size)\
    List_Init(&list_name, item_size, &CAT2(list_name, _array)[0], max_elements, &CAT2(list_name, _links)[0])


void List_Init(list_t* list, uint16_t item_size, void* item_array, uint16_t item_array_length, list_link_t* link_array);

/** 
 * List_SetSortFunction will set the sorting function to the given list
 *
 * Given a pointer to a list and a function pointer that returns a uint8_t with
 * two void pointer inputs, this function will set the sort_fn parameter of the
 * list to the sort_fun function pointer input
 *
 * A valid list with a List_Init() function called on it to initialize it
 * must be passed in to this function.
 *
 * @author Michael Muhlbaier
 *
 * @param list list_t pointer that has been initialized upon which the sorting
 * function will be set on
 *
 * @param sort_fun function pointer (with return type uint8_t and two input
 * parameters a and b (void pointers) ) that will point to the sorting function
 * for the given list input. The sorting function should return 1 when a comes 
 * before b and 0 otherwise.
 *
 */
void List_SetSortFunction(list_t* list, uint8_t(*sort_fn)(void* a, void* b));

/** List_SetIndentifyFunction sets a new identify function in the list. It does this by setting the identify_fn
 * member that list points to and changes it to the identify_fn from the function pointer in the second parameter.
 *
 * @param list is a linked list variable
 * @param identify_fn is a function pointer for identifying a function. Return 1 if match found, 0 otherwise
 *
 */
void List_SetIdentifyFunction(list_t* list, uint8_t(*identify_fn)(void* identifier, void* item));

/**
 * @warning Not interrupt safe, do not call from an interrupt.
 *
 * @param list - Pointer to a list that an item will be added to
 * @param item - Pointer to an item that will be added to the list
 * @return Returns pointer to item added in the list. If no room is available in the list, returns null pointer.
 */
void* List_AddAndLink(list_t* list, void* item);

/**
 * @brief List_Add is used to add an item to a list. Uses List_AddIndirect to find where the item should go in the list.
 *
 * @param list - Pointer to a list that an item will be added to
 * @param item - Pointer to an item that will be added to the list
 * @return Returns pointer to item added in the list. If no room is available in the list, returns null pointer.
 *
 */
void* List_Add(list_t* list, void* item);

/** @brief List_AddIndirect is used to obtain a valid position for which an item can be saved to.
 *
 * List_AddIndirect gets an unused link using GetUnusedLink. If unused link is available, function will update pointers
 * to unlinked items and update working links. If no unused links are available, return a null pointer
 *
 * @param list - Pointer to list that an item needs to be added to
 * @return Returns a null pointer if there is no room in list.
 * Otherwise returns item_ptr in the first unused link
 *
 */
void* List_AddIndirect(list_t* list);

/**
 * This function takes in a pointer to the list_t struct and checks to
 * see if the 2nd element in the unlinked array stored in the list_t
 * struct is not equal to the PENDING_LINK macro. If it isn't, the
 * whole list doesn't need to be linked, and just checks to see if
 * the first two elements in the unlinked array are non-zero. If they
 * are, they are ran through the Link function to link the items. If
 * the 2nd element in the unlinked array is equal to PENDING_LINK,
 * then the whole list of items is searched and they are linked
 * together if they aren't already. NOTE: This function is not
 * interrupt safe.
 *
 * @warning Not interrupt safe, do not call from an interrupt.
 *
 * @param list The pointer to the list_t struct
 *
 */
void List_Link(list_t* list);

/**
 *
 * @warning Not interrupt safe, do not call from an interrupt.
 *
 * @param list The pointer to the list_t struct
 *
 */
void List_ResortFirst(list_t* list);

/**
 * @warning Not interrupt safe, do not call from an interrupt.
 *
 * @param list The pointer to the list_t struct
 *
 */
void List_ResortLast(list_t* list);

/**
 *
 * @warning Not interrupt safe, do not call from an interrupt.
 *
 * @param list The pointer to the list_t struct
 * @param item
 *
 */
void List_LinkItem(list_t* list, void* item);

/** List_GetFirst returns a pointer to the first item in the list.
 *
 * List_GetFirst returns a pointer to the first item in the list if
 * there is an item in the list. Otherwise, nothing
 *
 * @param list List of pointers to items (list_t struct)
 * @return Pointer to first item in list or 0 if empty
 *
 */
void* List_GetFirst(list_t* list);

/**
 * @brief Get the Last Item in the List
 *
 * List_GetLast() returns the last item from the list provided when called
 *
 * @param list is a pointer to the list the user wants the last item for
 * @return is a pointer to the last item in the list provided when the function is called
 *
 */
void* List_GetLast(list_t* list);

/**
 * @brief Get the Next Item in the List
 * 
 * List_GetNext() returns the next item in the list provided. 
 *
 * @param list is a pointer to the list trying to be accessed
 * @param item is a pointer to the item within the list 
 * @return is a pointer to the item after the input item
 *
 */
void* List_GetNext(list_t* list, void* item);

/**
 * Searches a list for the provided identifier, returning a void* to the item if found or 0 if not found.
 * @param list The list to search through.
 * @param identifier The item to check for in the list.
 * @return a void pointer to the item found using the identifier. Returns 0 if not found.
 *
 * @warning The idenfity_func must be set before calling this method. Identify_func performs the comparison 
 * of each element to the passed in identifier.
 *
 */
void* List_GetItem(list_t* list, void* identifier);

/**
 * Removes the first element of list by internally unlinking the element.
 * @param list a pointer to a list_t
 *
 * @warning This method does not return the item that was removed. Use List_UnlinkFirst() if the item is to be used.
 *
 */
void List_RemoveFirst(list_t* list);

/**
 *
 * @param list
 *
 */
void List_RemoveLast(list_t* list);

/**
 *
 * @param list
 * @param item
 *
 */
void List_Remove(list_t* list, void* item);

/**
 *
 * @param list
 *
 */
void List_RemoveAll(list_t* list);

/**
 *
 * @param list
 * @param item
 *
 */
void List_UnlinkItem(list_t* list, void* item);

/**
 *
 * @param list pointer to list structure (object)
 *
 *Function: List_UnlinkFirst(list_t* list)
 *
 *This function unlinks the first item in a linked list. If a valid pointer is not found an error code may be given.
 *
 */
void List_UnlinkFirst(list_t* list);

/**
 *
 * @param list
 *
 *Function: List_UnlinkLast(list_t* list)
 *
 *This function unlinks the last item in a link list. If a valid pointer is not found an error code may be given.
 *
 */
void List_UnlinkLast(list_t* list);

void List_RunFunction(list_t* list, void(*function)(void *) );

/// @}

#endif //_LIST_H_
