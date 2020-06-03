#ifndef RAIDSTREAM_RAIDSTREAM_HPP
#define RAIDSTREAM_RAIDSTREAM_HPP

#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <filesystem>
#include "RaidStream/RaidConfiguration.hpp"
#include "RaidStream/RaidFile.hpp"

namespace RaidStream {
    class RaidStream {
    public:
        enum RaidStreamStatus {
            CLOSED,
            OPENING_UNVERIFIED,
            OPENING_UNVERIFIED_VERIFYING,
            ERROR,
            DEGRADED,
            DEGRADED_REBUILDING,
            CONSISTENT,
            SYNCING,
            SYNCING_DEGRADED,
            SYNCING_REBUILDING,
            CLOSING,
            OFFLINE,
            OFFLINE_DEGRADED,
            OFFLINE_REBUILDING
        };

        RaidStream(RaidConfiguration configuration, bool existingArray, bool allowInitializeArray = false) :
            _configuration{configuration},
            _openExistingArray{existingArray},
            _openAllowInitialize{allowInitializeArray}
        {}

//        void open(std::ios_base::openmode __mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary) {
//            if (!Closed()) {
//                throw std::invalid_argument("Unable to open RaidStream when not closed");
//            }
//            // open all the handles
//            _status = RaidStreamStatus::OPENING_UNVERIFIED;
//            for (std::vector<RaidFile>::iterator it = _configuration.Files().begin(); it != _configuration.Files().end(); it++) {
//                it->Mode(__mode);
//                _handles[&(*it)] = std::fstream(it->FileName(), __mode);
//            }
//            // verify that all the files are associated and consistent
//            _status = RaidStreamStatus::OPENING_UNVERIFIED_VERIFYING;
//            // TODO: verify()
//        }


        inline bool Online() {
            return (!Opening() && !Closing() && !Closed() && !Error());
        }

        inline bool Opening() {
            return (InStatus(std::vector<RaidStreamStatus>({
                 {RaidStreamStatus::OPENING_UNVERIFIED},
                 {RaidStreamStatus::OPENING_UNVERIFIED_VERIFYING}
         })));
        }

        inline bool Closing() {
            return (_status == RaidStreamStatus::CLOSING);
        }

        inline bool Closed() {
            return (_status == RaidStreamStatus::CLOSED);
        }

        inline bool Error() {
            return (_status == RaidStreamStatus::ERROR);
        }

//        void Close() {
//            if (Closing() || Closed()) {
//                throw std::invalid_argument("Unable to Close RaidStream when not open");
//            }
//            // Sync!
//            Sync();
//            // Close all the handles
//            _status = RaidStreamStatus::CLOSING;
//            for (std::vector<RaidFile>::iterator it = _configuration.Files().begin(); it != _configuration.Files().end(); it++) {
//                _handles[&(*it)].close();
//            }
//            // verify that all the files are associated and consistent
//            _status = RaidStreamStatus::OPENING_UNVERIFIED_VERIFYING;
//            // TODO: verify()
//        }

        static RaidConfiguration MakeConfiguration(RaidConfiguration::RaidType type, std::vector<RaidFile> files) {
            return RaidConfiguration(type, files);
        }

        inline bool Degraded() {
            return InStatus(std::vector<RaidStreamStatus> ({
               {RaidStreamStatus::DEGRADED},
               {RaidStreamStatus::DEGRADED_REBUILDING},
               {RaidStreamStatus::OFFLINE_DEGRADED},
               {RaidStreamStatus::SYNCING_DEGRADED}
            }));
        }

        inline bool Rebuilding() {
            return InStatus(std::vector<RaidStreamStatus> ({
                {RaidStreamStatus::DEGRADED_REBUILDING},
                {RaidStreamStatus::OFFLINE_REBUILDING},
                {RaidStreamStatus::SYNCING_REBUILDING}
            }));
        }

        inline bool Consistent() {
            // TODO; check for impossible states first
            if (_status == RaidStreamStatus::CLOSED) return false; // can't check consistency if closed

            for (std::vector<RaidFile>::iterator it = _configuration.Files().begin(); it != _configuration.Files().end(); it++) {
                if (!it->Consistent()) return false;
            }
            return true;
        }

        inline void Rebuild(bool forceRebuild = false) {
            if (!Online() || Rebuilding() || (Consistent() && !forceRebuild)) return;
            // TODO: Rebuild
        }

        friend std::ostream &operator<<(std::ostream &os, const RaidStream &raidstream) {

        }

