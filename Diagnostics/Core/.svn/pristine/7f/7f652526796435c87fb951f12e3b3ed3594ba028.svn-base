/*
    File    :   LinkedList.c
    Title   :
    Author  :   jc45540
    Created :   02/04/2016
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:    Object-oriented Linked List implementation

===============================================================================
*/

#include "project.h"
#include "LinkedList.h"
#include "BufferManager.h"

static void LinkedList_Dtor( LinkedList_t* self );
static BOOL LinkedList_Push( LinkedList_t* self, void* pItem );
static void* LinkedList_Pop( LinkedList_t* self );
static BOOL LinkedList_Full( LinkedList_t* self );
static BOOL LinkedList_Empty( LinkedList_t* self );
static BOOL LinkedList_Delete( LinkedList_t* self, uint32_t index );
static BOOL LinkedList_Insert( LinkedList_t* self, uint32_t index, void* pItem );
static void* LinkedList_Peek( LinkedList_t* self, uint32_t Index );
static BOOL LinkedList_SetMaxSize( LinkedList_t* self, uint32_t Size );
static uint32_t LinkedList_GetMaxSize( LinkedList_t* self );
static uint32_t LinkedList_GetSize( LinkedList_t* self );

void LinkedList_Ctor( struct tag_LinkedList* self )
{
    // Initialize attributes
    self->pHead = NULL;
    self->pTail = NULL;
    self->NItems = 0;
    self->MaxItems = LINKEDLIST_MAX_SIZE;

    // Set method pointers
    self->Dtor = LinkedList_Dtor;
    self->Push = LinkedList_Push;
    self->Pop = LinkedList_Pop;
    self->Full = LinkedList_Full;
    self->Empty = LinkedList_Empty;
    self->Delete = LinkedList_Delete;
    self->Insert = LinkedList_Insert;
    self->Peek = LinkedList_Peek;
    self->SetMaxSize = LinkedList_SetMaxSize;
    self->GetMaxSize = LinkedList_GetMaxSize;
    self->GetSize = LinkedList_GetSize;
}

void LinkedList_Dtor( LinkedList_t* self )
{
    LinkedListItem_t* pListItem = self->pHead;
    LinkedListItem_t* pNextItem;
    for ( pListItem = self->pHead; pListItem != NULL; pListItem = pNextItem )
    {
        pNextItem = pListItem->pNextItem;
        BufferManagerFreeBuffer( pListItem );
    }
    self->pHead = NULL;
    self->pTail = NULL;
    self->NItems = 0;
}

static BOOL LinkedList_Push( LinkedList_t* self, void* pItem )
{
    BOOL result = FALSE;
    LinkedListItem_t* pListItem = BufferManagerGetBuffer( sizeof( LinkedListItem_t ) );

    if ( pListItem != NULL && self->NItems < self->MaxItems )
    {
        pListItem->pItem = pItem;
        pListItem->pNextItem = NULL;
        taskENTER_CRITICAL();
        if ( self->pHead == NULL )
        {
            self->pHead = pListItem;
            self->pTail = pListItem;
        }
        else
        {
            self->pTail->pNextItem = pListItem;
            self->pTail = pListItem;
        }
        self->NItems++;
        taskEXIT_CRITICAL();

        result = TRUE;
    }
    else
    {
        if ( pListItem != NULL )
        {
            BufferManagerFreeBuffer( pListItem );
        }
        result = FALSE;
    }

    return result;
}

static void* LinkedList_Pop( LinkedList_t* self )
{
    void* item;
    LinkedListItem_t* pListItem;

    if ( self->pHead != NULL )
    {
        taskENTER_CRITICAL();
        pListItem = self->pHead;
        item = pListItem->pItem;
        self->pHead = pListItem->pNextItem;
        if ( self->NItems > 0 )
        {
            self->NItems--;
        }
        taskEXIT_CRITICAL();
        BufferManagerFreeBuffer( pListItem );
    }
    else
    {
        item = NULL;
    }

    return item;
}

BOOL LinkedList_Full( LinkedList_t* self )
{
    return ( self->NItems >= self->MaxItems );
}

BOOL LinkedList_Empty( LinkedList_t* self )
{
    return ( self->NItems == 0 );
}

BOOL LinkedList_SetMaxSize( LinkedList_t* self, uint32_t Size )
{
    self->MaxItems = Size;
    return TRUE;
}

uint32_t LinkedList_GetMaxSize( LinkedList_t* self )
{
    return self->MaxItems;
}

uint32_t LinkedList_GetSize( LinkedList_t* self )
{
    return self->NItems;
}

static BOOL LinkedList_Delete( LinkedList_t* self, uint32_t index )
{
    BOOL result = FALSE;
    uint32_t i;
    LinkedListItem_t* pListItem;
    LinkedListItem_t* pPreviousListItem;

    taskENTER_CRITICAL();
    for ( i = 0, pListItem = self->pHead, pPreviousListItem = NULL; pListItem != NULL && i < index && i < self->NItems; i++, pPreviousListItem = pListItem, pListItem = pListItem->pNextItem )
    {
    }

    if ( pListItem != NULL )
    {
        if ( pListItem == self->pHead )
        {
            self->pHead = pListItem->pNextItem;
        }
        else
        {
            pPreviousListItem->pNextItem = pListItem->pNextItem;
        }

        if ( self->NItems > 0 )
        {
            self->NItems--;
        }

        result = TRUE;
    }
    else
    {
        result = FALSE;
    }
    taskEXIT_CRITICAL();

    if ( pListItem != NULL )
    {
        BufferManagerFreeBuffer( pListItem );
    }

    return result;
}

static BOOL LinkedList_Insert( LinkedList_t* self, uint32_t index, void* pItem )
{
    BOOL result = FALSE;
    uint32_t i;
    LinkedListItem_t* pListItem;
    LinkedListItem_t* pNewListItem = BufferManagerGetBuffer( sizeof( LinkedListItem_t ) );
    LinkedListItem_t* pPreviousListItem;

    if ( pNewListItem != NULL && self->NItems < self->MaxItems )
    {
        pNewListItem->pItem = pItem;

        taskENTER_CRITICAL();
        for ( i = 0, pPreviousListItem = NULL, pListItem = self->pHead; pListItem != NULL && i < index && i < self->NItems; i++, pPreviousListItem = pListItem, pListItem = pListItem->pNextItem )
        {
        }
        pNewListItem->pNextItem = pListItem;

        if ( self->pHead == pListItem )
        {
            self->pHead = pNewListItem;
        }
        else
        {
            pPreviousListItem->pNextItem = pNewListItem;
        }

        self->NItems++;
        taskEXIT_CRITICAL();

        result = TRUE;
    }
    else
    {
        if ( pNewListItem != NULL )
        {
            BufferManagerFreeBuffer( pNewListItem );
        }
        result = FALSE;
    }

    return result;
}

static void* LinkedList_Peek( LinkedList_t* self, uint32_t Index )
{
    void* result = NULL;
    LinkedListItem_t* pListItem;
    uint32_t i;

    for ( i = 0, pListItem = self->pHead; pListItem != NULL && i < Index; i++, pListItem = pListItem->pNextItem )
    {
        result = ( void* )pListItem;
    }

    if ( pListItem != NULL )
    {
        result = pListItem->pItem;
    }
    else
    {
        result = NULL;
    }

    return result;
}
