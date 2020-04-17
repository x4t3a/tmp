#pragma once

#include <stddef.h> // size_t
#include <stdbool.h> // bool

/**
 * @brief 
 * 
 */
typedef struct __BufferView
{
    unsigned char* view;
    size_t         size;
} BufferView;

/**
 * @brief Checks whenever the pointer to the view and its contents are valid or not.
 * 
 * @param view is a pointer to the view.
 * @return true if the view or its contents are invalid.
 * @return false if all is good.
 */
bool checkBufferView(BufferView* view);