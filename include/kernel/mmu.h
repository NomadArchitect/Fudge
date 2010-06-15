#ifndef MMU_H
#define MMU_H

#define MMU_ERROR_PRESENT  0x01
#define MMU_ERROR_RW       0x02
#define MMU_ERROR_USER     0x04
#define MMU_ERROR_RESERVED 0x08
#define MMU_ERROR_FETCH    0x16
#define MMU_FRAME_SIZE     0x1000
#define MMU_PAGE_SIZE      0x1000

typedef struct mmu_page
{

    uint32_t present   : 1;
    uint32_t writeable : 1;
    uint32_t usermode  : 1;
    uint32_t accessed  : 1;
    uint32_t dirty     : 1;
    uint32_t unused    : 7;
    uint32_t frame     : 20;

} mmu_page_t;

typedef struct mmu_table
{

    mmu_page_t pages[1024];

} mmu_table_t;

typedef struct mmu_directory
{

    mmu_table_t *tables[1024];
    uint32_t tablesPhysical[1024];
    uint32_t physicalAddress;

} mmu_directory_t;

extern void mmu_flush(uint32_t *tablesPhysical);

extern void mmu_set_directory(mmu_directory_t *directory);
extern void mmu_handler(registers_t *r);
extern void mmu_init();

#endif

