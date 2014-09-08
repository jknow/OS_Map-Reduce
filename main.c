/* 
* File:   main.c
* Author: joeko, shinwari
*
* Created on March 22, 2014, 8:40 PM
*/

#include <stdio.h>
#include <stdlib.h>
#include "mapredhead.h"
#include "uthash.h"

/*
* Hash table user guide: http://troydhanson.github.io/uthash/userguide.html
*/

keyv* hash_table; 		/* Head of hash table */
int rednum; 			/* Number of reduce threads */
int wordornum_flag; 	/* Application flag: 0 = wordcount, 1 = sort numbers */
SortedListPtr printer;	/* Head of linked list used to print*/
sem_t hash_sem;			/* Semaphore used to lock hash table for writing*/
sem_t printer_sem;		/* Semaphore used to lock printer for writing*/

/* The main function is mostly used for error checking */
int main(int argc, char** argv) {
	
	if (argc != 11)
	{
		perror("Invalid number of arguments.Must be in form of: \n"
				"mapred -a [wordcount, sort] -i [procs, threads] -m "
				"num_maps -r num_reduces infile outfile\n");
		return -1;
	}
	
	if (strcmp(argv[2], "wordcount") != 0 && strcmp(argv[2], "sort") != 0)
	{
		perror("For -a flag, only wordcount and sort are supported.");
		return -1;
	}
	
	if (strcmp(argv[4], "threads") != 0)
	{
		perror("For -i flag, only threads are supported.");
		return -1;
	}
	
	int mapnum = strtol(argv[6], NULL, 10);
	rednum = strtol(argv[8], NULL, 10);
	
	if (mapnum <= 0 || rednum <= 0)
	{
		perror("Number of mappers and reducers must be integers greater \
				than zero");
		return -1;
	}
	
	if (access( argv[9], F_OK ) == -1)
	{
		perror("Input file does not exist.");
		return -1;
	} else if (access( argv[9], R_OK ) == -1) {
		perror("Input file does not have permission to be read.");
		return -1;
	}
	
	if (access(argv[10], F_OK ) != -1)
	{
		/* Output file already exists exists. Check if can overwrite. */
		if (access( argv[10], W_OK ) == -1) {
			perror("Output file exists, no permission to write.");
			return -1;
		}
	}
	
	hash_table = NULL;
	
	if (strcmp(argv[2], "wordcount") == 0)
	{
		wordornum_flag = 0;
	} else if (strcmp(argv[2], "sort") == 0) {
		wordornum_flag = 1;
	}
	
	if (wordornum_flag == 0)
	{
		printer = SLCreate(compareStrings);
	}
	else
	{
		printer = SLCreate(compareInts);
	}
	
	thread_spinner(mapnum, argv[9], argv[10]);
	
	return 1;
}

