#ifndef PAGING_H
#define PAGING_H
#include <inttypes.h>



#define PAGE_PRESENT 1
#define PAGE_RW      2
#define PAGE_USER    4
#define PAGE_DIRTY   16

typedef struct {
        uint32_t present    : 1;   // Page present in memory
        uint32_t rw         : 1;   // Read-only if clear, readwrite if set
        uint32_t user       : 1;   // Supervisor level only if clear
        uint32_t accessed   : 1;   // Has the page been accessed since last refresh?
        uint32_t dirty      : 1;   // Has the page been written to since last refresh?
        uint32_t unused     : 7;   // Amalgamation of unused and reserved bits
        uint32_t page       : 20;  // Page address (shifted right 12 bits)
} page_table_entry_t;

typedef union {
    page_table_entry_t page_entry;
    uint32_t value;
} PTE; // PTE = Page Table Entry 

typedef PTE * PageTable;

typedef struct {
        uint32_t present    : 1;   // Page table is present in memory
        uint32_t rw         : 1;   // Read-only if clear, readwrite if set
        uint32_t user       : 1;   // Supervisor level only if clear
        uint32_t reserved   : 9;   // Some useful bits not we dont use them for now
        uint32_t page_table : 20;  // Page table address
} page_dir_entry_t;

typedef union {
    page_dir_entry_t dir_entry;
    uint32_t value;
} PDE;

typedef PDE * PageDirectory;

PageDirectory initialise_paging();
PageTable alloc_page_entry(uint32_t address, int is_writeable, int is_kernel );

#endif
