/*
* Joe Komosinski
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mapredhead.h"
#include "uthash.h"

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s2, s1);
}

int compareInts(void *p1, void *p2)
{
	int i1 = atoi((char*)p1);
	int i2 = atoi((char*)p2);

	return i2 - i1;
}

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

/*
* Joe's Notes: The only thing that needs memory allocation is the node itself.
* Besides that, something and next are set to NULL, the compare pointer is set
* to the function pointed to by the parameter, and the ptrDelete is set to 0
* because the iterator is not pointing to it.
*/

SortedListPtr SLCreate(CompareFuncT cf)
{
	struct SortedList *stor = malloc(sizeof(struct SortedList));
	stor->something = NULL;
	stor->value = 0;
	stor->comp = cf;
	stor->next = NULL;
	stor->ptrDelete = 0;
	return stor;
}

/*
* SLDestroy destroys a list, freeing all dynamically allocated memory.
*
* You need to fill in this function as part of your implementation.
*/

/*
* Joe's Notes: SLDestroy is simple. If the list does not exist yet, the function simply returns.
* If the head is the only node in the list, it simply frees that. If the linked list is populated,
* the function frees the memory malloc-ed for each node one by one.
*
* There is no checking for where the iterator points because when the whole list is freed, the
* iterator has nowhere to point anyway.
*/
void SLDestroy(SortedListPtr list)
{
	if(list == NULL)
	{
		return;
	}

	if(list->next == NULL)
	{
		free(list->something);
		free(list);
		return;
	}

	struct SortedList *freed = list;
	struct SortedList *temp = list;

	while(temp->next != NULL)
	{
		freed = temp;
		temp = temp->next;
		free(freed->something);
		free(freed);
	}

	free(temp->something);
	free(temp);
}


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

/*
* Joe's Notes: SLInsert returns if there is no head to the linked list. If there is a linked list,
* a loop runs through the linked list until it finds the proper place for the newObj parameter or
* the end of the linked list is reached. Then, a newNode is created and it is inserted wherever the
* loop ended. If it is inserted at the end of the linked list, the previous last node simply points
* to it. Otherwise, the newNode points to the previous pointer's next node and the previous pointer
* points to the newNode.
*/

int SLInsert(SortedListPtr list, char *newObj, int value)
{
	if(list == NULL)
	{
		return 0;
	}

	struct SortedList *temp = list;

	while (temp->next != NULL)
	{
		if (temp->comp(temp->next->something, newObj) < 0)
		{
			break;
		}
		temp = temp->next;
	}

	struct SortedList *newNode = SLCreate(temp->comp);

	newNode->something = calloc(strlen(newObj), sizeof(char));
	strcpy(newNode->something, newObj);
	newNode->value = value;

	if(temp->next == NULL)
	{
		temp->next = newNode;
		return 1;
	}
	else
	{
		newNode->next = temp->next;
		temp->next = newNode;
		return 1;
	}

	return 0;
}

/*
* SLInsertNoDup means SL insert no duplicates. This function was added for the mapred project to increment the count instead of inserting duplicates.
*/ 

int SLInsertNoDup(SortedListPtr list, char *newObj, int value)
{
	if(list == NULL)
	{
		return 0;
	}

	struct SortedList *temp = list;

	while (temp->next != NULL)
	{
		if (temp->comp(temp->next->something, newObj) == 0)
		{
			temp->next->value++;
			return 1;
		}
		else if (temp->comp(temp->next->something, newObj) < 0)
		{
			break;
		}
		temp = temp->next;
	}

	struct SortedList *newNode = SLCreate(temp->comp);

	newNode->something = calloc(strlen(newObj), sizeof(char));
	strcpy(newNode->something, newObj);
	newNode->value = value;

	if(temp->next == NULL)
	{
		temp->next = newNode;
		return 1;
	}
	else
	{
		newNode->next = temp->next;
		temp->next = newNode;
		return 1;
	}

	return 0;
}


/*
* SLRemove removes a given object from a sorted list.  Sorted ordering
* should be maintained.
*
* If the function succeeds, it returns 1.  Else, it returns 0.
*
* You need to fill in this function as part of your implementation.
*/