/* thread_spinner creates the necessary threads for either word counting or integer sorting. In 
* between the map and reduce functions, the hash table is sorted by hash(key) mod R where R is 
* the number of reduce threads. This makes it easy for the reduce threads to find their portion
* of work in the hash table. */
int thread_spinner(int mapnum, const char* infile, const char* outfile)
{
	FILE* book = fopen(infile, "r");
	
	sem_init(&hash_sem, 0, 1);
	pthread_t map_t[mapnum];
	toThread* map_args = malloc(sizeof(toThread));
	map_args->file2threads = book;
	
	int i = 0, ret = -1;
	if (wordornum_flag == 0)
	{
		for(i = 0; i < mapnum; i++)
		{
			printf("CREATING MAP THREAD %d\n", i+1);
			ret = pthread_create(&map_t[i], NULL, &mapwordcount_t, map_args);
		}
	}
	else
	{
		for(i = 0; i < mapnum; i++)
		{
			printf("CREATING MAP THREAD %d\n", i+1);
			ret = pthread_create(&map_t[i], NULL, &mapintsort_t, map_args);
		}
	}
	
	for(i = 0; i < mapnum; i++)
	{
		pthread_join(map_t[i], NULL);
	}

	fclose(book);
	sem_destroy(&hash_sem);
	free(map_args);
	
	printf("SORTING");
	HASH_SORT(hash_table, mod_sort);

	keyv* ptr_tracker[rednum];
	i = 0;
	ptr_tracker[i] = hash_table;

	keyv *s, *tmp;
	HASH_ITER(hh, hash_table, s, tmp)
	{
		if(((s->hh.hashv) % rednum) != i)
		{
			i++;
			ptr_tracker[i] = s;
			
		}
	}
	
	sem_init(&printer_sem, 0, 1);
	pthread_t red_t[rednum];
	toThread* red_args[rednum];
	
	if (wordornum_flag == 0)
	{
		for(i = 0; i < rednum; i++)
		{
			printf("CREATING REDUCE THREAD %d\n", i+1);
			red_args[i] = malloc(sizeof(toThread));
			red_args[i]->rednum_t = i;
			red_args[i]->to_red_ptr_tracker = ptr_tracker[i];
			ret = pthread_create(&red_t[i], NULL, &redwordcount_t, red_args[i]);
		}
	}
	else
	{
		for(i = 0; i < rednum; i++)
		{
			printf("CREATING REDUCE THREAD %d\n", i+1);
			red_args[i] = malloc(sizeof(toThread));
			red_args[i]->rednum_t = i;
			red_args[i]->to_red_ptr_tracker = ptr_tracker[i];
			ret = pthread_create(&red_t[i], NULL, &redintsort_t, red_args[i]);
		}
	}
	
	for(i = 0; i < rednum; i++)
	{
		free(red_args[i]);
		pthread_join(red_t[i], NULL);
	}
	
	SortedListPtr testtmp = printer->next;
	FILE* output_file;
	
	output_file = fopen(outfile, "w");
	
	printf("PRINTING TO %s\n", outfile);
	if (wordornum_flag == 0)
	{
		while(testtmp != NULL)
		{
			fprintf(output_file, "%s, %d\n", testtmp->something, testtmp->value);
			testtmp = testtmp->next;
		}
	}
	else
	{
		while(testtmp != NULL)
		{
			fprintf(output_file, "%s", testtmp->something);
			testtmp = testtmp->next;
		}
	}
	
	printf("MEMORY CLEAN UP\n");
	fclose(output_file);
	SLDestroy(printer);
	
	keyv *toDelete, *tmpDelete;
	HASH_ITER(hh, hash_table, toDelete, tmpDelete) {
		HASH_DEL(hash_table, toDelete);
		free(toDelete->word);
		free(toDelete);
	}
	return 1;
}

/* redintsort_t (Reduce Integer Sort_Thread) takes the 
* integer out of the hash table and inserts it into 
* the printer linked list*/
void *redintsort_t(void* pargs)
{
	toThread* args = pargs;
	int thread_num = args->rednum_t;
	keyv* work_start = args->to_red_ptr_tracker;
	
	keyv *s, *tmp;
	HASH_ITER(hh, work_start, s, tmp)
	{
		if(((s->hh.hashv) % rednum) != thread_num)
		{
			break;
		}
		printf("THREAD %d REDUCING %s", thread_num, s->word);
		sem_wait(&printer_sem);
		SLInsert(printer, s->word, 1);
		sem_post(&printer_sem);
	}
}

/* redwordcount_t (Reduce Word Count_Thread) takes the 
* word out of the hash table and inserts it into 
* the printer linked list*/
void *redwordcount_t(void* pargs)
{
	toThread* args = pargs;
	int thread_num = args->rednum_t;
	keyv* work_start = args->to_red_ptr_tracker;
	
	keyv *s, *tmp;
	HASH_ITER(hh, work_start, s, tmp)
	{
		if(((s->hh.hashv) % rednum) != thread_num)
		{
			break;
		}
		sem_wait(&printer_sem);
		SLInsertNoDup(printer, s->word, 1);
		sem_post(&printer_sem);
	}
}

