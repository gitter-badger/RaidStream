#include <iostream>
#include <string>
#include "RaidStream/RaidStream.hpp"

int main() {
    // open up the raidstream
    RaidStream::RaidStream rs = RaidStream::RaidStream(RaidStream::RaidStream::MakeConfiguration(
            RaidStream::RaidConfiguration::RaidType::EXPERIMENTAL, {
                    {0, "/tmp/raidstream.0", RaidStream::RaidFile::FileType::DATA},
                    {1, "/tmp/raidstream.1", RaidStream::RaidFile::FileType::DATA},
                    {2, "/tmp/raidstream.2", RaidStream::RaidFile::FileType::DATA},
                    {3, "/tmp/raidstream.3", RaidStream::RaidFile::FileType::DATA},
                    {4, "/tmp/raidstream.4", RaidStream::RaidFile::FileType::PARITY_XOR},
                    {5, "/tmp/raidstream.5", RaidStream::RaidFile::FileType::PARITY_RS},
                    {6, "/tmp/raidstream.6", RaidStream::RaidFile::FileType::PARITY_EXPERIMENTAL}
            }), true, false);
}
