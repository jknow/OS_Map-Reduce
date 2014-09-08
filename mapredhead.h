/* 
* File:   mapredhead.h
* Author: joeko
*
* Created on March 22, 2014, 8:50 PM
*/

#ifndef MAPREDHEAD_H
#define	MAPREDHEAD_H

#include "uthash.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

typedef struct keyValueStruct{
	char* word;
	int value;
	UT_hash_handle hh;
} keyv;

struct topasstothread
{
	FILE* file2threads;
	int rednum_t;
	keyv* to_red_ptr_tracker;
};
typedef struct topasstothread toThread;

int thread_spinner(int mapnum, const char* infile, const char* outfile);

void *redintsort_t(void* pargs);

void *redwordcount_t(void* pargs);

void *mapintsort_t(void* pargs);

void *mapwordcount_t(void* pargs);

int mod_sort(keyv *a, keyv *b);

/*
-----------------START OF SORTED-LIST HEADER-------------------
*/



int compareStrings(void *p1, void *p2);

int compareInts(void *p1, void *p2);

/*
* Sorted list type.  You need to fill in the type as part of your implementation.
*/
typedef int (*CompareFuncT)(void *, void *);

/*
* Joe's Notes: The SortedList structure is a node in a linked list. It stores a void pointer (something),
*  a function pointer (comp (short for compare), as defined in the above typedef),
*  a pointer to the next node in the list (next), and an int to keep track of where the iterator is so
*  the node is not deleted if it is being pointed at by the iterator (ptrDelete).
*/
struct SortedList
{
	int ptrDelete;
	void *something;
		int value;
	CompareFuncT comp;
	struct SortedList *next;
};
typedef struct SortedList* SortedListPtr;


/*
* Iterator type for user to "walk" through the list item by item, from
* beginning to end.  You need to fill in the type as part of your implementation.
*/

/*
* Joe's Notes: The iterator is a simple structure consisting of two SortedList pointers:
* one for the head, if there is need to return to the top of the linked list; and one for
* the node that the iterator is currently on.
*/
struct SortedListIterator
{
	struct SortedList *curr;
	struct SortedList *head;
};
typedef struct SortedListIterator* SortedListIteratorPtr;


/*
* When your sorted list is used to store objects of some type, since the
* type is opaque to you, you will need a comparator function to order
* the objects in your sorted list.
*
* You can expect a comparator function to return -1 if the 1st object is
* smaller, 0 if the two objects are equal, and 1 if the 2nd object is
* smaller.
*
* Note that you are not expected to implement any comparator functions.
* You will be given a comparator function when a new sorted list is
* created.
*/



/*
* SLCreate creates a new, empty sorted list.  The caller must provide
* a comparator function that can be used to order objects that will be
* kept in the list.
* 
* If the function succeeds, it returns a (non-NULL) SortedListT object.
* Else, it returns NULL.
*
* You need to fill in this function as part of your implementation.
*/

SortedListPtr SLCreate(CompareFuncT cf);

/*
* SLDestroy destroys a list, freeing all dynamically allocated memory.
*
* You need to fill in this function as part of your implementation.
*/
void SLDestroy(SortedListPtr list);


/*
* SLInsert inserts a given object into a sorted list, maintaining sorted
* order of all objects in the list.  If the new object is equal to a subset
* of existing objects in the list, then the subset can be kept in any
* order.
*
* If the function succeeds, it returns 1.  Else, it returns 0.
*
* You need to fill in this function as part of your implementation.
*/

int SLInsert(SortedListPtr list, char *newObj, int value);

int SLInsertNoDup(SortedListPtr list, char *newObj, int value);

/*
* SLRemove removes a given object from a sorted list.  Sorted ordering
* should be maintained.
*
* If the function succeeds, it returns 1.  Else, it returns 0.
*
* You need to fill in this function as part of your implementation.
*/

int SLRemove(SortedListPtr list, void *newObj);


/*
* SLCreateIterator creates an iterator object that will allow the caller
* to "walk" through the list from beginning to the end using SLNextItem.
*
* If the function succeeds, it returns a non-NULL SortedListIterT object.
* Else, it returns NULL.
*
* You need to fill in this function as part of your implementation.
*/

SortedListIteratorPtr SLCreateIterator(SortedListPtr list);


/*
* SLDestroyIterator destroys an iterator object that was created using
* SLCreateIterator().  Note that this function should destroy the
* iterator but should NOT affectt the original list used to create
* the iterator in any way.
*
* You need to fill in this function as part of your implementation.
*/

void SLDestroyIterator(SortedListIteratorPtr iter);


/*
* SLNextItem returns the next object in the list encapsulated by the
* given iterator.  It should return a NULL when the end of the list
* has been reached.
*
* One complication you MUST consider/address is what happens if a
* sorted list encapsulated within an iterator is modified while that
* iterator is active.  For example, what if an iterator is "pointing"
* to some object in the list as the next one to be returned but that
* object is removed from the list using SLRemove() before SLNextItem()
* is called.
*
* You need to fill in this function as part of your implementation.
*/

void *SLNextItem(SortedListIteratorPtr iter);

#endif	/* MAPREDHEAD_H */