/*
* Joe's Notes: If SLRemove is called to an empty linked list, the function simply returns.
* If there is a list, there is a loop that moves through the list until it finds what the caller
* wants to delete or it reaches the end of the loop. An if statement then checks if the temp pointer
* is at the end of the list. If it is, then the function returns with a zero, signifying an error.
* Otherwise, a nested if statement checks if there is an iterator pointing to the node that is
* about to be deleted. If there is no iterator, the pointers are changed to skip the current
* node and then the current node is freed. If there is an iterator, the ptrDelete is increased to 2
* to signify that the node is reserved for deletion when the iterator moves on.
*/
int SLRemove(SortedListPtr list, void *newObj)
{
	if(list == NULL)
	{
		return 0;
	}

	struct SortedList *temp = list;

	while (temp->next != NULL)
	{
		if(temp->comp(temp->next->something, newObj) == 0)
		{
			break;
		}
		temp = temp->next;
	}

	if(temp->next == NULL)
	{
		return 0;
	}
	else
	{
		if (temp->next->ptrDelete == 0)
		{
			struct SortedList *toDelete = temp->next;
			temp->next = toDelete->next;
						free(toDelete->something);
			free(toDelete);
			return 1;
		}
		else
		{
			struct SortedList *toDelete = temp->next;
			temp->next = toDelete->next;
			toDelete->ptrDelete++;
			return 1;
		}
	}

	return 0;
}


/*
* SLCreateIterator creates an iterator object that will allow the caller
* to "walk" through the list from beginning to the end using SLNextItem.
*
* If the function succeeds, it returns a non-NULL SortedListIterT object.
* Else, it returns NULL.
*
* You need to fill in this function as part of your implementation.
*/

/*
* Joe's Notes: The iterator is created by allocating memory to it then making the current pointer
* point to the first node that has data stored in it. After that, the iterator stores the head of
* the linked list and finally the current pointer's ptrDelete is incremented to signify that the
* node has a pointer pointing to it.
*/
SortedListIteratorPtr SLCreateIterator(SortedListPtr list)
{
	struct SortedListIterator *iter = malloc(sizeof(struct SortedListIterator));
	iter->curr = list->next;
	iter->head = list;
	iter->curr->ptrDelete++;
	return iter;
}


/*
* SLDestroyIterator destroys an iterator object that was created using
* SLCreateIterator().  Note that this function should destroy the
* iterator but should NOT affectt the original list used to create
* the iterator in any way.
*
* You need to fill in this function as part of your implementation.
*/

/*
* Joe's Notes: The iterator is simply freed because nothing inside the iterator has malloc-ed
* memory.
*/

void SLDestroyIterator(SortedListIteratorPtr iter)
{
	free(iter);
}


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

/*
* Joe's Notes: The function first checks to see if the current node the iterator is pointed at
* is set for deletion. If it is, the iterator is set to the first node that has data stored in it
* and a loop iterates through the linked list until the comparator function stored in the head determines
* which node is suppose to be next on the linked list and the iterator is set to that node. The previous
* node is then freed.
*
* If the current node is not set for deletion, there is a nested if statement that will determine
* if the current node is at the end of the linked list or not. If the current node is at the end
* of the list, the node is set to the first node with data in it. Otherwise, it is just changed to
* the next node. In both previous options, the deletion pointer is decremented in the original pointer
* and incremented in the new pointer.
*/

void *SLNextItem(SortedListIteratorPtr iter)
{
	if(iter->curr->ptrDelete == 2)
	{
		struct SortedList *toDelete = iter->curr;

		if (iter->head->next != NULL)
		{
			iter->curr = iter->head->next;
			while (iter->curr->next != NULL)
			{
				if(iter->head->comp(iter->curr->next->something, toDelete->something) < 0)
				{
					iter->curr = iter->curr->next;
					break;
				}
				iter->curr = iter->curr->next;
			}
		}
		else
		{
			iter->curr = iter->head;
		}
		free(toDelete);
	}
	else
	{
		if(iter->curr->next == NULL)
		{
			iter->curr->ptrDelete--;
			if (iter->head->next != NULL)
			{
				iter->curr = iter->head->next;
			}
			else
			{
				iter->curr = iter->head;
			}
			iter->curr->ptrDelete++;
		}
		else
		{
			iter->curr->ptrDelete--;
			iter->curr = iter->curr->next;
			iter->curr->ptrDelete++;
		}
	}

	return iter;
}
