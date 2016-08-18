/************************************************************************/
/*  Isis Language Interpreter -- Source Code                            */
/*  (c) MIT Media Lab -- Stefan Agamanolis                              */
/************************************************************************/

/* This file contains low-level memory management functions */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "isis.h"

static Memory *mem_head = NULL;
static Memory *mem_tail = NULL;
static int mem_num_kinds = 0;
static long mem_tot_allocated = 0;
static long mem_tot_addr_space = 0;

/*-------------------------------------------------------------*/
void *mem_alloc(int n)
{
  /* Allocate the memory and exit with message if we are out of memory */
  
  void *ptr;
  
  ptr = malloc(n);
  
  if(ptr == NULL) {
    fprintf(stderr, "** Out of memory: mem_alloc of %d bytes failed.\n", n);
    exit(1);
  }  

  return ptr;
}

/*-------------------------------------------------------------*/
Memory *mem_new_memory(char *name, int size, int quantity, int increment)
{
  /* Allocates a new block of memory for objects of a certain size. 
     Returns NULL if out of memory.  */
  
  int i;
  Memory *mem;

  if(sizeof(unsigned long) != sizeof(void *)) {
    fprintf(stderr, 
	    "*===* The sizes of long integers and addresses are not equal.\n");
    fprintf(stderr, 
	    "*===* (unsigned long) is %d bytes, (void *) is %d bytes.\n",
	    sizeof(unsigned long), sizeof(void *));
    fprintf(stderr, 
	    "*===* You may have memory allocation problems.\n");
  }
  
  mem = (Memory *) mem_alloc(sizeof(Memory));

  mem->size = size;
  mem->quantity = quantity;
  mem->increment = increment;
  mem->num_used = 0;
  mem->tot_used = 0;
  mem->max_used = 0;

  /* allocate first block of storage */

  mem->blocks = (Block *) mem_alloc(sizeof(Block));
  mem->blocks->next = NULL;
  mem->blocks->memptr = (void *) mem_alloc(quantity * size);
  mem_tot_allocated += quantity * size;
  
  /* allocate and initialize the address space */

  mem->addrs = (void **) mem_alloc(quantity * sizeof(void *));
  mem_tot_addr_space += quantity * sizeof(void *);
  
  for(i = 0; i < quantity; i++) 
    mem->addrs[i] = (void *) 
      (((unsigned long) mem->blocks->memptr) + (size * i));
  
  /* copy the name */

    mem->name = (char *) mem_alloc(strlen(name) + 1 * sizeof(char));
    strcpy(mem->name, name);

  /* insert in list of all Memorys in use */

    if(mem_head == NULL) {
      mem->next = mem->prev = NULL;
      mem_head = mem_tail = mem;
    }
    else {
      mem->next = NULL;
      mem->prev = mem_tail;
      mem_tail->next = mem;
      mem_tail = mem;
    }
    mem_num_kinds++;

    return mem;
}

/*-------------------------------------------------------------*/
void mem_free_all(void)
{
  /* frees all memory previous allocated */

  Memory *nextmem, *mem;
  
  mem = mem_head;
  
  while(mem != NULL) {
    nextmem = mem->next;
    mem_free_memory(mem);
    mem = nextmem;
  }
}
  
/*-------------------------------------------------------------*/
void mem_free_memory(Memory *mem)
{
  /* free this particular memory */

  Block *block, *lastblock;

  if(mem == NULL) return;

  /* free name, address space, and actual memory blocks */

  free(mem->name);
  free(mem->addrs);

  block = mem->blocks;
  while(block != NULL) {
    free(block->memptr);
    lastblock = block;
    block = block->next;
    free(lastblock);
  }

  /* remove from list of all memories */
  
  if(mem == mem_head) mem_head = mem->next;
  if(mem == mem_tail) mem_tail = mem->prev;
  if(mem->prev != NULL) mem->prev->next = mem->next;
  if(mem->next != NULL) mem->next->prev = mem->prev;
  
  /* update statistics */

  mem_num_kinds--;
  mem_tot_allocated -= mem->quantity * mem->size;
  mem_tot_addr_space -= mem->quantity * sizeof(void *);

  /* free the memory structure itself */

  free(mem);
}

