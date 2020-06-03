#include <iostream>
#include <string>
#include <raidstream.hpp>

int main() {
    std::vector<RaidStream::RaidFile> files = {
            {"/tmp/raidstream.0", RaidStream::RaidFile::FileType::DATA},
            {"/tmp/raidstream.1", RaidStream::RaidFile::FileType::DATA},
            {"/tmp/raidstream.2", RaidStream::RaidFile::FileType::DATA},
            {"/tmp/raidstream.3", RaidStream::RaidFile::FileType::DATA},
            {"/tmp/raidstream.4", RaidStream::RaidFile::FileType::PARITY_XOR},
            {"/tmp/raidstream.5", RaidStream::RaidFile::FileType::PARITY_RS},
            {"/tmp/raidstream.6", RaidStream::RaidFile::FileType::PARITY_EXPERIMENTAL},
    };

    // open up the raidstream
    RaidStream::RaidStream rs = RaidStream::RaidStream(files);

}
