/**
 * Copyright (c) 2018-2022, NXOS Development Team
 * SPDX-License-Identifier: Apache-2.0
 * 
 * Contains: Page cache alloctor
 * 
 * Change Logs:
 * Date           Author            Notes
 * 2021-10-24     JasonHu           Init
 */

#include <mm/page.h>
#include <mm/page_cache.h>
#include <mm/buddy.h>
#include <utils/memory.h>
#include <sched/mutex.h>

#define NX_LOG_LEVEL NX_LOG_INFO
#define NX_LOG_NAME "PageCache"
#include <utils/log.h>
#include <xbook/debug.h>

/**
 * Maximum number of pages in small span
 */
#define SMALL_SPAN_PAGES_MAX  128

/**
 * max allowed pages to allocate by system.
 */
#define NX_PAGE_CACHE_MAX_PAGES  2048

/**
 * max threshold for free large span.
 * if free large list span count beyond thresold,
 * free span will directly free to buddy system.
 */
#define LARGE_SPAN_FREE_THRESHOLD_MAX  32

/**
 * max threshold for free small span.
 * if free small list span count beyond thresold,
 * free span will directly free to buddy system.
 */
#define SMALL_SPAN_FREE_THRESHOLD_MAX  64

/**
 * max threshold for free one page span.
 * one page span will used more 
 */
#define ONE_PAGE_SPAN_FREE_THRESHOLD_MAX  1024

/**
 * span mark use to covert page to span
 */
struct SpanMark
{
    NX_U16 count;  /* span page count */
    NX_U16 idx;    /* page idx int span */
};
typedef struct SpanMark SpanMark;

struct PageCache
{
    NX_List spanFreeList[SMALL_SPAN_PAGES_MAX];  /* list head for span list */
    NX_List largeSpanFreeList;     /* list for large span page */
    NX_Atomic spanFreeCount[SMALL_SPAN_PAGES_MAX];
    NX_Atomic largeSpanFreeCount;
};
typedef struct PageCache PageCache;

NX_PRIVATE PageCache PageCacheObject;
NX_PRIVATE SpanMark *SpanMarkMap;
NX_PRIVATE void *SpanBaseAddr;
NX_PRIVATE NX_Mutex PageCacheLock;

NX_PRIVATE void *PageAllocVirtual(NX_Size count)
{
    void *ptr = NX_PageAlloc(count);
    if (ptr == NX_NULL)
    {
        return NX_NULL;
    }
    return NX_Phy2Virt(ptr);
}

NX_PRIVATE void PageFreeVirtual(void *ptr)
{
    if (ptr == NX_NULL)
    {
        return;
    }
    NX_PageFree(NX_Virt2Phy(ptr));
}

/**
 * Mark spans for count pages
 */
NX_PRIVATE void MarkSpan(void *span, NX_Size count)
{
    NX_Addr dis = (NX_Addr)span - (NX_Addr)SpanBaseAddr;
    NX_Size idx = dis >> NX_PAGE_SHIFT;

    SpanMark *mark = SpanMarkMap + idx;
    int i;
    for (i = 0; i < count; i++)
    {
        mark->idx = i;
        mark->count = count;
        mark++;
    }
}

/**
 * clear span for count pages
 */
NX_PRIVATE void ClearSpan(void *span, NX_Size count)
{
    NX_Addr dis = (NX_Addr)span - (NX_Addr)SpanBaseAddr;
    NX_Size idx = dis >> NX_PAGE_SHIFT;

    SpanMark *mark = SpanMarkMap + idx;

    int i;
    for (i = 0; i < count; i++)
    {
        mark->idx = 0;
        mark->count = 0;
        mark++;
    }
}

void *NX_PageToSpan(void *page)
{
    NX_ASSERT(page != NX_NULL);
    if (page < SpanBaseAddr)
    {
        return NX_NULL;
    }
    
    NX_Size dis = (NX_Addr)page - (NX_Addr)SpanBaseAddr;
    NX_Size idx = dis >> NX_PAGE_SHIFT;

    SpanMark *mark = SpanMarkMap + idx;
    return (void *)((NX_Addr)page - mark->idx * NX_PAGE_SIZE);
}

NX_Size NX_SpanToCount(void *span)
{
    NX_ASSERT(span != NX_NULL);
    if (span < SpanBaseAddr)
    {
        return 0;
    }
    
    NX_Size dis = (NX_Addr)span - (NX_Addr)SpanBaseAddr;
    NX_Size idx = dis >> NX_PAGE_SHIFT;

    SpanMark *mark = SpanMarkMap + idx;
    return mark->count;
}

