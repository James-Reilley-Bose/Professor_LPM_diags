/*
    File    :   LinkedList.h
    Title   :
    Author  :   jc45540
    Created :   02/04/2016
    Language:   C
    Copyright:  (C) 2015 Bose Corporation, Framingham, MA

    Description:    Object-oriented Linked List implementation

===============================================================================
*/

#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

#include <stdint.h>
#include <stdlib.h>

#define LINKEDLIST_MAX_SIZE 10

// Simple linked lis
#pragma pack(1)
typedef struct tag_LinkedListItem
{
    void* pItem;
    struct tag_LinkedListItem* pNextItem;
} LinkedListItem_t;

typedef struct tag_LinkedList
{
    // Attributes
    LinkedListItem_t* pHead;
    LinkedListItem_t* pTail;
    uint32_t NItems;
    uint32_t MaxItems;

    // Methods
    void ( *Dtor )( struct tag_LinkedList* self );
    uint8_t ( *Push )( struct tag_LinkedList* self, void* pValue );
    void* ( *Pop )( struct tag_LinkedList* self );
    uint8_t ( *Full )( struct tag_LinkedList* self );
    uint8_t ( *Empty )( struct tag_LinkedList* self );
    uint8_t ( *Delete )( struct tag_LinkedList* self, uint32_t index );
    uint8_t ( *Insert )( struct tag_LinkedList* self, uint32_t index, void* pItem );
    void* ( *Peek)( struct tag_LinkedList* self, uint32_t Index );
    uint8_t ( *SetMaxSize )( struct tag_LinkedList* self, uint32_t Size );
    uint32_t ( *GetMaxSize )( struct tag_LinkedList* self );
    uint32_t ( *GetSize )( struct tag_LinkedList* self );
} LinkedList_t;
#pragma pack()

void LinkedList_Ctor( struct tag_LinkedList* self );

#endif // _LINKEDLIST_H_
