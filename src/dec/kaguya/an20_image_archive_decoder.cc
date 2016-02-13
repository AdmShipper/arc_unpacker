#include "dec/kaguya/an20_image_archive_decoder.h"
#include "algo/range.h"
#include "enc/png/png_image_encoder.h"
#include "err.h"

using namespace au;
using namespace au::dec::kaguya;

static const bstr magic = "AN20"_b;

namespace
{
    struct CustomArchiveEntry final : dec::ArchiveEntry
    {
        size_t offset;
        size_t x, y;
        size_t width, height;
        size_t channels;
    };
}

algo::NamingStrategy An20ImageArchiveDecoder::naming_strategy() const
{
    return algo::NamingStrategy::Sibling;
}

bool An20ImageArchiveDecoder::is_recognized_impl(io::File &input_file) const
{
    return input_file.stream.seek(0).read(magic.size()) == magic;
}

std::unique_ptr<dec::ArchiveMeta> An20ImageArchiveDecoder::read_meta_impl(
    const Logger &logger, io::File &input_file) const
{
    input_file.stream.seek(magic.size());
    const auto unk_count = input_file.stream.read_le<u16>();
    input_file.stream.skip(2);
    for (const auto i : algo::range(unk_count))
    {
        const auto control = input_file.stream.read<u8>();
        if (control == 0) continue;
        else if (control == 1) input_file.stream.skip(8);
        else if (control == 2) input_file.stream.skip(4);
        else if (control == 3) input_file.stream.skip(4);
        else if (control == 4) input_file.stream.skip(4);
        else if (control == 5) input_file.stream.skip(4);
        else throw err::NotSupportedError("Unsupported control");
    }

    input_file.stream.skip(2);
    auto meta = std::make_unique<dec::ArchiveMeta>();
    const auto file_count = input_file.stream.read_le<u16>();
    if (!file_count)
        return meta;
    const auto base_x = input_file.stream.read_le<u32>();
    const auto base_y = input_file.stream.read_le<u32>();
    const auto base_width = input_file.stream.read_le<u32>();
    const auto base_height = input_file.stream.read_le<u32>();
    for (const auto i : algo::range(file_count))
    {
        auto entry = std::make_unique<CustomArchiveEntry>();
        entry->x = input_file.stream.read_le<u32>();
        entry->y = input_file.stream.read_le<u32>();
        entry->width = input_file.stream.read_le<u32>();
        entry->height = input_file.stream.read_le<u32>();
        entry->channels = input_file.stream.read_le<u32>();
        entry->offset = input_file.stream.pos();
        input_file.stream.skip(entry->channels * entry->width * entry->height);
        meta->entries.push_back(std::move(entry));
    }
    return meta;
}

std::unique_ptr<io::File> An20ImageArchiveDecoder::read_file_impl(
    const Logger &logger,
    io::File &input_file,
    const dec::ArchiveMeta &m,
    const dec::ArchiveEntry &e) const
{
    const auto entry = static_cast<const CustomArchiveEntry*>(&e);
    res::Image image(
        entry->width,
        entry->height,
        input_file.stream.seek(entry->offset),
        entry->channels == 3
            ? res::PixelFormat::BGR888
            : res::PixelFormat::BGRA8888);
    image.flip_vertically();
    return enc::png::PngImageEncoder().encode(logger, image, entry->path);
}

static auto _ = dec::register_decoder<An20ImageArchiveDecoder>("kaguya/an20");
