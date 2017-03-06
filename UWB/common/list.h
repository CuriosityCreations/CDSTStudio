/**
 * @file list.h
 * @brief header for function on double linked list
 * @author bgi@bespoon.com
 * @date 19/09/2012
 */
/*
 * Copyright (C) Bespoon 2014-2015. No Part of the source code shall be
 * reproduced, transmitted, distributed, used nor modified in any way without
 * the express prior written authorization of Bespoon.
 */

#ifndef _LISTHEADER_H_
#define _LISTHEADER_H_

#include <osal_type.h>

typedef struct list
{
	 struct list*  prev;/**< a pointer to the previous list_t */
	 struct list*  next;/**< a pointer to  the next list_t*/
	
}list_t;


OSAL_void INITLISTHEAD( list_t *liste);

/**
* @brief  cast a member of a structure out to the containing structure
* @param ptr:        the pointer to the member.
* @param type:       the type of the container struct this is embedded in.
* @param member:     the name of the member within the struct.
*
*/
#define ContainerOf(ptr, type, member) ((type *)( ((char *)(ptr)) - offsetof(type,member) ))

/**
 * @brief add the entry at the head of the list
 * @param new:	the entry to add
 * @param next:	the next etry in the list
 */
OSAL_void ListAdd( list_t *anew, list_t *head);

/**
 * @brief delete the entry which is the tail of the list
 * @param head:	the head of the list
 */
OSAL_void ListDelTail( list_t * head);

/**
 * @brief delete the entry between prev and next
 * @param prev:	the previous entry in the list
 * @param next:	the next etry in the list
 */
OSAL_void ListDel( list_t * prev,  list_t * next);

/**
 * @brief move the tail of the list head_src to the top of the list head_dest
 * @param head_src:	the source list
 * @param head_dest: the destination list
 */
OSAL_void ListMove( list_t * head_src, list_t * head_dest );

/**
 * @brief return the size of the list 
 * @param head:	the head of the list
 */
OSAL_u32 ListSize(list_t * head);

/**
 * @brief Tell wether or not a list contain some element (other than the head) 
 * @return true if the list contains only the head, false if the list has at least one element which is not the head.
 */
OSAL_bool_t ListIsEmpty(list_t * head);

/**
 * @brief get the struct for this entry
 * @param ptr:	the &struct list_head pointer
 * @param type:	the type of the struct this is embedded in
 * @param member:	the name of the list_struct within the struct
 */
#define ListEntry(ptr, type, member) \
	ContainerOf(ptr, type, member)

/**
 * @brief get the first entry of a list
 * @param head: the head of the list
 * @param type: the type of the struct the first element is embedded in
 * @param member: the name of the list_struct within the struct
 */
#define ListFirstEntry(head, type, member) \
	(ListIsEmpty(head) ? NULL : ListEntry((head)->next, type, member))

/**
 * @brief get the last entry of a list
 * @param head: the head of the list
 * @param type: the type of the struct the first element is embedded in
 * @param member: the name of the list_struct within the struct
 */
#define ListLastEntry(head, type, member) \
	(ListIsEmpty(head) ? NULL : ListEntry((head)->prev, type, member))


/**
 * @brief get the n-th entry of the list
 * @param head: the head of the list
 * @param pos: the index of the entry in the list, may be negative to count backward, 0 is the head, 1, the first entry, 2 the second, ..., -1 the last entry
 */
list_t* ListGetAt(list_t* head, OSAL_s32 pos);


// Helper for ListGetEntryAt()
OSAL_void* _ListGetEntryAt(list_t* head, OSAL_s32 pos, OSAL_u32 offset);

/**
 * @brief get the struct for the n-th entry of the list
 * @param head: the head of the list
 * @param pos: the index of the entry in the list, may be negative to count backward, 0 is the head, 1, the first entry, 2 the second, ..., -1 the last entry
 * @param type: the type of the struct the first element is embedded in
 * @param member: the name of the list_struct within the struct
 */

#define ListEntryAt(head, pos, type, member) \
	((type*)_ListGetEntryAt(head, pos, offsetof(type,member)))

/**
 * @brief	iterate over list of given type
 * @param pos:	the type * to use as a loop cursor
 * @param head:	the head for your list
 * @param member:	the name of the list_struct within the struct
 */
#define ListForEachEntry(pos, head, member)				\
	for (pos = ListEntry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = ListEntry(pos->member.next, typeof(*pos), member))

/**
 * @brief iterate backwards over list of given type
 * @param pos:	the type * to use as a loop cursor
 * @param head:	the head for your list
 * @param member:	the name of the list_struct within the struct.
 */
#define ListForEachEntryReverse(pos, head, member)			\
	for (pos = ListEntry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = ListEntry(pos->member.prev, typeof(*pos), member))
 
#endif
