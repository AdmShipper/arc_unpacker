#pragma once

#include "types.h"

namespace au {
namespace fmt {
namespace leaf {
namespace common {

    bstr custom_lzss_decompress(const bstr &input, size_t output_size);

} } } }