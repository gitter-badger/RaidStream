//
// Created by Jessica Mulein on 6/3/20.
//

#ifndef RAIDSTREAM_CONFIGURATION_HPP
#define RAIDSTREAM_CONFIGURATION_HPP

#include <vector>
#include <raidfile.hpp>

namespace RaidStream {
    class Configuration {
    public:
        Configuration(std::vector<RaidFile> files, std::fstream *stdout = nullptr, std::fstream *stderr = nullptr) {
            Configuration* This = this;
            for(std::vector<RaidFile>::iterator it = files.begin(); it != files.end(); ++it) {
                std::error_code ec;
                uintmax_t fileSize = std::filesystem::file_size(it->Filename(), ec);
                if (ec) {
                    This->warn("ERROR: " + it->Filename() + ": " + ec.message());
                }
                if (fileSize > 0) {
                    // if it's empty, its ready to initialize
                    // if it's not, this file has data!
                    This->warn("WARNING: " + it->Filename() + ": File has existing data");
                }
                switch(it->Type()) {
                    case RaidFile::FileType::DATA:
                        this->_filesData++;
                        break;
                    case RaidFile::FileType::PARITY_XOR:
                        this->_filesXor++;
                        break;
                    case RaidFile::FileType::PARITY_RS:
                        this->_filesReedSolomon++;
                        break;
                    case RaidFile::FileType::PARITY_EXPERIMENTAL:
                        this->_filesExperimental++;
                        break;
                    default:
                        throw std::invalid_argument("Unexpected RaidFile::FileType");
                }
                this->_files.push_back(*it);
            }
        }

        inline std::pair<unsigned int, unsigned int> logStats() {
            return std::make_pair(_logCount, _warningCount);
        }

        inline void log(std::string data) {
            _logCount++;
            if ((_stdout == nullptr) || (_stdout == NULL)) return;
            *_stdout << data << std::endl;
        }

        inline void warn(std::string warning) {
            _warningCount++;
            if ((_stderr == nullptr) || (_stderr == NULL)) return;
            *_stderr << warning << std::endl;
        }
        inline void setStdOut(std::fstream* fs) {
            _stdout = fs;
        }
        inline void setStdErr(std::fstream* fs) {
            _stderr = fs;
        }
    protected:
        std::vector<RaidFile> _files;
        std::fstream* _stdout = nullptr;
        std::fstream* _stderr = nullptr;
        unsigned long _logCount = 0;
        unsigned long _warningCount = 0;
        unsigned short _filesData = 0;
        unsigned short _filesXor = 0;
        unsigned short _filesReedSolomon = 0;
        unsigned short _filesExperimental = 0;
    };
}


#endif //RAIDSTREAM_CONFIGURATION_HPP
