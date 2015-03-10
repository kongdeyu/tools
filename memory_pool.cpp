
#include <cstddef>
#include <cstdlib>

const size_t ALIGN = 8;
const size_t MAX_BYTES = 128;
const size_t NFREELISTS = MAX_BYTES / ALIGN;

class my_alloc
{
public:
    static void* allocate(size_t n);
    static void deallocate(void* p, size_t n);
    static void* reallocate(void* p, size_t old_sz, size_t new_sz);

private:
    union obj
    {
        union obj* free_list_link;
        char client_data[1];
    };

    static size_t _round_up(size_t bytes);
    static size_t _free_list_index(size_t bytes);
    static void* _refill(size_t n);
    static char* _chunk_alloc(size_t size, int32_t& nobjs);

    static union obj* volatile _free_list[NFREELISTS];
    static char* _start;
    static char* _end;
    static size_t _heap_size;
};

union my_alloc::obj* volatile my_alloc::_free_list[NFREELISTS] =
    {NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL};

char* my_alloc::_start = 0;
char* my_alloc::_end = 0;
size_t my_alloc::_heap_size = 0;

void* my_alloc::allocate(size_t n)
{
    if(MAX_BYTES < n)
    {
        return malloc(n);
    }
    size_t index = _free_list_index(n);
    if(NULL == _free_list[index]->free_list_link)
    {
        return _refill(_round_up(n));
    }
    union obj* volatile res = _free_list[index];
    _free_list[index] = res->free_list_link;
    return res;
}

void my_alloc::deallocate(void* p, size_t n)
{
    if(MAX_BYTES < n)
    {
        return free(p);
    }
    union obj* temp = (union obj*)p;
    size_t index = _free_list_index(n);
    union obj* volatile free_list = _free_list[index];
    temp->free_list_link = free_list;
    _free_list[index] = temp;
}

void* my_alloc::reallocate(void* p, size_t old_sz, size_t new_sz)
{
    deallocate(p, old_sz);
    return allocate(new_sz);
}

size_t my_alloc::_round_up(size_t bytes)
{
    return (bytes + ALIGN - 1) & ~(ALIGN - 1);
}

size_t my_alloc::_free_list_index(size_t bytes)
{
    return (bytes + ALIGN - 1) / ALIGN - 1;
}

void* my_alloc::_refill(size_t n)
{
    int32_t nobjs = 20;
    char* chunk = _chunk_alloc(n, nobjs);
    if(1 == nobjs)
    {
        return chunk;
    }

    void* res = chunk;
    union obj* volatile curr = _free_list[n];
    curr->free_list_link = (union obj*)chunk + n;
    for(int32_t i = 1; i < nobjs; i++)
    {
        union obj* next = curr->free_list_link + n;
        curr->free_list_link = next;
    }
    curr->free_list_link = NULL;
    return res;
}

char* my_alloc::_chunk_alloc(size_t size, int32_t& nobjs)
{
    char* res;
    size_t bytes_left = _end - _start;
    size_t bytes_to_get = size * nobjs;
    if(bytes_left >= bytes_to_get)
    {
        res = _start;
        _start += bytes_to_get;
        return res;
    }
    if(bytes_left >= size)
    {
        nobjs = bytes_left / size;
        res = _start;
        _start += nobjs * size;
        return res;
    }
    if(bytes_left > 0)
    {
        size_t index = _free_list_index(bytes_left);
        union obj* volatile free_list = _free_list[index];
        union obj* temp = (union obj*)_start;
        temp->free_list_link = free_list;
        _free_list[index] = temp;
    }
    size_t bytes_to_realloc = 2 * bytes_to_get + _round_up(_heap_size >> 4);
    _start = (char*)malloc(bytes_to_realloc);
    if(_start == NULL)
    {
        for(size_t i = size; i < MAX_BYTES; i += ALIGN)
        {
            union obj* volatile free_list = _free_list[i];
            if(NULL != free_list)
            {
                _start = (char*)free_list;
                _end = _start + i;
                free_list = free_list->free_list_link;
                return _chunk_alloc(size, nobjs);
            }
        }
        _end = 0;
        _start = (char*)malloc(bytes_to_realloc);
    }
    _end = _start + bytes_to_realloc;
    _heap_size += bytes_to_realloc;
    return _chunk_alloc(size, nobjs);
}

int32_t main()
{
    return 0;
}
