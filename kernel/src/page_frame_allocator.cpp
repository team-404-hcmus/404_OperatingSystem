#include "paging/page_frame_allocator.h"


uint64_t last_frame_allocated = 0;
void PageFrameAllocator::ReadEFIMemory(EFI_MEMORY_DESCRIPTOR* mMap,size_t mapSize,size_t DescriptorSize,uint64_t kernel_size,void* kernel_base) 
{
	if(initialize) return;
	initialize = true;
	uint64_t entries = mapSize/DescriptorSize;
	void* largest_free_segment = NULL;
	uint64_t largest_free_segment_size = 0;
	EFI_MEMORY_DESCRIPTOR* desc = mMap;
	for(uint64_t i = 0; i < entries; ++i)
	{
		if(desc->Type == EFI_CONVENTIONAL_MEMORY) //EFI conventional memory
		{
			uint64_t size=desc->numpages*4096;
			if(size >largest_free_segment_size)
			{
				largest_free_segment = desc->physical_addr;
				largest_free_segment_size = size;
			}
		}
		desc = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)desc+DescriptorSize);
	}
	uint64_t total_memory = GetMemorySize(mMap,entries,DescriptorSize);
	freeMemory = total_memory;
	uint64_t bitmap_size =largest_free_segment_size/4096/8 +1;
	page_bitmap.init((uint8_t*)largest_free_segment,bitmap_size);
	LockPages(page_bitmap.Buffer,bitmap_size/4096+1);
	LockPages(kernel_base,kernel_size/4096+1);
	desc = mMap;
	for(uint64_t i = 0; i < entries; ++i)
	{
		if(desc->Type != EFI_CONVENTIONAL_MEMORY) //EFI conventional memory
		{
			ReservePages(desc->physical_addr,desc->numpages);
		}
		desc = (EFI_MEMORY_DESCRIPTOR*)((uint8_t*)desc+DescriptorSize);
	}
}

void PageFrameAllocator::FreePage(void* addr) 
{
	uint64_t index = (uint64_t)addr/4096;
	if(page_bitmap[index] == false) return;
	page_bitmap.set(index,false);
	freeMemory += 4096;
	usedMemory -= 4096;
}
void PageFrameAllocator::LockPage(void* addr) 
{
	uint64_t index = (uint64_t)addr/4096;
	if(page_bitmap[index] == true) return;
	page_bitmap.set(index,true);
	freeMemory -= 4096;
	usedMemory += 4096;
}

void PageFrameAllocator::FreePages(void* addr,uint64_t count) 
{
	for(uint64_t i =0; i < count; i++)
	{
		FreePage((void*)((uint64_t)addr+(i*4096)));
		if(last_frame_allocated > ((uint64_t)addr/0x1000)) last_frame_allocated = (uint64_t)addr;
	}
}

void PageFrameAllocator::LockPages(void* addr,uint64_t count) 
{
	for(uint64_t i =0; i < count; i++)
	{
		LockPage((void*)((uint64_t)addr+(i*4096)));
	}
}

void PageFrameAllocator::init() 
{
	usedMemory = 0;
	freeMemory = 0;
	reservedMemory =0;
	initialize = false;
}
void PageFrameAllocator::ReservePage(void* addr) 
{
	uint64_t index = (uint64_t)addr/4096;
	if(page_bitmap[index] == true) return;
	page_bitmap.set(index,true);
	freeMemory -= 4096;
	reservedMemory += 4096;
}
void PageFrameAllocator::unReservePage(void* addr) 
{
	uint64_t index = (uint64_t)addr/4096;
	if(page_bitmap[index] == false) return;
	page_bitmap.set(index,false);
	freeMemory += 4096;
	reservedMemory -= 4096;
}

void PageFrameAllocator::ReservePages(void* addr,uint64_t count) 
{
	for(uint64_t i =0; i < count; i++)
	{
		ReservePage((void*)((uint64_t)addr+(i*4096)));
	}
}

void PageFrameAllocator::unReservePages(void* addr,uint64_t count) 
{
	for(uint64_t i =0; i < count; i++)
	{
		unReservePage((void*)((uint64_t)addr+(i*4096)));
		if(last_frame_allocated > ((uint64_t)addr/0x1000)) last_frame_allocated = (uint64_t)addr;
	}
}

uint64_t PageFrameAllocator::getFreeMemory() 
{
	return freeMemory;
}

uint64_t PageFrameAllocator::getReservedMemory() 
{
	return reservedMemory;
}

uint64_t PageFrameAllocator::getUsedMemory() 
{
	return usedMemory;
}

void* PageFrameAllocator::requestPage() 
{
    for (; last_frame_allocated < page_bitmap.Size * 8; last_frame_allocated++){
        if (page_bitmap[last_frame_allocated] == true) continue;
        LockPage((void*)(last_frame_allocated * 4096));
        return (void*)(last_frame_allocated * 4096);
    }
    return NULL; // Page Frame Swap to file
}

PageFrameAllocator gPageFrameAllocator;