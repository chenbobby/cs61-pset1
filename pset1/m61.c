#define M61_DISABLE 1
#include "m61.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include <assert.h>

#define MEMORY_CAPACITY (size_t) -1

struct metadata {
    size_t data_size;
};


//+ Initialize statistics object with values 0
static struct m61_statistics stat_store = {0, 0, 0, 0, 0, 0, NULL, NULL};

/// m61_malloc(sz, file, line)
///    Return a pointer to `sz` bytes of newly-allocated dynamic memory.
///    The memory is not initialized. If `sz == 0`, then m61_malloc may
///    either return NULL or a unique, newly-allocated pointer value.
///    The allocation request was at location `file`:`line`.

void* m61_malloc(size_t sz, const char* file, int line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings

    if(sz == (size_t) -1){
        //+ Requested Memory Size Too Large


        //+ Increment failed allocations count
        stat_store.nfail++;

        //+ Update failed allocation size
        stat_store.fail_size += sz;

        return NULL;
    }

    void* block = base_malloc(sizeof(struct metadata) + sz);


    if (block == NULL){
        //+ Memory Allocation Failure 
        

        //+ Increment failed allocations count
        stat_store.nfail++;

        //+ Update failed allocation size
        stat_store.fail_size += sz;

        return block;

    }

    //+ Memory Allocation Success
    
    

    //+ Create metadata pointer and set metadata
    struct metadata* md_ptr = (struct metadata*) block;            //+ cast void* to size_t* so a value can be stored
    md_ptr->data_size = sz;

    //+ Create data pointer
    void* data = block + sizeof(struct metadata);


    //+ Increment total allocations count
    stat_store.ntotal++;

    //+ Increment active allocations count
    stat_store.nactive++;

    //+ Update total allocation size
    stat_store.total_size += md_ptr->data_size;

    //+ Update active allocation size
    stat_store.active_size += md_ptr->data_size;


    //+ Update heap_min by comparing address values
    if (stat_store.heap_min == NULL || (uintptr_t) data <= (uintptr_t) stat_store.heap_min) {
        stat_store.heap_min = data;
    }

    //+ Update heap_max by comparing address values
    if (stat_store.heap_max == NULL || (uintptr_t) data + sz >= (uintptr_t) stat_store.heap_max) {
        stat_store.heap_max = data + sz;
    }


    return data;
}


/// m61_free(ptr, file, line)
///    Free the memory space pointed to by `ptr`, which must have been
///    returned by a previous call to m61_malloc and friends. If
///    `ptr == NULL`, does nothing. The free was called at location
///    `file`:`line`.

void m61_free(void *ptr, const char *file, int line) {
    (void) file, (void) line;   // avoid uninitialized variable warnings

    if (ptr == NULL) {
        //+ Edge Case: free(NULL) is defined to not do anything.
        return;
    }

    //+ Decrement active allocations count
    stat_store.nactive--;

    //+ Update active allocation size
    size_t* data_size = ptr - sizeof(struct metadata);
    stat_store.active_size -= *data_size;


    base_free(ptr);
}


/// m61_realloc(ptr, sz, file, line)
///    Reallocate the dynamic memory pointed to by `ptr` to hold at least
///    `sz` bytes, returning a pointer to the new block. If `ptr` is NULL,
///    behaves like `m61_malloc(sz, file, line)`. If `sz` is 0, behaves
///    like `m61_free(ptr, file, line)`. The allocation request was at
///    location `file`:`line`.

void* m61_realloc(void* ptr, size_t sz, const char* file, int line) {
    void* new_ptr = NULL;
    if (sz) {

        //+ Allocate using m61_alloc, which updates stats and makes metadata already
        new_ptr = m61_malloc(sz, file, line);
        
    }
    if (ptr && new_ptr) {
        // Copy the data from `ptr` into `new_ptr`.
        // To do that, we must figure out the size of allocation `ptr`.
        // Your code here (to fix test014).
        
        //+ Copy data from ptr to new_ptr
        size_t* old_size = ptr - sizeof(struct metadata);
        memcpy(new_ptr, ptr, *old_size);

    }
    m61_free(ptr, file, line);
    return new_ptr;
}


/// m61_calloc(nmemb, sz, file, line)
///    Return a pointer to newly-allocated dynamic memory big enough to
///    hold an array of `nmemb` elements of `sz` bytes each. The memory
///    is initialized to zero. If `sz == 0`, then m61_malloc may
///    either return NULL or a unique, newly-allocated pointer value.
///    The allocation request was at location `file`:`line`.

void* m61_calloc(size_t nmemb, size_t sz, const char* file, int line) {
    // Your code here (to fix test016).
    void* ptr = m61_malloc(nmemb * sz, file, line);
    if (ptr) {
        memset(ptr, 0, nmemb * sz);
    }
    return ptr;
}


/// m61_getstatistics(stats)
///    Store the current memory statistics in `*stats`.

void m61_getstatistics(struct m61_statistics* stats) {
    //+ Retrieve statistics from global store
    stats->nactive = stat_store.nactive;
    stats->active_size = stat_store.active_size;
    stats->ntotal = stat_store.ntotal;
    stats->total_size = stat_store.total_size;
    stats->nfail = stat_store.nfail;
    stats->fail_size = stat_store.fail_size;
    stats->heap_min = stat_store.heap_min;
    stats->heap_max = stat_store.heap_max;
}


/// m61_printstatistics()
///    Print the current memory statistics.

void m61_printstatistics(void) {
    struct m61_statistics stats;
    m61_getstatistics(&stats);


    printf("malloc count: active %10llu   total %10llu   fail %10llu\n",
           stats.nactive, stats.ntotal, stats.nfail);
    printf("malloc size:  active %10llu   total %10llu   fail %10llu\n",
           stats.active_size, stats.total_size, stats.fail_size);
}


/// m61_printleakreport()
///    Print a report of all currently-active allocated blocks of dynamic
///    memory.

void m61_printleakreport(void) {
    // Your code here.
}
