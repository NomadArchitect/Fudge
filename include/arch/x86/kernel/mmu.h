#ifndef ARCH_MMU_H
#define ARCH_MMU_H

#define MMU_ERROR_PRESENT  1 << 0
#define MMU_ERROR_RW       1 << 1
#define MMU_ERROR_USER     1 << 2
#define MMU_ERROR_RESERVED 1 << 3
#define MMU_ERROR_FETCH    1 << 4

#define MMU_DIRECTORY_SIZE    1024

#define MMU_TABLE_ADDRESS           0x197000
#define MMU_TABLE_SIZE              1024
#define MMU_TABLE_FLAG_PRESENT      1 << 0
#define MMU_TABLE_FLAG_WRITEABLE    1 << 1
#define MMU_TABLE_FLAG_USERMODE     1 << 2
#define MMU_TABLE_FLAG_CACHEWRITE   1 << 3
#define MMU_TABLE_FLAG_CACHEDISABLE 1 << 4
#define MMU_TABLE_FLAG_ACCESSED     1 << 5
#define MMU_TABLE_FLAG_LARGE        1 << 6
#define MMU_TABLE_FLAG_IGNORED      1 << 7

#define MMU_PAGE_FLAG_PRESENT      1 << 0
#define MMU_PAGE_FLAG_WRITEABLE    1 << 1
#define MMU_PAGE_FLAG_USERMODE     1 << 2
#define MMU_PAGE_FLAG_CACHEWRITE   1 << 3
#define MMU_PAGE_FLAG_CACHEDISABLE 1 << 4
#define MMU_PAGE_FLAG_ACCESSED     1 << 5
#define MMU_PAGE_FLAG_DIRTY        1 << 6
#define MMU_PAGE_FLAG_GLOBAL       1 << 7

struct isr_registers;

struct mmu_table
{

    unsigned int entries[MMU_TABLE_SIZE];

} __attribute__((aligned(4096)));

struct mmu_directory
{

    struct mmu_table *tables[MMU_DIRECTORY_SIZE];

} __attribute__((aligned(4096)));

extern void mmu_handler(struct isr_registers *registers);
extern void mmu_init();

#endif

