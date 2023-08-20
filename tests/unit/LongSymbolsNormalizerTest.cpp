#include <gtest/gtest.h>
#include <modules/LongSymbolsNormalizer.hpp>

using namespace vcore;

namespace
{
    std::string VectorToString(const VCORE_Reels::Reel_t& vec) {
        std::ostringstream oss;
        oss << "{";
        for (size_t i = 0; i < vec.size(); ++i) {
            oss << vec[i];
            if (i != vec.size() - 1) {
                oss << ", ";
            }
        }
        oss << "}";
        return oss.str();
    }
}

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

    const LongSymbolsNormalizer normalizer(long_symbols);
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
    const LongSymbolsNormalizer normalizer(long_symbols, replace_symbols);

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

    const LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<std::pair<VCORE_Reels::Reel_t, VCORE_Reels::Reel_t>> reels {
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

    const std::vector<std::pair<VCORE_Reels::Reel_t, VCORE_Reels::Reel_t>> rollings {
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

    const LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<std::pair<VCORE_Reels::Reel_t, VCORE_Reels::Reel_t>> reels {
        { { 5, 5, 5, 5, 5, 5  }, { 51, 48, 49, 50, 51, 48 } },
        { { 6, 6,  6,  6,  6,  6  }, { 52, 53, 54, 55, 52, 53 } },
        { { 7, 7,  7,  7,  7,  7  }, { 59, 56, 57, 58, 59, 56 } },
        { { 3, 10, 1,  1,  9,  2  }, { 45, 10, 40, 41, 9, 42 } },
        { { 3, 3,  1,  1,  2,  2  }, { 44, 45, 40, 41, 42, 43 } },
        { { 1, 1,  14, 10, 6,  6  }, { 40, 41, 14, 10, 52, 53 } },
        { { 6, 6,  11, 10, 10, 6  }, { 54, 55, 11, 10, 10, 52 } },
        { { 6, 6,  6,  14, 11, 6  }, { 53, 54, 55, 14, 11, 52 } },
        { { 6, 6,  6,  6,  12, 13 }, { 52, 53, 54, 55, 12, 13 } },
        { { 6, 6,  10, 11, 12, 13 }, { 54, 55, 10, 11, 12, 13 } },
    };

    for (const auto& it : reels)
    {
        auto original_reel = normalizer.GetOriginalReel(it.second);
        EXPECT_EQ(original_reel, it.first);
    }

    const std::vector<std::pair<VCORE_Reels::Reel_t, VCORE_Reels::Reel_t>> rollings {
        { { 9, 10, 11, 12, 13, 14, 1, 1, 6, 6, 6, 6, 7, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 1, 1, 6, 6  }, { 9, 10, 11, 12, 13, 14, 40, 41, 52, 53, 54, 55, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 40, 41, 52, 53 } },
    };

    for (const auto& it : rollings)
    {
        auto original_reel = normalizer.GetOriginalReel(it.second);
        EXPECT_EQ(original_reel, it.first);
    }
}

TEST(LongSymbolsNormalizerTest, GetModifiedContents_Simple)
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

    const LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<VCORE_Reels::Reel_t> original_contents  {
        { { 9, 49, 50, 9  } },
        { { 9, 53, 54, 9  } },
        { { 9, 57, 58, 9  } },
        { { 9, 45, 10, 9  } },
        { { 9, 44, 45, 9  } },
        { { 9, 40, 41, 9  } },
    };

    const std::vector<VCORE_Reels::Reel_t> expected_contents {
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

    const LongSymbolsNormalizer normalizer(long_symbols);

    const std::vector<VCORE_Reels::Reel_t> original_contents  {
        { 9, 51, 48, 49, 50, 51, 48, 9  },
        { 9,  52, 53, 54, 55, 52, 53, 9 },
        { 9,  59, 56, 57, 58, 59, 56, 9 },
        { 9,  45, 10, 40, 41, 9 , 42, 9 },
        { 9,  44, 45, 40, 41, 42, 43, 9 },
        { 9,  40, 41, 14, 10, 52, 53, 9 },
        { 9,  54, 55, 11, 10, 10, 52, 9 },
        { 9,  53, 54, 55, 14, 11, 52, 9 },
        { 9,  52, 53, 54, 55, 12, 13, 9 },
        { 55, 52, 53 ,54 ,55, 8 , 52, 55 },
        { 57, 11, 11, 56, 57, 58, 59, 57 },

        { 50, 8 , 48, 49, 50, 51, 48, 50 },
        { 55, 52, 53, 54, 55, 8 , 52, 55 },
        { 57, 8 , 10, 56, 57, 58, 59, 57 },
        { 45, 54, 55, 44, 45, 9 , 44, 45 },
        { 44, 53, 54, 55, 44, 45, 40, 44 },
        { 54, 55, 52, 53, 54, 55, 52, 53 },
    };

    const std::vector<VCORE_Reels::Reel_t> expected_contents {
        { 50, 51, 48, 49, 50, 51, 48, 49 },
        { 9,  52, 53, 54, 55, 52, 53, 54 },
        { 58, 59, 56, 57, 58, 59, 56, 57 },
        { 44, 45, 10, 40, 41, 9 , 42, 43 },
        { 9,  44, 45, 40, 41, 42, 43, 9 },
        { 9,  40, 41, 14, 10, 52, 53, 54 },
        { 53, 54, 55, 11, 10, 10, 52, 53 },
        { 52, 53, 54, 55, 14, 11, 52, 53 },
        { 9,  52, 53, 54, 55, 12, 13, 9 },
        { 55, 52, 53 ,54 ,55, 8 , 52, 53 },
        { 59, 11, 11, 56, 57, 58, 59, 56 },

        { 51, 8 , 48, 49, 50, 51, 48, 49 },
        { 55, 52, 53, 54, 55, 8 , 52, 53 },
        { 59, 8 , 10, 56, 57, 58, 59, 56 },
        { 53, 54, 55, 44, 45, 9 , 44, 45 },
        { 52, 53, 54, 55, 44, 45, 40, 41 },
        { 52, 53, 54, 55, 52, 53, 54, 55 },
    };

    EXPECT_EQ(expected_contents.size(), original_contents.size());

    for (size_t i = 0; i < expected_contents.size(); ++i)
    {
        const auto modified_reel_contents = normalizer.GetModifiedContents(original_contents[i]);
        EXPECT_EQ(expected_contents[i], modified_reel_contents);
    }
}

TEST(LongSymbolsNormalizerTest, GetModifiedFakeRollings)
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

    const VCORE_Reels::Reel_t replace_id = { 9 };

    const LongSymbolsNormalizer normalizer(long_symbols, replace_id);

    const std::vector<VCORE_Reels::Reel_t> current_contents {
        { 51, 8, 48, 49, 50, 51, 48, 49  },
        { 55, 52, 53, 54, 55, 8, 52, 53  },
        { 59, 8, 10, 56, 57, 58, 59, 56  },
        { 53, 54, 55, 44, 45, 9, 44, 45  },
        { 52, 53, 54, 55, 44, 45, 40, 41 },
    };

    const std::vector<VCORE_Reels::Reel_t> original_fake_rolling  {
        { 9, 10, 11, 12, 13, 14, 40, 41, 52, 53, 54, 55, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 40, 41, 52, 53 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 50, 51, 42, 43, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 48, 49, 50, 51 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 50, 51, 44, 45, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 48, 49, 50, 51 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 50, 51, 52, 53, 54, 55, 40, 41, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14 },
        { 9, 10, 11, 12, 13, 14, 52, 53, 54, 55, 42, 43, 56, 8, 9, 10, 11, 12, 13, 14, 9, 10, 11, 12, 13, 14, 52, 53, 54, 55 },
    };

    const std::vector<VCORE_Reels::Reel_t> expected_fake_rolling {
        { 9, 10, 11, 12, 13, 14, 40, 41, 51, 52, 53, 54, 55,  8,  9, 10, 11, 12, 13, 14,  9, 10, 11, 12, 13, 14, 9, 48, 49, 50 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 55,  9, 42, 43,  9,  8,  9, 10, 11, 12, 13, 14,  9, 10, 11, 12, 13, 14,  9, 52, 53, 54 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 59,  9, 44, 45,  9,  8,  9, 10, 11, 12, 13, 14,  9, 10, 11, 12, 13, 14,  9, 56, 57, 58 },
        { 9, 10, 11, 12, 13, 14, 48, 49, 53, 54, 55, 52, 53, 54, 55, 40, 41,  8,  9, 10, 11, 12, 13, 14,  9, 10, 11, 12, 13, 52 },
        { 9, 10, 11, 12, 13, 14, 52, 53, 52, 53, 54, 55,  9,  8,  9, 10, 11, 12, 13, 14,  9, 10, 11, 12, 13, 14, 52, 53, 54, 55 },
    };

    EXPECT_EQ(original_fake_rolling.size(), expected_fake_rolling.size());
    EXPECT_EQ(original_fake_rolling.size(), current_contents.size());

    for (size_t i = 0; i < expected_fake_rolling.size(); ++i)
    {
        const auto modified_reel_contents = normalizer.GetModifiedFakeRolling(original_fake_rolling[i], current_contents[i]);
        EXPECT_EQ(expected_fake_rolling[i], modified_reel_contents) << "\original_fake_rolling: " << VectorToString(original_fake_rolling[i]);
    }
}

