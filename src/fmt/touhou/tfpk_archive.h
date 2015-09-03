#pragma once

#include "fmt/archive.h"

namespace au {
namespace fmt {
namespace touhou {

    class TfpkArchive final : public Archive
    {
    public:
        TfpkArchive();
        ~TfpkArchive();
        void register_cli_options(ArgParser &) const override;
        void parse_cli_options(const ArgParser &) override;
    protected:
        bool is_recognized_internal(File &) const override;
        void unpack_internal(File &, FileSaver &) const override;
    private:
        struct Priv;
        std::unique_ptr<Priv> p;
    };

} } }
