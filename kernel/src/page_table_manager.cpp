#include "paging/page_table_manager.h"
#include "stdout.h"
PageTable *PML4 = NULL;
PageTableManager gPageTableManager;


PageTableManager::PageTableManager(PageTable* PML4Address){
    this->PML4 = PML4Address;
}

void PageTableManager::MapMemory(void* virtualMemory, void* physicalMemory){
    PageMapIndexer indexer = PageMapIndexer(virtualMemory);
    PageDirectoryEntry PDE;
    PDE = PML4->entries[indexer.PDP_i];
    PageTable* PDP;
    if (!PDE.Present){
        PDP = (PageTable*)gPageFrameAllocator.requestPage();
        memset(PDP, 0, 0x1000);
        PDE.Address = (uint64_t)PDP >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PML4->entries[indexer.PDP_i] = PDE;
    }
    else
    {
        PDP = (PageTable*)((uint64_t)PDE.Address << 12);
    }
    PDE = PDP->entries[indexer.PD_i];
    PageTable* PD;
    if (!PDE.Present){
        PD = (PageTable*)gPageFrameAllocator.requestPage();
        memset(PD, 0, 0x1000);
        PDE.Address = (uint64_t)PD >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PDP->entries[indexer.PD_i] = PDE;
    }
    else
    {
        PD = (PageTable*)((uint64_t)PDE.Address << 12);
    }

    PDE = PD->entries[indexer.PT_i];
    PageTable* PT;
    if (!PDE.Present){
        PT = (PageTable*)gPageFrameAllocator.requestPage();
        memset(PT, 0, 0x1000);
        PDE.Address = (uint64_t)PT >> 12;
        PDE.Present = true;
        PDE.ReadWrite = true;
        PD->entries[indexer.PT_i] = PDE;
    }
    else
    {
        PT = (PageTable*)((uint64_t)PDE.Address << 12);
    }

    PDE = PT->entries[indexer.P_i];
    PDE.Address = (uint64_t)physicalMemory >> 12;
    PDE.Present = true;
    PDE.ReadWrite = true;
    PT->entries[indexer.P_i] = PDE;
}