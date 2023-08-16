#include <gtest/gtest.h>
#include <modules/LongSymbolsNormalizer.hpp>

using namespace vcore;

TEST(LongSymbolsNormalizerTest, TestConstructorAndSetLongSymbols)
{
    const LongSymbols_t long_symbols = {
        { 1, { 40, 41 } },
        { 2, { 42, 43 } },
        { 3, { 44, 45 } },
        { 4, { 46, 47 } },
        { 5, { 48, 49, 50, 51 } },
        { 6, { 52, 53, 54, 55 } },
        { 7, { 56, 57, 58, 59 } }
    };

    const vcore::LongSymbolsNormalizer normalizer(long_symbols);
    ASSERT_EQ(normalizer.GetLongSymbols(), long_symbols);
}

TEST(LongSymbolsNormalizerTest, TestGetModifiedReel)
{
    const LongSymbols_t long_symbols = {
        { 1, { 40, 41 } },
        { 2, { 42, 43 } },
        { 3, { 44, 45 } },
        { 4, { 46, 47 } },
        { 5, { 48, 49, 50, 51 } },
        { 6, { 52, 53, 54, 55 } },
        { 7, { 56, 57, 58, 59 } }
    };

    const vcore::LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> v {
        // { { 5, 5, 5, 5, 5, 5 }, { 51, 48, 49, 50, 51, 48 } },
        // { { 6, 6, 6, 6, 6, 6 }, { 52, 53, 54, 55, 52, 53 } },
        // { { 7, 7, 7, 7, 7, 7 }, { 59, 56, 57, 58, 59, 56 } },
        { { 3, 10, 1, 1, 9, 2 }, { 45, 10, 40, 41, 9, 42 } },
        { { 3, 3, 1, 1, 2, 2 }, {44, 45, 40, 41, 42, 43 } },
        { { 1, 1, 14, 10, 6, 6 }, { 40, 41, 14, 10, 52, 53 } },
        { { 6, 6, 11, 10, 10, 6 }, { 54, 55, 11, 10, 10, 52 } },
        { { 6, 6, 6, 14, 11, 6 }, { 53, 54, 55, 14, 11, 52 } },
        { { 6, 6, 6, 6, 12, 13 }, { 52, 53, 54, 55, 12, 13 } },
        { { 6, 6, 10, 11, 12, 13}, { 54, 55, 10, 11, 12, 13 } },
    };

    for (const auto& it : v)
    {
        auto modified_reel = normalizer.GetModifiedReel(it.first);
        EXPECT_EQ(modified_reel, it.second);
    }
}

TEST(LongSymbolsNormalizerTest, TestGetOriginalReel)
{
    const LongSymbols_t long_symbols = {
        { 1, { 40, 41 } },
        { 2, { 42, 43 } },
        { 3, { 44, 45 } },
        { 4, { 46, 47 } },
        { 5, { 48, 49, 50, 51 } },
        { 6, { 52, 53, 54, 55 } },
        { 7, { 56, 57, 58, 59 } }
    };

    const vcore::LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> v {
        { { 5, 5, 5, 5, 5, 5 }, { 51, 48, 49, 50, 51, 48 } },
        { { 6, 6, 6, 6, 6, 6 }, { 52, 53, 54, 55, 52, 53 } },
        { { 7, 7, 7, 7, 7, 7 }, { 59, 56, 57, 58, 59, 56 } },
        { { 3, 10, 1, 1, 9, 2 }, { 45, 10, 40, 41, 9, 42 } },
        { { 3, 3, 1, 1, 2, 2 }, {44, 45, 40, 41, 42, 43 } },
        { { 1, 1, 14, 10, 6, 6 }, { 40, 41, 14, 10, 52, 53 } },
        { { 6, 6, 11, 10, 10, 6 }, { 54, 55, 11, 10, 10, 52 } },
        { { 6, 6, 6, 14, 11, 6 }, { 53, 54, 55, 14, 11, 52 } },
        { { 6, 6, 6, 6, 12, 13 }, { 52, 53, 54, 55, 12, 13 } },
        { { 6, 6, 10, 11, 12, 13}, { 54, 55, 10, 11, 12, 13 } },
    };

    for (const auto& it : v)
    {
        auto original_reel = normalizer.GetOriginalReel(it.second);
        EXPECT_EQ(original_reel, it.first);
    }
}
