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

TEST(LongSymbolsNormalizerTest, TestConstructorAndSetLongAndreplaceSymbols)
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

    const VCORE_Reels::Reel_t replace_symbols { 8, 9 };
    const vcore::LongSymbolsNormalizer normalizer(long_symbols, replace_symbols);

    ASSERT_EQ(normalizer.GetLongSymbols(), long_symbols);
    ASSERT_EQ(normalizer.GetReplaceSymbols(), replace_symbols);
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

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> reels {
        { { 3, 10, 1, 1, 9, 2 }, { 45, 10, 40, 41, 9, 42 } },
        { { 3, 3, 1, 1, 2, 2 }, { 44, 45, 40, 41, 42, 43 } },
        { { 1, 1, 14, 10, 6, 6 }, { 40, 41, 14, 10, 52, 53 } },
        { { 6, 6, 11, 10, 10, 6 }, { 54, 55, 11, 10, 10, 52 } },
        { { 6, 6, 6, 14, 11, 6 }, { 53, 54, 55, 14, 11, 52 } },
        { { 6, 6, 6, 6, 12, 13 }, { 52, 53, 54, 55, 12, 13 } },
        { { 6, 6, 10, 11, 12, 13}, { 54, 55, 10, 11, 12, 13 } },
    };

    for (const auto& it : reels)
    {
        auto modified_reel = normalizer.GetModifiedReel(it.first);
        EXPECT_EQ(modified_reel, it.second);
    }

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> rollings {
        { { 9, 10, 11, 12, 13, 14, 1, 1, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 1, 1, 6, 6  }, { 9, 10, 11, 12, 13, 14,  40, 41, 52, 53, 54, 55, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 40, 41, 52, 53 } },
    };

    for (const auto& it : rollings)
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

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> reels {
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

    for (const auto& it : reels)
    {
        auto original_reel = normalizer.GetOriginalReel(it.second);
        EXPECT_EQ(original_reel, it.first);
    }

    const std::vector<std::pair<vcore::VCORE_Reels::Reel_t, vcore::VCORE_Reels::Reel_t>> rollings {
        { { 9, 10, 11, 12, 13, 14, 1, 1, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 1, 1, 6, 6  }, { 9, 10, 11, 12, 13, 14,  40, 41, 52, 53, 54, 55, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 40, 41, 52, 53 } },
    };

    for (const auto& it : rollings)
    {
        auto original_reel = normalizer.GetOriginalReel(it.second);
        EXPECT_EQ(original_reel, it.first);
    }
}

TEST(LongSymbolsNormalizerTest, DISABLED_GetModifiedContents_Simple)
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

    const std::vector<vcore::VCORE_Reels::Reel_t> original_contents  {
        { { 9, 49, 50, 9  } },
        { { 9, 53, 54, 9  } },
        { { 9, 57, 58, 9  } },
        { { 9, 45, 10, 9  } },
        { { 9, 44, 45, 9  } },
        { { 9, 40, 41, 9  } },
    };

    const std::vector<vcore::VCORE_Reels::Reel_t> expected_contents {
        { { 48, 49, 50, 51 }, },
        { { 52, 53, 54, 55 }, },
        { { 56, 57, 58, 59 }, },
        { { 44, 45, 10, 9  }, },
        { { 9,  44, 45, 9  }, },
        { { 9,  40, 41, 9  }, },
    };

    EXPECT_EQ(original_contents.size(), expected_contents.size());

    for (size_t i = 0; i < original_contents.size(); ++i)
    {
        const auto modified_reel_contents = normalizer.GetModifiedContents(original_contents[i]);
        EXPECT_EQ(expected_contents[i], modified_reel_contents);
    }
}

TEST(LongSymbolsNormalizerTest, GetModifiedContents)
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

    {
        const std::vector<vcore::VCORE_Reels::Reel_t> original_contents  {
            { 9, 51, 48, 49, 50, 51, 48, 9 },
            { 9, 52, 53, 54, 55, 52, 53, 9 },
            { 9, 59, 56, 57, 58, 59, 56, 9 },
            { 9, 45, 10, 40, 41, 9 , 42, 9 },
            { 9, 44, 45, 40, 41, 42, 43, 9 },
            { 9, 40, 41, 14, 10, 52, 53, 9 },
            { 9, 54, 55, 11, 10, 10, 52, 9 },
            { 9, 53, 54, 55, 14, 11, 52, 9 },
            { 9, 52, 53, 54, 55, 12, 13, 9 },
        };
        
        const std::vector<vcore::VCORE_Reels::Reel_t> new_contents {
            { { 50, 51, 48, 49, 50, 51, 48, 49 }, },
            { { 9, 52, 53, 54, 55, 52, 53, 54 }, },
            { { 58, 59, 56, 57, 58, 59, 56, 57 }, },
            { { 44, 45, 10, 40, 41, 9 , 42, 43 }, },
            { { 9,  44, 45, 40, 41, 42, 43, 9  }, },
            { { 9,  40, 41, 14, 10, 52, 53, 54 }, },
            { { 53, 54, 55, 11, 10, 10, 52, 53 }, },
            { { 52, 53, 54, 55, 14, 11, 52, 53 }, },
            { { 9,  52, 53, 54, 55, 12, 13, 9  }, },
        };

        EXPECT_EQ(new_contents.size(), original_contents.size());

        for (size_t i = 0; i < new_contents.size(); ++i)
        {
            const auto modified_reel_contents = normalizer.GetModifiedContents(original_contents[i]);
            EXPECT_EQ(modified_reel_contents, new_contents[i]);
        }
    }
}
