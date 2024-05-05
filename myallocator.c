#include <stdio.h>
#include <stddef.h>

char mymemory[8000];

struct block {
    size_t size;
    int free;
    struct block *next;
};

typedef struct block block;

block *list = (void *)mymemory;

void initmemory() {
    list->size = 8000 - sizeof(block);
    list->free = 1;
    list->next = NULL;
    printf("--Memory Initialization Completed--\n");
    printf("Initial size is: %zu\n\n", list->size);
}

void split(block *slotToBeSplitted, size_t size) {
    block *newSlot = (block *)((char *)slotToBeSplitted + size + sizeof(block));
    newSlot->size = slotToBeSplitted->size - size - sizeof(block);
    newSlot->free = 1;
    newSlot->next = slotToBeSplitted->next;
    slotToBeSplitted->size = size;
    slotToBeSplitted->free = 0;
    slotToBeSplitted->next = newSlot;
    printf("Allocate block with splitting for %zu bytes\n", size);
}

void *NewMalloc(size_t allocSize) {
    block *curr = list;
    block *best_fit = NULL;
    size_t min_diff = (size_t) -1;
    size_t largest_free_block = 0;

    while (curr) {
        if (curr->free) {
            largest_free_block = curr->size > largest_free_block ? curr->size : largest_free_block;
            if (curr->size >= allocSize && (curr->size - allocSize < min_diff)) {
                min_diff = curr->size - allocSize;
                best_fit = curr;
            }
        }
        curr = curr->next;
    }

    if (best_fit) {
        size_t old_size = largest_free_block;
        if (best_fit->size > allocSize + sizeof(block)) {
            split(best_fit, allocSize);
            largest_free_block = old_size - allocSize - sizeof(block);
        } else {
            best_fit->free = 0;
            largest_free_block -= best_fit->size;
        }
        printf("curr size is: %zu\n", largest_free_block);
        return (void *)((char *)best_fit + sizeof(block));
    } else {
        printf("No sufficient memory to allocate %zu bytes\n", allocSize);
        return NULL;
    }
}

void coalesce() {
    block *curr = list;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += curr->next->size + sizeof(block);
            curr->next = curr->next->next;
        } else {
            curr = curr->next;  // Move to the next block if no coalescing is needed
        }
    }
}

void NewFree(void *ptr) {
    if (ptr) {
        block *b = (block *)((char *)ptr - sizeof(block));
        b->free = 1;
        coalesce();
        printf("Block freed.\n");
    }
}

int main() {
    initmemory();

    int *p = (int *)NewMalloc(800);
    printf("\n");

    char *q = (char *)NewMalloc(250 * sizeof(char));
    printf("\n");

    int *r = (int *)NewMalloc(1000 * sizeof(int));
    printf("\n");

    NewFree(p);
    printf("\n");

    char *w = (char *)NewMalloc(1000);
    printf("\n");

    NewFree(r);
    printf("\n");

    int *k = (int *)NewMalloc(1500 * sizeof(int));
    printf("\n");

    return 0;
}

