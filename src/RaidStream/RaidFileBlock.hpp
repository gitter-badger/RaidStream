#ifndef RAIDSTREAM_RAIDFILEBLOCK_HPP
#define RAIDSTREAM_RAIDFILEBLOCK_HPP

#define USE_1024 false // Make sure this aligns with your disk!
#define UNITS_KB (USE_1024 ? 1024 : 1000)
#define UNITS_MB (UNITS_KB * UNITS_KB)
#define UNITS_GB (UNITS_MB * UNITS_KB)
#define UNITS_TB (UNITS_GB * UNITS_KB)
#define UNITS_PB (UNITS_TB * UNITS_KB)


#include <exception>
#include "RaidStream/RaidStream.hpp"
#include "RaidStream/RaidFile.hpp"
#include "RaidStream/crc64/crc64.cpp"
#include "RaidStream/crc32/crc32.hpp"


namespace RaidStream {
    class RaidFileBlock {
    public:
        typedef uint32_t block_size_t;
        typedef uint16_t block_pos_t;
        typedef unsigned char block_data_type;

        static const block_size_t BLOCK_SIZE = (4 * UNITS_MB); // Make sure this aligns with your disk!
        RaidFileBlock(uint64_t nativeOffset) :
                _blockID{static_cast<block_pos_t>(nativeOffset / BLOCK_SIZE)} {
            if ((nativeOffset % BLOCK_SIZE) != 0) {
                throw std::invalid_argument("nativeOffset must be at multiple of block size");
            }
        }

        inline bool InMemory() {
            return (_bytes != nullptr);
        }

        inline bool NeedsSync() {
            return _uncommittedWrites;
        }

        inline bool ReleaseMemory(bool force = false) {
            if (!force && NeedsSync()) return false;
            if (_bytes == nullptr) return true;
            delete _bytes;
            _bytes = nullptr;
            return true;
        }

        inline bool ReadBlockFromDisk(bool force = false) {
            if (!force && (InMemory() || NeedsSync())) return false;
            if (_bytes == nullptr) _bytes = new block_data_type[BLOCK_SIZE];
            // TODO: read from disk
            return false;

            _uncommittedWrites = false;
            return true;
        }

        inline bool WriteBlockToDisk(bool force = false, bool keepMemory = true) {
            if (!force && !NeedsSync()) return false;
            // TODO: write to disk
            _uncommittedWrites = false;
            if (!keepMemory) {
                ReleaseMemory();
            }
        }

        inline const block_size_t BlockSize() {
            return BLOCK_SIZE;
        }

        inline const block_size_t BlockID() {
            return _blockID;
        }

        inline block_data_type at(block_size_t offset) {
            if ((offset < 0) || (offset >= BLOCK_SIZE)) {
                throw std::invalid_argument("Invalid offset out of range");
            }
            return Bytes()[offset];
        }

        /** Writes data to memory, forces full block sync if immediateSync, bool to keep data in memory after write **/
        inline bool
        Set(block_size_t blockOffset, const block_data_type *data, size_t len, bool immediateSync = false,
            bool keepInMemory = true) {
            size_t data_it = 0;
            for (size_t it = blockOffset; (it < BLOCK_SIZE) && (data_it < len); it++) {
                _bytes[it] = data[data_it++];
            }
            if (immediateSync) {
                return WriteBlockToDisk(true, keepInMemory);
            }
            return false;
        }

        inline RaidFileBlock::block_data_type *Bytes() {
            if (!InMemory()) {
                if (!ReadBlockFromDisk()) {
                    throw std::invalid_argument("Unable to read disk"); // TODO: error types
                }
            }
            return _bytes;
        }

        inline RaidFileBlock::block_data_type *BytesCopy() {
            block_data_type* ourBytes = Bytes();
            block_data_type* newCopy = new block_data_type [BLOCK_SIZE];
            memcpy(newCopy, ourBytes, BLOCK_SIZE);
            return newCopy;
        }

        inline CRC32::crc crc32() {
            CRC32::crcInit();
            return CRC32::crcFast(Bytes(), BLOCK_SIZE);
        }

        inline CRC64::crc crc64() {
            return CRC64::crc64(0, Bytes(), BLOCK_SIZE);
        }

        inline bool UpdateCRC32() {
            CRC32::crc oldCRC = _lastCRC32;
            _lastCRC32 = crc32();
            return (oldCRC == _lastCRC32);
        }

        inline bool UpdateCRC64() {
            CRC64::crc oldCRC = _lastCRC64;
            _lastCRC64 = crc64();
            return (oldCRC == _lastCRC64);
        }

        inline block_data_type* XorBlock(RaidFileBlock block) {
            block_data_type* ourBytes = Bytes();
            block_data_type* theirBytes = block.Bytes();
            block_data_type* xorBuf = new block_data_type [BLOCK_SIZE];
            for (block_size_t it = 0; it<BLOCK_SIZE; ++it) {
                xorBuf[it] = ourBytes[it] ^ theirBytes[it];
            }
            return xorBuf;
        }

        inline block_data_type* XorBlock(std::vector<RaidFileBlock> blocks) {
            block_data_type* xorBuf = BytesCopy();
            for (std::vector<RaidFileBlock>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
                block_data_type* theirBytes = it->Bytes();
                for (block_size_t it = 0; it<BLOCK_SIZE; ++it) {
                    xorBuf[it] = xorBuf[it] ^ theirBytes[it];
                }
            }
            return xorBuf;
        }

        inline bool SwapBlockBytes(block_data_type* newBytes, bool forceRelease = false) {
            if (InMemory()) {
                if (!ReleaseMemory(forceRelease)) {
                    return false;
                }
            }
            _bytes = newBytes;
            _uncommittedWrites = true;
            return true;
        }

    protected:
        bool _uncommittedWrites = false;
        const block_pos_t _blockID;
        CRC32::crc _lastCRC32 = 0;
        CRC64::crc _lastCRC64 = 0;
        /** @var _inMem bool Whether the block is loaded into memory or is only on disk **/
        RaidFileBlock::block_data_type *_bytes = nullptr;
    };
}


#endif //RAIDSTREAM_RAIDFILEBLOCK_HPP
