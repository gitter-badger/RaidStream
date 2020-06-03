#ifndef RAIDSTREAM_RAIDFILE_HPP
#define RAIDSTREAM_RAIDFILE_HPP
#include <string>
#include <fstream>


namespace RaidStream {
    class RaidFile{
    public:
        enum FileType {
            DATA,
            PARITY_XOR,
            PARITY_RS,
            PARITY_EXPERIMENTAL
        };
        inline RaidFile(const RaidFile &file) : _filename{file._filename}, _type{file._type} {}
        RaidFile& operator=( const RaidFile& ) = default;

        inline RaidFile(const char* filename,
                        FileType type,
                        std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::app
        ) : _type{type}, _filename{std::string(filename)} {}
        inline FileType Type() const {
            return _type;
        }
        inline std::string Filename() const {
            return _filename;
        }
    protected:
        const std::string _filename;
        const FileType _type;
    };
}


#endif //RAIDSTREAM_RAIDFILE_HPP