NX_PRIVATE void *__PageCacheAlloc(NX_Size count)
{
    int isLargeSpan = 0;
    NX_List *listHead;
    NX_PageSpan *spanNode = NX_NULL;
    NX_PageSpan *spanNodeBest = NX_NULL;
    NX_Atomic *freeCount = NX_NULL;

    if (count >= SMALL_SPAN_PAGES_MAX)    /* alloc from large list */
    {
        isLargeSpan = 1;
        listHead = &PageCacheObject.largeSpanFreeList;
        freeCount = &PageCacheObject.largeSpanFreeCount;
    }
    else    /* alloc from normal list */
    {
        listHead = &PageCacheObject.spanFreeList[count];
        freeCount = &PageCacheObject.spanFreeCount[count];
    }

    if (NX_ListEmpty(listHead)) /* cache list empty, alloc from page system */
    {
        /* alloc from buddy system */
        void *span = PageAllocVirtual(count);
        if (span == NX_NULL)
        {
            NX_LOG_E("no enough memroy to allocate for %d pages!", count);
            return NX_NULL;
        }
        MarkSpan(span, count);
        return span;
    }
    else /* cache list not empty, alloc from list */
    {
        if (isLargeSpan)
        {
            /* use best fit to alloc a span */
            NX_ListForEachEntry (spanNode, &PageCacheObject.largeSpanFreeList, list)
            {
                if (spanNodeBest == NX_NULL)
                {
                    spanNodeBest = spanNode;
                }
                if (spanNode != spanNodeBest && spanNode->pageCount < spanNodeBest->pageCount)
                {
                    spanNodeBest = spanNode;
                }
            }
        }
        else
        {
            /* use first fit to alloc a span */
            spanNodeBest = NX_ListFirstEntry(&PageCacheObject.spanFreeList[count], NX_PageSpan, list);
        }
        /* del span node from list */
        NX_ListDelInit(&spanNodeBest->list);
        NX_AtomicDec(freeCount);

        /* return base addr as spin */
        return (void *)spanNodeBest;
    }
}

/**
 * alloc span from heap, if no free page, alloc from buddy system
 */
void *NX_PageCacheAlloc(NX_Size count)
{
    if (!count)
    {
        NX_LOG_E("alloc page count is 0!");
        return NX_NULL;
    }
    else if (count > NX_PAGE_CACHE_MAX_PAGES)
    {
        NX_LOG_E("alloc page count beyond %d", NX_PAGE_CACHE_MAX_PAGES);
        return NX_NULL;
    }
    
    NX_MutexLock(&PageCacheLock);
    void *ptr = __PageCacheAlloc(count);
    NX_MutexUnlock(&PageCacheLock);
    return ptr;
}

NX_PRIVATE NX_Error __PageCacheFree(void *page)
{
    void *span = NX_PageToSpan(page);
    if (span == NX_NULL)
    {
        NX_LOG_E("page cache free invalid page %p", page);
        return NX_EINVAL;
    }

    NX_Size count = NX_SpanToCount(span);
    if (!count)
    {
        return NX_EFAULT;
    }

    NX_Atomic *freeCount = NX_NULL;

    if (!count)
    {
        NX_LOG_E("span count is 0!");
        return NX_EPERM;
    }

    NX_List *listHead;
    NX_Size maxThresold = 0;

    if (count >= SMALL_SPAN_PAGES_MAX)    /* free to large list */
    {
        maxThresold = LARGE_SPAN_FREE_THRESHOLD_MAX;
        listHead = &PageCacheObject.largeSpanFreeList;
        freeCount = &PageCacheObject.largeSpanFreeCount;
    }
    else    /* free from normal list */
    {
        if (count == 1) /* one page special */
        {
            maxThresold = ONE_PAGE_SPAN_FREE_THRESHOLD_MAX;
        }
        else
        {
            maxThresold = SMALL_SPAN_FREE_THRESHOLD_MAX;
        }
        listHead = &PageCacheObject.spanFreeList[count];
        freeCount = &PageCacheObject.spanFreeCount[count];
    }

    if (NX_AtomicGet(freeCount) >= maxThresold)    /* directly free */
    {
        ClearSpan(span, count);
        PageFreeVirtual(span);
    }
    else    /* add to list for cache */
    {
        NX_PageSpan *spanNode = (NX_PageSpan *)span;
        spanNode->pageCount = count;
        NX_ListAdd(&spanNode->list, listHead);
        NX_AtomicInc(freeCount);
    }
    return NX_EOK;
}

NX_Error NX_PageCacheFree(void *page)
{
    if (page == NX_NULL)
    {
        NX_LOG_E("free NX_NULL page!");
        return NX_EINVAL;
    }
    
    NX_MutexLock(&PageCacheLock);
    NX_Error err = __PageCacheFree(page);
    NX_MutexUnlock(&PageCacheLock);
    return err;
}

void NX_PageCacheInit(void)
{
    int i;
    for (i = 0; i < NX_ARRAY_SIZE(PageCacheObject.spanFreeList); i++)
    {
        NX_ListInit(&PageCacheObject.spanFreeList[i]);
        NX_AtomicSet(&PageCacheObject.spanFreeCount[i], 0);
    }
    NX_ListInit(&PageCacheObject.largeSpanFreeList);
    NX_AtomicSet(&PageCacheObject.largeSpanFreeCount, 0);

    SpanBaseAddr = NX_PageZoneGetBase(NX_PAGE_ZONE_NORMAL);
    NX_LOG_I("span base addr: %p", SpanBaseAddr);

    NX_Size pages = NX_PageZoneGetPages(NX_PAGE_ZONE_NORMAL);
    /* alloc span mark array */
    NX_Size spanMarkPages = NX_DIV_ROUND_UP(pages * sizeof(SpanMark), NX_PAGE_SIZE);
    NX_LOG_I("span mark used page: %d", spanMarkPages);

    SpanMarkMap = PageAllocVirtual(spanMarkPages);
    if (SpanMarkMap == NX_NULL)
    {
        NX_PANIC("alloc page for span mark failed!");
    }
    NX_MemZero(SpanMarkMap, spanMarkPages * NX_PAGE_SIZE);

    NX_MutexInit(&PageCacheLock);
}
