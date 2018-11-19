#include "mm.h"

#define ALIGNMENT 16
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~(ALIGNMENT-1))

#define CHUNK_SIZE (1 << 14)
#define CHUNK_ALIGN(size) (((size)+(CHUNK_SIZE-1)) & ~(CHUNK_SIZE-1))

#define OVERHEAD (sizeof(block_header)+sizeof(block_footer))
#define HDRP(bp) ((char *)(bp) - sizeof(block_header))

#define GET_SIZE(p)  ((block_header *)(p))->size
#define GET_ALLOC(p) ((block_header *)(p))->allocated

#define NEXT_BLKP(bp) ((char *)(bp) + GET_SIZE(HDRP(bp)))
#define PREV_BLKP(bp) ((char *)(bp)-GET_SIZE((char *)(bp)-OVERHEAD))
#define FTRP(bp) ((char *)(bp)+GET_SIZE(HDRP(bp))-OVERHEAD)

void *coalesce(void *bp);

typedef struct {
  size_t size;
  char   allocated;
} block_header;

typedef struct {
 size_t size;
 int filler;
} block_footer;

void *first_bp;

void mm_init(void *heap, size_t heap_size)
{
  void *bp;

  bp = heap + sizeof(block_header);

  GET_SIZE(HDRP(bp)) = heap_size;
  GET_ALLOC(HDRP(bp)) = 0;

  first_bp = bp;
  mm_malloc(0); /* never freed */
}

void set_allocated(void *bp, size_t size) {
  size_t extra_size = GET_SIZE(HDRP(bp)) - size;
  if (extra_size > ALIGN(1 + OVERHEAD)) {
    GET_SIZE(HDRP(bp)) = size;
    GET_SIZE(FTRP(bp)) = size;
    GET_SIZE(HDRP(NEXT_BLKP(bp))) = extra_size;
    GET_SIZE(FTRP(NEXT_BLKP(bp))) = extra_size;

    GET_ALLOC(HDRP(NEXT_BLKP(bp))) = 0;
  }
  GET_ALLOC(HDRP(bp)) = 1;
}

void *mm_malloc(size_t size)
{
  int new_size = ALIGN(size + OVERHEAD);
  void *bp = first_bp;
  while (GET_SIZE(HDRP(bp)) != 0) {
    if (!GET_ALLOC(HDRP(bp)) && (GET_SIZE(HDRP(bp)) >= new_size)) {
      set_allocated(bp, new_size);
      return bp;
    }
    bp = NEXT_BLKP(bp);
  }
  // extend(new_size);
  set_allocated(bp, new_size);
  return bp;
}

void mm_free(void *bp)
{
  GET_ALLOC(HDRP(bp)) = 0;
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
    GET_SIZE(HDRP(bp)) = size;
    GET_SIZE(FTRP(bp)) = size;
  }
  else if (!prev_alloc && next_alloc) { /* Case 3 FFM*/
    size += GET_SIZE(HDRP(PREV_BLKP(bp)));
    GET_SIZE(FTRP(bp)) = size;
    GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  }
  else { /* Case 4  FFF*/
    size += (GET_SIZE(HDRP(PREV_BLKP(bp)))
    + GET_SIZE(HDRP(NEXT_BLKP(bp))));
    GET_SIZE(HDRP(PREV_BLKP(bp))) = size;
    GET_SIZE(FTRP(NEXT_BLKP(bp))) = size;
    bp = PREV_BLKP(bp);
  }

  return bp;
}