TEST(LongSymbolsNormalizerTest, DISABLED_GetModifiedTrueRollings)
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

    const VCORE_Reels::Reel_t replace_id = { 9 };

    const LongSymbolsNormalizer normalizer(long_symbols, replace_id);

    const std::vector<VCORE_Reels::Reel_t> current_contents {
        { 41, 48, 49, 50, 51,  8, 48, 49 },
        { 51,  9, 10,  8,  9, 52, 53, 54 },
    };

    const std::vector<VCORE_Reels::Reel_t> original_true_rolling  {
        { 40,  8, 48, 49, 50, 51, 48, 41, 52, 53, 9, 10, 11, 12 },
        { 48, 52, 53, 54, 55,  8, 52, 49, 50, 51, 9, 10, 11, 12 },
    };

    const std::vector<VCORE_Reels::Reel_t> expected_true_rolling {
        { 41, 8, 48, 49, 50, 51, 48, 40, 41, 9, 9, 10, 11, 40 },
        { 51, 52, 53, 54, 55,  8, 52, 53, 54, 55, 9, 48, 49, 50 },
    };

    EXPECT_EQ(original_true_rolling.size(), expected_true_rolling.size());
    EXPECT_EQ(original_true_rolling.size(), current_contents.size());

    for (size_t i = 0; i < expected_true_rolling.size(); ++i)
    {
        const auto modified_reel_contents = normalizer.GetModifiedTrueRolling(original_true_rolling[i], current_contents[i]);
        EXPECT_EQ(expected_true_rolling[i], modified_reel_contents);
    }
}