        inline bool InStatus(std::vector<RaidStreamStatus> statusList) {
            for (std::vector<RaidStreamStatus>::iterator it = statusList.begin(); it != statusList.end(); ++it) {
                if (_status == (*it)) return true;
            }
            return false;
        }

        inline void Sync() {
            if (needFlush() && Closed()) throw std::invalid_argument("Sync Failure. Resource not available.");
            // write each uncommitted byte to disk
//            std::streamsize bytesToSync = _uncommitted.in_avail();
//            for (std::streamsize it = 0; it < bytesToSync; it++) {
//                // fan out write to disk with known data- update all rows
//                ForceWrite(_currentOffset, _uncommitted.sgetc());
//            }
        }

        inline void Seek(uint64_t offset, bool seekSpares = false) {
            // TODO: write mutex
            if (!Online()) return;
//            for (std::map<RaidFile*, std::fstream>::iterator it = _handles.begin(); it != _handles.end(); ++it) {
//                if (!seekSpares && (it->first->Type() == RaidFile::FileType::SPARE)) continue;
//                it->second.seekg(offset);
//            }
            // TODO: check for errors
            _currentOffset = offset;
        }

        inline void ForceWrite(uint64_t offset, char data) {
            // TODO:
            if (!Online()) return;
//            for (std::map<RaidFile*, std::fstream>::iterator it = _handles.begin(); it != _handles.end(); ++it) {
//                if (it->first->Type() == RaidFile::FileType::SPARE) continue;
//                it->second.seekg(offset);
//                const unsigned char c = CalculateKnownData(it->first, offset, data, nullptr);
//                it->second.write(reinterpret_cast<const char *>(&c), 1);
////                else throw std::invalid_argument("Unexpected spare disk.");
//            }

        }

        inline const unsigned char CalculateKnownData(RaidFile* file, uint64_t offset, const unsigned char data, bool *spare = nullptr) {
            // TODO: there's a dependency order required here
            // [ A | B | C | D | E ]
            // A = DATA
            // B = XOR_0
            // C =
            // E = RS_0
            // depending on which disk in the array this is
            if (spare != nullptr) *spare = false;
            switch (file->Type()) {
                case RaidFile::FileType::SPARE:
                    if (spare != nullptr) *spare = true;
                    // spare fallthrough, return data for spares since it has no calc
                case RaidFile::FileType::DATA:
                    // data disks just get the data
                    return data;
                case RaidFile::FileType::PARITY_XOR:
                    // take XOR of remaining disks
//                    unsigned char c = data; // start with data
//                    for (std::map<RaidFile*, std::fstream>::iterator it = _handles.begin(); it != _handles.end(); ++it) {
//                        if (it->first->FileName() == file->FileName()) {
//                            continue; // skip
//                        }
//                        c = c ^
//
//                    }

                    break;
                case RaidFile::FileType::PARITY_RS:
                    break;
                case RaidFile::FileType::PARITY_EXPERIMENTAL:
                    break;
                default:
                    throw std::invalid_argument("Unknown raid column type");
            }
            // already thrown in default by here or retunred
            assert(false);
        }

        inline std::vector<unsigned char> readAll(uint64_t offset, bool readSpares = false) {
            std::vector<unsigned char> results;
            // seek all disks to the location
            Seek(offset, readSpares);
//            for (std::map<RaidFile *, std::fstream>::iterator it = _handles.begin(); it != _handles.end(); ++it) {
//                if (!readSpares && (it->first->Type() == RaidFile::FileType::SPARE)) {
//                    results.push_back(NULL);
//                } else {
//                    results.push_back(it->second.get());
//                }
//            }
            return results;
        }

    protected:
        inline bool needFlush() {
            // TODO: check if any uncommitted writes to file buffers or if any of those need syncing
//            return _uncommitted.in_avail();
            return false;
        }

        inline void flush() {
            if (!needFlush()) return;
            _status = RaidStreamStatus::SYNCING;
        }
        RaidStreamStatus _status = RaidStreamStatus::CLOSED;
        bool _openExistingArray = false;
        bool _openAllowInitialize = false;
        uint64_t _currentOffset = 0;
        uint64_t _normalizedDeviceSize = 0;
        uint64_t _largestDeviceSize = 0;
        RaidConfiguration _configuration;
//        std::streambuf _uncommitted;
    };
}
#endif //RAIDSTREAM_RAIDSTREAM_HPP
