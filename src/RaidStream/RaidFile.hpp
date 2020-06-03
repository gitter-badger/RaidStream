#ifndef RAIDSTREAM_RAIDFILE_HPP
#define RAIDSTREAM_RAIDFILE_HPP
#include <string>
#include <fstream>
#include <map>
#include "RaidStream/RaidFileBlock.hpp"

namespace RaidStream {
    class RaidFile {
    public:
        typedef std::map<RaidStream::RaidFileBlock::block_pos_t, RaidStream::RaidFileBlock> raid_block_map;

        enum FileStatus {
            NEW,
            UNVERIFIED,
            CONSISTENT,
            DEGRADED,
            DEGRADED_REBUILDING,
            OFFLINE,
            OFFLINE_REBUILDING
        };

        enum FileType {
            DATA,
            PARITY_XOR,
            PARITY_RS,
            PARITY_EXPERIMENTAL,
            SPARE
        };

        inline RaidFile(uint16_t index, const char* filename,
                        FileType type,
                        std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::ate | std::ios_base::binary
                        ) :
                            _fileType{type},
                            _fileName{std::string(filename)},
                            _arrayIndex{index},
                            _fileMode{mode},
                            _fileStream{new std::ofstream(filename, mode)}
            {}

        inline FileType Type() const {
            return _fileType;
        }

        inline std::string FileName() const {
            return _fileName;
        }

        inline uint64_t SparseSize() {
            // TODO: get the actual size of the file on disk, taking into account sparsefile compression
            return _actualSize;
        }

        inline uint64_t Size() {
            return _actualSize;
        }

        inline void Size(uint64_t size) {
            _actualSize = size;
        }

        inline uint64_t VirtualSize() {
            return _virtualSize;
        }

        inline void VirtualSize(uint64_t size) {
            if (size > _actualSize) {
                throw std::invalid_argument("Virtual Size must be <= Size");
            }
            _virtualSize = size;
        }

        inline FileStatus Status() {
            return _fileStatus;
        };

        inline bool Consistent() {
            return (_fileStatus == FileStatus::CONSISTENT);
        }

        inline bool Rebuilding() {
            return (
                (_fileStatus == FileStatus::DEGRADED_REBUILDING) ||
                (_fileStatus == FileStatus::OFFLINE_REBUILDING)
            );
        }

        inline unsigned int Mode() {
            return _fileMode;
        }

        inline void Mode(unsigned int mode) {
            _fileMode = mode;
        }

        inline void FlushBlock(RaidStream::RaidFileBlock::block_pos_t id, bool force = false, bool keepMemory = true) {
            raid_block_map::iterator it = _loadedBlocks.find(id);
            if (it == _loadedBlocks.end()) {
                return;
            }
            if (!keepMemory) {
                it->second.ReleaseMemory(force);
                _loadedBlocks.erase(it);
            }
        }

    protected:
        // uuid _id;
        const uint16_t _arrayIndex;
        const std::string _fileName;
        const FileType _fileType;
        const std::ofstream* _fileStream;
        size_t _virtualSize = 0; // may be < actual size
        size_t _actualSize = 0;
        FileStatus _fileStatus = FileStatus::NEW;
        RaidFileBlock::block_pos_t _numBlocks;
        RaidFileBlock::block_pos_t _numBlocksVirtual;
        raid_block_map _loadedBlocks;
        unsigned int _fileMode;

    };
}

#endif //RAIDSTREAM_RAIDFILE_HPP
