/*
*   This file is part of PKSM
*   Copyright (C) 2016-2018 Bernardo Giordano, Admiral Fish, piepie62
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#ifndef SAVUSUM_HPP
#define SAVUSUM_HPP

#include <algorithm>
#include "personal.hpp"
#include "Sav.hpp"
#include "PK7.hpp"
#include "WC7.hpp"

extern "C"  {
#include "../../source/memecrypto/memecrypto.h"
#include "sha256.h"
}

class SavUSUM : public Sav
{
protected:
    static constexpr u32 chkofs[39] = { 
        0x00000, 0x01000, 0x01200, 0x01400, 0x01600, 0x01E00, 0x02C00, 
        0x03C00, 0x04000, 0x04400, 0x04600, 0x04800, 0x04A00, 0x04C00, 
        0x05200, 0x3B800, 0x41000, 0x41200, 0x42600, 0x44200, 0x4A800, 
        0x50E00, 0x54800, 0x54A00, 0x54C00, 0x65200, 0x65600, 0x66200, 
        0x6A200, 0x6A600, 0x6AE00, 0x6B000, 0x6B800, 0x6BA00, 0x6BC00, 
        0x6BE00, 0x6C000, 0x6C200, 0x6C600
    };

    static constexpr u32 chklen[39] = { 
        0xE28, 0x7C, 0x14, 0xC0, 0x61C, 0xE00, 0xF78, 
        0x228, 0x30C, 0x1FC, 0x4C, 0x4, 0x58, 0x5E6, 
        0x36600, 0X572C, 0x8, 0x1218, 0x1A08, 0x6408, 0x6408, 
        0x3998, 0x100, 0x100, 0x10528, 0x204, 0xB60, 0x3F50, 
        0x358, 0x728, 0x200, 0x718, 0x1FC, 0x200, 0x120, 
        0x1C8, 0x200, 0x39C, 0x400
    };

    // u16 species, u16 formcount
    static constexpr u16 formtable[246] = {
        0x0003, 0x0002, 0x0006, 0x0003, 0x0009, 0x0002, 0x000F, 0x0002,
        0x0012, 0x0002, 0x0013, 0x0002, 0x0014, 0x0003, 0x0019, 0x0008,
        0x001A, 0x0002, 0x001B, 0x0002, 0x001C, 0x0002, 0x0025, 0x0002,
        0x0026, 0x0002, 0x0032, 0x0002, 0x0033, 0x0002, 0x0034, 0x0002,
        0x0035, 0x0002, 0x0041, 0x0002, 0x004A, 0x0002, 0x004B, 0x0002,
        0x004C, 0x0002, 0x0050, 0x0002, 0x0058, 0x0002, 0x0059, 0x0002,
        0x005E, 0x0002, 0x0067, 0x0002, 0x0069, 0x0003, 0x0073, 0x0002,
        0x007F, 0x0002, 0x0082, 0x0002, 0x008E, 0x0002, 0x0096, 0x0003,
        0x00B5, 0x0002, 0x00C9, 0x001C, 0x00D0, 0x0002, 0x00D4, 0x0002,
        0x00D6, 0x0002, 0x00E5, 0x0002, 0x00F8, 0x0002, 0x00FE, 0x0002,
        0x0101, 0x0002, 0x0104, 0x0002, 0x011A, 0x0002, 0x012E, 0x0002,
        0x012F, 0x0002, 0x0132, 0x0002, 0x0134, 0x0002, 0x0136, 0x0002,
        0x013F, 0x0002, 0x0143, 0x0002, 0x014E, 0x0002, 0x015F, 0x0004,
        0x0162, 0x0002, 0x0167, 0x0002, 0x016A, 0x0002, 0x0175, 0x0002,
        0x0178, 0x0002, 0x017C, 0x0002, 0x017D, 0x0002, 0x017E, 0x0002,
        0x017F, 0x0002, 0x0180, 0x0002, 0x0182, 0x0004, 0x019C, 0x0003,
        0x019D, 0x0003, 0x019E, 0x0003, 0x01A5, 0x0002, 0x01A6, 0x0002,
        0x01A7, 0x0002, 0x01AC, 0x0002, 0x01BD, 0x0002, 0x01C0, 0x0002,
        0x01CC, 0x0002, 0x01DB, 0x0002, 0x01DF, 0x0006, 0x01E7, 0x0002,
        0x01EC, 0x0002, 0x01ED, 0x0012, 0x0213, 0x0002, 0x0226, 0x0002,
        0x022B, 0x0002, 0x0249, 0x0004, 0x024A, 0x0004, 0x0281, 0x0002,
        0x0282, 0x0002, 0x0285, 0x0002, 0x0286, 0x0003, 0x0287, 0x0002,
        0x0288, 0x0002, 0x0289, 0x0005, 0x0292, 0x0003, 0x0298, 0x0014,
        0x0299, 0x0014, 0x029A, 0x0014, 0x029D, 0x0005, 0x029E, 0x0006,
        0x029F, 0x0005, 0x02A4, 0x000A, 0x02A6, 0x0002, 0x02A9, 0x0002,
        0x02C6, 0x0004, 0x02C7, 0x0004, 0x02CC, 0x0002, 0x02CE, 0x0005,
        0x02CF, 0x0002, 0x02D0, 0x0002, 0x02DF, 0x0002, 0x02E2, 0x0002,
        0x02E5, 0x0004, 0x02E7, 0x0002, 0x02E8, 0x0002, 0x02E9, 0x0003,
        0x02EA, 0x0002, 0x02F0, 0x0002, 0x02F2, 0x0002, 0x02F6, 0x0002,
        0x0305, 0x0012, 0x0306, 0x000E, 0x0309, 0x0002, 0x030A, 0x0004,
        0x0310, 0x0002, 0x0320, 0x0004, 0x0321, 0x0002
    };

    int dexFormIndex(int species, int formct, int start) const;
    int dexFormCount(int species) const;
    void setDexFlags(int index, int gender, int shiny, int baseSpecies);
    bool sanitizeFormsToIterate(int species, int& fs, int& fe, int formIn) const;

public:
    SavUSUM(u8* dt);
    virtual ~SavUSUM() { };

    u16 check16(u8* buf, u32 blockID, u32 len) const;
    void resign(void) override;

    u16 TID(void) const override;
    void TID(u16 v) override;
    u16 SID(void) const override;
    void SID(u16 v) override;
    u8 version(void) const override;
    void version(u8 v) override;
    u8 gender(void) const override;
    void gender(u8 v) override;
    u8 subRegion(void) const override;
    void subRegion(u8 v) override;
    u8 country(void) const override;
    void country(u8 v) override;
    u8 consoleRegion(void) const override;
    void consoleRegion(u8 v) override;
    u8 language(void) const override;
    void language(u8 v) override;
    std::string otName(void) const override;
    void otName(const char* v) override;
    u32 money(void) const override;
    void money(u32 v) override;
    u32 BP(void) const override;
    void BP(u32 v) override;
    u16 playedHours(void) const override;
    void playedHours(u16 v) override;
    u8 playedMinutes(void) const override;
    void playedMinutes(u8 v) override;
    u8 playedSeconds(void) const override;
    void playedSeconds(u8 v) override;

    u8 currentBox(void) const override;
    void currentBox(u8 v) override;
    u32 boxOffset(u8 box, u8 slot) const override;
    u32 partyOffset(u8 slot) const override;
    
    std::unique_ptr<PKX> pkm(u8 slot) const override;
    std::unique_ptr<PKX> pkm(u8 box, u8 slot, bool ekx = false) const override;

    // NOTICE: this sets a pkx into the savefile, not a pkx
    // that's because PKSM works with decrypted boxes and
    // crypts them back during resigning
    void pkm(PKX& pk, u8 box, u8 slot) override;
    void pkm(PKX& pk, u8 slot) override;

    std::shared_ptr<PKX> emptyPkm() const override;

    void dex(PKX& pk) override;
    int emptyGiftLocation(void) const override;
    std::vector<MysteryGift::giftData> currentGifts(void) const override;
    void mysteryGift(WCX& wc, int& pos) override;
    void cryptBoxData(bool crypted) override;
    std::string boxName(u8 box) const override;
    void boxName(u8 box, std::string name) override;
    u8 partyCount(void) const override;

    int maxBoxes(void) const override { return 32; }
    size_t maxWondercards(void) const override { return 48; }
    Generation generation(void) const override { return Generation::SEVEN; }
    int maxSpecies(void) const { return 807; }
    int maxMove(void) const { return 728; }
    int maxItem(void) const { return 959; }
    int maxAbility(void) const { return 233; }
    int maxBall(void) const { return 0x1A; }
};

#endif