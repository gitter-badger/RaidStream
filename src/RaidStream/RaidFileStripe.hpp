//
// Created by Jessica Mulein on 6/6/20.
//

#ifndef RAIDSTREAM_RAIDFILESTRIPE_HPP
#define RAIDSTREAM_RAIDFILESTRIPE_HPP
#include "RaidFileBlock.hpp"

namespace RaidStream {
    enum RaidStripeType {
        META,
        JBOD,
        RAID5,
        RAID6,
        EXPERIMENTAL
    };

    class RaidFileStripe {
    public:
        inline RaidFileStripe(RaidFileStripe type, std::vector<RaidFileBlock> blocks) : _type{type} {
            bool setBlockSize = true;
            for (std::vector<RaidFileBlock>::iterator it = blocks.begin(); it != blocks.end(); ++it) {
                if (setBlockSize) {
                    _blockSize = it->BlockSize();
                    setBlockSize = false;
                } else if (it->BlockSize() != _blockSize) {
                    throw new std::invalid_argument("Block sizes must match throughout stripe");
                }
               _stripeBlocks[it->BlockID()] = *it;
                if (it->)
            }
        }
    protected:
        const RaidStripeType _type;
        RaidFileBlock::block_size_t _blockSize;
        size_t _effectiveStripSize;
        raid_block_map _stripeBlocks;
    };
}


#endif //RAIDSTREAM_RAIDFILESTRIPE_HPP