/* mapintsort_t (Map Integer Sort_Thread) reads the file into a 
* temporary linked list. The elements of the linked list are
* later inserted into the hash_table in the proper format.*/
void *mapintsort_t(void* pargs)
{
	toThread* args = pargs;
	FILE* book = args->file2threads;
	char read_buffer[1024];
	char* word;
	char* toksavptr;
	
	SortedListPtr int_store;
	
	int_store = SLCreate(compareInts);
	
	while(fgets(read_buffer, 1024, book) != NULL)
	{
		printf("INSERTING %s", read_buffer);
		SLInsert(int_store, read_buffer, 1);
	}
	
	if(int_store->next == NULL)
	{
		pthread_exit(0);
	}
	SortedListPtr temp = int_store->next;
	while (temp->next != NULL)
	{
		keyv* current = malloc(sizeof(keyv));
		current->word = calloc(strlen(temp->something), sizeof(char));
		strcpy(current->word, temp->something);
		printf("MAPPING %s", current->word);
		sem_wait(&hash_sem);
		HASH_ADD_KEYPTR(hh, hash_table, current->word, strlen(current->word), current);
		sem_post(&hash_sem);
		temp = temp->next;
	}
	
	keyv* current = malloc(sizeof(keyv));
	current->word = calloc(strlen(temp->something), sizeof(char));
	strcpy(current->word, temp->something);
	sem_wait(&hash_sem);
	HASH_ADD_KEYPTR(hh, hash_table, current->word, strlen(current->word), current);
	sem_post(&hash_sem);
	
	SLDestroy(int_store);
	
}

/* mapwordcount_t (Map Word Count_Thread) reads the file into a 
* temporary linked list. The elements of the linked list are
* later inserted into the hash_table in the proper format.*/
void *mapwordcount_t(void* pargs)
{
	toThread* args = pargs;
	FILE* book = args->file2threads;
	char read_buffer[1024];
	char* word;
	const char* delimiters_word = " \f\n\r\t;()\"\'<>:{}[]|+-=&*#%$!?@^_.,/\\~0123456789";
	char* toksavptr;
	
	SortedListPtr word_store;
	
	word_store = SLCreate(compareStrings);
	
	while(fgets(read_buffer, 1024, book) != NULL)
	{
		word = strtok_r(read_buffer, delimiters_word, &toksavptr);
		
		while(word != NULL)
		{
			int k = 0;
			for(; word[k]; k++)
			{
				word[k] = tolower(word[k]);
			}
			
			SLInsert(word_store, word, 1);
			word = strtok_r(NULL, delimiters_word, &toksavptr);
		}
	}
	
	if(word_store->next == NULL)
	{
		pthread_exit(0);
	}
	SortedListPtr temp = word_store->next;
	while (temp->next != NULL)
	{
		keyv* current = malloc(sizeof(keyv));
		current->word = calloc(strlen(temp->something), sizeof(char));
		strcpy(current->word, temp->something);
		sem_wait(&hash_sem);
		HASH_ADD_KEYPTR(hh, hash_table, current->word, strlen(current->word), current);
		sem_post(&hash_sem);
		temp = temp->next;
	}
	
	keyv* current = malloc(sizeof(keyv));
	current->word = calloc(strlen(temp->something), sizeof(char));
	strcpy(current->word, temp->something);
	sem_wait(&hash_sem);
	HASH_ADD_KEYPTR(hh, hash_table, current->word, strlen(current->word), current);
	sem_post(&hash_sem);
	
	SLDestroy(word_store);
	
}

/* mod_sort is the sort function uthash uses to sort the hash table
* into hash(key) mod R order where R is the number of reduce threads.*/
int mod_sort(keyv *a, keyv *b)
{
	return (((a->hh.hashv) % rednum) - ((b->hh.hashv) % rednum));
}
