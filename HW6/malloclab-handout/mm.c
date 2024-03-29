#include "mm.h"

/*
 * CS4400
 * HW6
 * Author: Taku Sakikawa
 */

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#define CHUNK_SIZE (1 << 14)
#define CHUNK_ALIGN(size) (((size)+(CHUNK_SIZE-1)) & ~(CHUNK_SIZE-1))

// #define OVERHEAD (sizeof(block_header)+sizeof(block_footer))
#define OVERHEAD (sizeof(block_header)+sizeof(block_footer))
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))

// #define GET_SIZE(p)  ((block_header *)(p))->size
// #define GET_ALLOC(p) ((block_header *)(p))->allocated

#define GET(p) (*(size_t *)(p))
#define GET_ALLOC(p) (GET(p) & 0x1)
#define GET_SIZE(p) (GET(p) & ~0xF)
#define PUT(p, val) (*(size_t *)(p) = (val))
#define PACK(size, alloc) ((size) | (alloc))

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))
#define FTRP(bp) ((char *)(bp)+GET_SIZE(HDRP(bp))-OVERHEAD)

void *coalesce(void *bp);

typedef size_t block_header;
typedef size_t block_footer;

// typedef struct {
//   size_t size;
//   char   allocated;
// } block_header;
//
// typedef struct {
//  size_t size;
//  int filler;
// } block_footer;

void *first_bp;

void mm_init(void *heap, size_t heap_size)
{
  // printf("int:   %d\n",sizeof(int) );
  // printf("size_t:   %d\n",sizeof(size_t) );
  //
  //
  // printf("OVERHEAD:   %d\n", OVERHEAD);
  // // printf("OVERHEAD:   %d\n", sizeof(char));
  // printf("OVERHEAD:   %d\n", sizeof(block_header));

  void *bp;
  bp = heap + sizeof(block_header)+8;
  first_bp = bp;
  // printf("%p\n", first_bp);



  PUT(HDRP(bp), PACK(heap_size - sizeof(block_footer), 0));

  // GET_SIZE(HDRP(bp)) = heap_size - sizeof(block_footer);
  // GET_ALLOC(HDRP(bp)) = 0;

  mm_malloc(0); /* never freed */

  first_bp = NEXT_BLKP(bp);

  // printf("%p\n", first_bp);

  PUT(HDRP(NEXT_BLKP(first_bp)), PACK(0, 0));
  // GET_SIZE(HDRP(NEXT_BLKP(first_bp))) = 0;
  // GET_ALLOC(HDRP(NEXT_BLKP(first_bp))) = 1;

  // printf("first_bp: %lu\n", GET_SIZE(HDRP((first_bp))));
  // printf("first_bp: %d\n", GET_ALLOC(HDRP((first_bp))));
  //
  // printf("last: %lu\n", GET_SIZE(HDRP((NEXT_BLKP(first_bp)))));
  // printf("last: %d\n", GET_ALLOC(HDRP(NEXT_BLKP(first_bp))));
}

void set_allocated(void *bp, size_t size) {
  size_t extra_size = GET_SIZE(HDRP(bp)) - size;
  if (extra_size > ALIGN(1 + OVERHEAD)) {

    // GET_SIZE(HDRP(bp)) = size;
    // GET_SIZE(FTRP(bp)) = size;
    PUT(HDRP(bp), PACK(size, 1));
    PUT(FTRP(bp), PACK(size, 1));


    // GET_SIZE(HDRP(NEXT_BLKP(bp))) = extra_size;
    // GET_SIZE(FTRP(NEXT_BLKP(bp))) = extra_size;
    PUT(HDRP(NEXT_BLKP(bp)), PACK(extra_size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(extra_size, 0));

    // GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 0;
  }
  PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 1));
  // GET_ALLOC(HDRP(bp)) = 1;
}

void *mm_malloc(size_t size)
{
  int new_size = ALIGN(size + OVERHEAD);
  void *bp = first_bp;

  while (GET_SIZE(HDRP(bp)) != 0) {
    // printf("GET_SIZE(HDRP(bp)):  %lu\n", GET_SIZE(HDRP(bp)) );
    // printf("new_size:   %d\n", new_size );


    if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= new_size)) {
      set_allocated(bp, new_size);
      return bp;
    }
    // printf("%s\n","a" );
    bp = NEXT_BLKP(bp);
    // printf("%s\n","b" );

  }
  // extend(new_size);
  // set_allocated(bp, new_size);
  return NULL;
}

void mm_free(void *bp)
{


  // GET_ALLOC(HDRP(bp)) = 0;
  PUT(HDRP(bp), PACK(GET_SIZE(HDRP(bp)), 0));
  coalesce(bp);
}


void *coalesce(void *bp) {
  size_t prev_alloc = GET_ALLOC(HDRP(PREV_BLKP(bp)));
  size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
  size_t size = GET_SIZE(HDRP(bp));

  if (prev_alloc && next_alloc) { /* Case 1  MFM*/
  /* nothing to do */
  }
  else if (prev_alloc && !next_alloc) { /* Case 2  MFF*/
    size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));

    // GET_SIZE(HDRP(bp)) = size;
    // GET_SIZE(FTRP(bp)) = size;
  }
  else if (!prev_alloc && next_alloc) { /* Case 3 FFM*/
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    // GET_SIZE(FTRP(bp)) = size;
    // GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  }
  else { /* Case 4  FFF*/
    size += (GET_SIZE(HDRP(PREV_BLKP(bp)))
    + GET_SIZE(HDRP(NEXT_BLKP(bp))));

    PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
    PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
    // GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    // GET_SIZE(FTRP(NEXT_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  }

  return bp;
}
