#include <common/buffer_view.h>

bool
checkBufferView(BufferView* view)
{
    return !(view && view->view && view->size);
}