/*------------------------------------------------------------*/
int mem_increase_quantity(Memory *mem, int quantity_to_add)
{
  /* only call with memMutex locked */

  int i, old_quantity;
  void **old_addrs;
  Block *block;
  unsigned long newptr;

  /* fprintf(stderr, "*$*$*$* INCREASING %s quantity from %d to %d.\n", 
     mem->name, mem->quantity, mem->quantity + quantity_to_add); */
  
  if(quantity_to_add <= 0) return 0;
  old_quantity = mem->quantity;

  block = (Block *) mem_alloc(sizeof(Block));
  block->next = mem->blocks;
  mem->blocks = block;

  block->memptr = (void *) mem_alloc(quantity_to_add * mem->size);
  mem_tot_allocated += quantity_to_add * mem->size;
  newptr = (unsigned long) block->memptr;

  mem->quantity += quantity_to_add;
  old_addrs = mem->addrs;
  mem->addrs = (void **) mem_alloc(mem->quantity * sizeof(void *));
  mem_tot_addr_space += quantity_to_add * sizeof(void *);
  for(i = 0; i < old_quantity; i++) mem->addrs[i] = old_addrs[i];
  free(old_addrs);
  for(i = 0; i < quantity_to_add; i++) 
    mem->addrs[i + old_quantity] = (void *) (newptr + (mem->size * i));

    return 1;
}
    
/*-------------------------------------------------------------*/
void *mem_alloc_item(Memory *mem)
{
  /* No error checking done for speed!!! */
  
  void *ptr;
  
  isisMutexLock(memMutex);

  if(mem->num_used >= mem->quantity)  /* increase number of available items */
    mem_increase_quantity(mem, mem->increment);
  
  ptr = mem->addrs[mem->num_used++];

  /* uncomment the following to make the "total" and "max" statistics work */
  /* mem->tot_used++;
     if(mem->num_used > mem->max_used) mem->max_used++; */

  isisMutexUnlock(memMutex);

  return ptr;
}
  
/*-------------------------------------------------------------*/
void mem_freeItem(Memory *mem, void *ptr)
{
  /* No error checking done for speed!!! */

  isisMutexLock(memMutex);

  mem->addrs[--(mem->num_used)] = ptr;

  isisMutexUnlock(memMutex);
}
  
/*-------------------------------------------------------------*/
void mem_print_stats(FILE *fd)
{
  Memory *mem;
  
  isisMutexLock(memMutex);

  mem = mem_head;

  fprintf(fd, "****************************************");
  fprintf(fd, "***************************************\n");
  fprintf(fd, "Memory statistics:\n");
  fprintf(fd, "\n");
  fprintf(fd, "%12.12s %10.10s %10.10s %10.10s %10.10s %10.10s %10.10s\n",
	  "Kind", "Size", "Avail", "Current", "Increment", "Maximum", "Total");
  fprintf(fd, "----------------------------------------");
  fprintf(fd, "---------------------------------------\n");

  while(mem != NULL) {
    fprintf(fd, "%12.12s %10d %10d %10d %10d %10d %10d\n",
	    mem->name, mem->size, mem->quantity,
	    mem->num_used, mem->increment, mem->max_used, mem->tot_used);
    mem = mem->next;
  }

  fprintf(fd, "\n");
  fprintf(fd, "The size of an address is %d bytes (%d bits).\n",
	  sizeof(void *), sizeof(void *) * 8);
  fprintf(fd, "Total storage area allocated:  %10ld bytes\n", 
	  mem_tot_allocated); 
  fprintf(fd, "Total address area allocated:  %10ld bytes\n", 
	  mem_tot_addr_space);
  fprintf(fd, "Grand total:                   %10ld bytes\n", 
	  mem_tot_allocated + mem_tot_addr_space);
  fprintf(fd, "\n");

  fprintf(fd, "****************************************");
  fprintf(fd, "***************************************\n");

  isisMutexUnlock(memMutex);
}

/*-------------------------------------------------------------*/
void mem_reset_all_stats(void)
{
  Memory *mem;
  
  isisMutexLock(memMutex);
  
  mem = mem_head;
  
  while(mem != NULL) {
    mem->max_used = mem->tot_used = 0;
    mem = mem->next;
  }
  
  isisMutexUnlock(memMutex);
}
