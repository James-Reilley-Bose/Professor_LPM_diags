#ifndef ETAPHEAP_H
#define ETAPHEAP_H

/**
 * @DOCETAP
 *
 * COMMAND:   heap
 *
 * ALIAS:     he
 *
 * DESCRIP:   Returns the amount of free space in the heap
 *
 * PARAM:     n/a
 *
 * REPLY:     n/a
 *
 * EXAMPLE:   heap
 *
 * @DOCETAPEND
 */

#define HEAP_HELP_TEXT "* This command returns the size of free space in the heap"

void TAP_Heap(CommandLine_t*);

#endif
