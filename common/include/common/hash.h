#pragma once

#include <common/buffer_view.h>

typedef unsigned char Hash[16];

bool
calcMD5Hash(Hash hash, BufferView* view);