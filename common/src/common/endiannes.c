#include <common/endiannes.h>

#include <stdint.h>

static void 
reverseEndian(void* data, size_t data_size)
{
    char* arr_conv;
    char* arr;
    char tmp;

    arr = arr_conv = (char*) data;
    arr += (data_size - 1);

    data_size /= 2;

    while (data_size)
    {
        tmp = *arr_conv;
        *arr_conv = *arr;
        *arr = tmp;

        data_size--;
        arr_conv++;
        arr--;
    }
}

#define IS_LIL_ENDIAN (*(unsigned char*) &(uint16_t){1})

void
htond(void* data, size_t data_size)
{
    if (IS_LIL_ENDIAN)
    {
        reverseEndian(data, data_size);
    }
}

void
ntohd(void* data, size_t data_size)
{
    if (IS_LIL_ENDIAN)
    {
        reverseEndian(data, data_size);
    }
}

#undef IS_LIL_ENDIAN