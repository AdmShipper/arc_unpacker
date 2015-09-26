#include "fmt/qlie/abmp7_archive_decoder.h"
#include "util/encoding.h"

using namespace au;
using namespace au::fmt::qlie;

static const bstr magic = "ABMP7"_b;

static void read_first_file(io::IO &arc_io, FileSaver &saver)
{
    size_t size = arc_io.read_u32_le();
    std::unique_ptr<File> subfile(new File);
    subfile->io.write_from_io(arc_io, size);
    subfile->name = "base.dat";
    subfile->guess_extension();
    saver.save(std::move(subfile));
}

static void read_next_file(io::IO &arc_io, FileSaver &saver)
{
    bstr encoded_name = arc_io.read(arc_io.read_u8());
    arc_io.skip(31 - encoded_name.size());
    bstr name = util::sjis_to_utf8(encoded_name);
    size_t size = arc_io.read_u32_le();
    std::unique_ptr<File> subfile(new File);
    subfile->io.write_from_io(arc_io, size);
    subfile->name = (name == ""_b ? "unknown" : name.str()) + ".dat";
    subfile->guess_extension();
    saver.save(std::move(subfile));
}

bool Abmp7ArchiveDecoder::is_recognized_internal(File &arc_file) const
{
    return arc_file.io.read(magic.size()) == magic;
}

void Abmp7ArchiveDecoder::unpack_internal(
    File &arc_file, FileSaver &saver) const
{
    arc_file.io.seek(12);
    arc_file.io.skip(arc_file.io.read_u32_le());

    read_first_file(arc_file.io, saver);
    while (arc_file.io.tell() < arc_file.io.size())
        read_next_file(arc_file.io, saver);
}

static auto dummy = fmt::Registry::add<Abmp7ArchiveDecoder>("qlie/abmp7");