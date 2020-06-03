#ifndef RAIDSTREAM_RAIDSTREAM_HPP
#define RAIDSTREAM_RAIDSTREAM_HPP

#include <fstream>
#include <iostream>
#include <vector>
#include <raidfile.hpp>
#include <filesystem>
#include "configuration.hpp"

namespace RaidStream {
    class RaidStream : std::fstream {
    public:
        RaidStream(Configuration configuration) : _configuration{configuration} {}
        friend std::ostream &operator<<(std::ostream &os, const RaidStream &raidstream) {

        }
        void open(const char* __s, ios_base::openmode __mode = ios_base::in | ios_base::out) {

        }
        static Configuration MakeConfiguration(std::vector<RaidFile> files) {
            return Configuration(files);
        }

    protected:
        Configuration _configuration;
    };
}
#endif //RAIDSTREAM_RAIDSTREAM_HPP
