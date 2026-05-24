#ifndef LILARENA__H_
#define LILARENA__H_

#include <stddef.h>
#include <stdint.h>
#include <assert.h>

/** @file
 * Arena allocator.
 *
 *  An LArena allocates memory from a provided buffer.
 *
 * The backing buffer should be suitable aligned to the types
 * that will be allocated from it. Use .e.g
 * @code
 * _Alignas(16) uint8_t mem[1024];
 * @endcode
 * as the backing buffer.
 *
 * Normally the arena is used to allocate memory from, reset and reuse the arena.
 *
 * Temporary allocations can be obtained and released back to he arena via
 * larena_tmp_begin()/larena_tmp_end() pair of calls, releassing memory from all
 * lalloc() calls between larena_tmp_begin() and larena_tmp_end()the back to the arena.
 *
 */
typedef struct LArena LArena;
struct LArena {
    uint8_t *start;  // start of user buffer
    uint8_t *curr;   // next alloc point
    uint8_t *end;    // one past end of user buffer
    int tmp_allocs;  // level of larena_tmp_begin stacks
};

typedef struct LArenaTemp LArenaTemp;
struct LArenaTemp {
    uint8_t *alloc_point;
    LArena *arena;
};
/** Initialize a LArena with predefined backing array.
 *
 * @data must be an array type
 * Use as
 * @code
 * _Alignas(16) uint8_t v[128];
 * LArena a = LARENA_STATIC_INITIALIZER(v);
 * @endcode
 */
#define LARENA_STATIC_INITIALIZER(data) LARENA_INITIALIZER((data), sizeof(data))

/** Initializer for LArena with predefined memory
 * Use as
 * @code
 * uint8_t v[128];
 * LArena a = LARENA_INITIALIZER(v, sizeof v);
 * @endcode
 * or
 * @code
 * uint8_t *m = malloc(128);
 * LArena a = LARENA_INITIALIZER(m, 128);
 * @endcode
 */
#define LARENA_INITIALIZER(data, sz) {.start = (data), .curr = (data), .end = (data) + sz, .tmp_allocs = 0}

/** Initialize a LArena with an array as the backing memory to allocate from.
 * The macro can be used at local scope to allocate memory automatic on the stack,
 * or at global scope where the backing array will have static storage duration
 *
 * Use as:
 * @code
 * LARENA_DEFINE(my_allocator, 1024, 16)
 * struct foo *foo = larena_alloc(&my_allocator);
 * if (foo == NULL) {
 *     // out of memory
 * }
 * @endcode
 *
 * @param var_name Variable name for the allocator
 * @param size_  byte size of the backing array
 * @param alignment_  alignment of the backing array
 */
#define LARENA_DEFINE(var_name, size_, alignment_)  \
    struct {                                        \
        _Alignas(alignment_) uint8_t memory[size_]; \
    } var_name##_arena_memory;                      \
    LArena var_name = LARENA_STATIC_INITIALIZER(var_name##_arena_memory.memory)

/** Allocate space for @type_ , returned memory is aligned to requirements
 * of @type.
 *
 * Use as:
 * @code
 * LARENA_DEFINE(my_allocator, 1024)
 * struct foo *foo = LARENA_ALLOC_TYPE(&my_allocator, struct foo);
 * // Array types can be used too:
 * struct Foo *str = LARENA_ALLOC_TYPE(&my_allocator, struct Foo[32]);
 * // For arrays with runtime determined size, use:
 * LARENA_ALLOC_ARRAY(&my_allocator, struct Foo, cnt_foos);
 * @endcode
 */
#define LARENA_ALLOC_TYPE(allocator, type_) larena_alloc_aligned((allocator), sizeof(type_), _Alignof(type_))

#define LARENA_ALLOC_ARRAY(allocator, type_, nr_elements) larena_alloc_aligned((allocator), nr_elements * sizeof(type_), _Alignof(type_))

// Arena size in bytes
static inline size_t larena_size(const LArena *arena)
{
    return (size_t)(arena->end - arena->start);
}

// Used bytes of the arena
static inline size_t larena_used(const LArena *arena)
{
    return (size_t)(arena->curr - arena->start);
}

// Available unused bytes in the arena
static inline size_t larena_available(const LArena *arena)
{
    return larena_size(arena) - larena_used(arena);
}

// Initialize an LArena to allocate from @mem which is @sz bytes
static inline void larena_init(LArena *arena, void *memory, size_t sz)
{
    arena->start = arena->curr = (uint8_t *)memory;
    arena->end = arena->start + sz;
}

// begin temporary allocation. All allocations afterwards will be released in larena_tmp_end
static inline LArenaTemp larena_tmp_begin(LArena *arena)
{
    LArenaTemp state = {.alloc_point = arena->curr, .arena = arena};
    arena->tmp_allocs++;
    return state;
}

// end temporary allocations, releasing all allocations since the most recent larena_tmp_begin
static inline void larena_tmp_end(LArenaTemp *state)
{
    LArena *arena = state->arena;
    assert(arena->tmp_allocs > 0);
    arena->curr = state->alloc_point;
    arena->tmp_allocs--;
}

// Allocate @sz bytes from the LArena. Returns NULL if not enough space
static inline void *larena_alloc(LArena *arena, size_t sz)
{
    if (sz > larena_available(arena)) {
        return NULL;
    }
    void *start = arena->curr;
    arena->curr += sz;
    return start;
}

// Allocate @sz bytes from the LArena, returned pointer is aligned to @align
// Returns NULL if not enough space
[[gnu::alloc_align(3)]]
static inline void *larena_alloc_aligned(LArena *arena, size_t sz, unsigned int align)
{
    assert(align != 0);
    assert((align & (align - 1)) == 0);  // power of 2

    uintptr_t curr = (uintptr_t)arena->curr;
    uintptr_t aligned = (curr + (align - (uintptr_t)1)) & ~(align - (uintptr_t)1);
    uint8_t *start = (uint8_t *)aligned;

    if (sz > (larena_available(arena))) {
        return NULL;
    }

    arena->curr = start + sz;

    return start;
}

// reset arena, making all memory available for allocation
// also invalidates any LArenaTemp instances.
static inline void larena_reset(LArena *arena)
{
    arena->curr = arena->start;
}

#endif
