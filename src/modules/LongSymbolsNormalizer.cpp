#include <modules/LongSymbolsNormalizer.hpp>
#include <algorithm>
#include <random>

using namespace vcore;

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, bool use_random)
    : m_long_symbols{ long_symbols }
    , m_replace_symbols({})
    , m_use_random(use_random)
{

}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, const VCORE_Reels::Reel_t& replace_symbols, bool use_random)
    : m_long_symbols{ long_symbols }
    , m_replace_symbols(replace_symbols)
    , m_use_random(use_random)
{

}

VCORE_Reels LongSymbolsNormalizer::GetModifiedReels(const VCORE_Reels& original_reels) const
{
    const auto& stop_matrix = GetModifiedMatrix(original_reels.GetMatrix());
    const auto& rolling_matrix = GetModifiedMatrix(original_reels.GetRollingMatrix());
    return { stop_matrix, rolling_matrix };
}

VCORE_Reels::Matrix_t LongSymbolsNormalizer::GetModifiedMatrix(const VCORE_Reels::Matrix_t& original_matrix) const
{
    auto new_matrix = original_matrix;
    for (auto& reel : new_matrix)
    {
        reel = GetModifiedReel(reel);
    }

    return new_matrix;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedReel(const VCORE_Reels::Reel_t& original_reel) const
{
    if (original_reel.empty() || !HasLongSymbolOnReel(original_reel))
    {
        return original_reel;
    }

    const VCORE_Reels::Reel_t& new_reel = [&]
    {
        const auto first_symbol_id = original_reel.front();
        if (IsEqualAllId(original_reel) && m_long_symbols.count(first_symbol_id))
        {
            return GenerateRandomReel(original_reel.front(), original_reel.size());
        }

        return GenerateReelWithLongSymbols(original_reel);
    }();

    return new_reel;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedContents(const VCORE_Reels::Reel_t& reel) const
{
    VCORE_Reels::Reel_t new_reel = reel;

    for (size_t symbol_pos = 0; symbol_pos < reel.size(); ++symbol_pos)
    {
        const auto original_symbol = reel[symbol_pos];
        const auto found_long_symbol_id = FindLongSymbolId(original_symbol);
        if (found_long_symbol_id.first)
        {
            const size_t symbol_length = GetSymbolLength(reel, symbol_pos);
            const auto long_symbols = m_long_symbols.at(found_long_symbol_id.second);
            if(symbol_length == long_symbols.size())
            {
                continue;
            }

            const size_t start_index = std::min<size_t>(long_symbols.size(), std::distance(long_symbols.begin(), std::find(long_symbols.begin(), long_symbols.end(), original_symbol)));

            if (symbol_pos == 1 && start_index > 0)
            {
                new_reel[symbol_pos - 1] = long_symbols.at(start_index - 1);
            }
            else if (symbol_pos == reel.size() - 2 && start_index < long_symbols.size() - 1)
            {
                new_reel[symbol_pos + 1] = long_symbols.at(start_index + 1);
            }
        }
    }

    return new_reel;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedRolling(const VCORE_Reels::Reel_t& original_reel, const VCORE_Reels::Reel_t& reel_contents) const
{
    VCORE_Reels::Reel_t new_reel = original_reel;
    new_reel.insert(new_reel.end(), reel_contents.begin(), reel_contents.end());
    return new_reel;
}

VCORE_Reels LongSymbolsNormalizer::GetOriginalReels(const VCORE_Reels& modified_reels) const
{
    const auto& stop_matrix = GetOriginalMatrix(modified_reels.GetMatrix());
    const auto& rolling_matrix = GetOriginalMatrix(modified_reels.GetRollingMatrix());
    return { stop_matrix, rolling_matrix };
}

VCORE_Reels::Matrix_t LongSymbolsNormalizer::GetOriginalMatrix(const VCORE_Reels::Matrix_t& modified_matrix) const
{
    auto new_matrix = modified_matrix;
    for (auto& reel : new_matrix)
    {
        reel = GetOriginalReel(reel);
    }

    return new_matrix;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetOriginalReel(const VCORE_Reels::Reel_t& modified_reel) const
{
    if (modified_reel.empty() || !HasLongSymbolOnReel(modified_reel))
    {
        return modified_reel;
    }

    VCORE_Reels::Reel_t new_reel = modified_reel;

    for (auto& symbol_id : new_reel)
    {
        if (IsPartOfLongSymbol(symbol_id))
        {
            symbol_id = GetOriginalSymbolId(symbol_id);
        }

        // VCORE_ASSERT(symbol_id != DEFAULT_REEL_VALUE);
    }

    return new_reel;
}

void LongSymbolsNormalizer::SetLongSymbols(const LongSymbols_t& long_symbols)
{
    m_long_symbols = long_symbols;
}

void LongSymbolsNormalizer::SetSymbolsForReplace(const VCORE_Reels::Reel_t& replace_symbols)
{
    m_replace_symbols = replace_symbols;
}

const LongSymbols_t& LongSymbolsNormalizer::GetLongSymbols() const
{
    return m_long_symbols;
}

const VCORE_Reels::Reel_t& LongSymbolsNormalizer::GetReplaceSymbols() const
{
    return m_replace_symbols;
}

VCORE_Game::Figures_t LongSymbolsNormalizer::GetAdditionalPayTableSymbols(const VCORE_Game::Figures_t& current_symbols) const
{
    VCORE_Game::Figures_t additional_symbols;

    for (const auto& long_symbol : GetLongSymbols())
    {
        auto it = std::find_if(current_symbols.begin(), current_symbols.end(), [=](const auto& figure) {
            return figure.GetIdentity() == long_symbol.first;
            });

        if (it == std::end(current_symbols))
        {
            // LOG_WARNING("LongSymbolsNormalizer.GetAdditionalPayTableSymbols. can't find server figure id: %d", std::to_string(long_symbol.first));
            continue;
        }

        const auto& server_figure = *it;

        for (const auto& replace_id : long_symbol.second)
        {
            std::vector<unsigned int> pay_table;

            const auto& pay_table_size = server_figure.GetPayTableSize() + 1;
            for (size_t pay_index = 0; pay_index < pay_table_size; ++pay_index)
            {
                auto pay = server_figure.GetPayment(pay_index);
                pay_table.emplace_back(pay);
            }

            const auto client_figure = VCORE_Figure(replace_id, pay_table, server_figure.GetMask());
            additional_symbols.emplace_back(client_figure);
        }
    }

    return additional_symbols;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GenerateRandomReel(int symbol_id, size_t new_size) const
{
    std::vector<int> new_reel(new_size, DEFAULT_REEL_VALUE);

    const auto long_symbol_it = m_long_symbols.find(symbol_id);
    if (long_symbol_it != m_long_symbols.end())
    {
        const auto& long_symbol = long_symbol_it->second;

        auto get_random_index = [&]
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> distributor(0, static_cast<int>(long_symbol.size() - 1));
            return distributor(gen);
        };

        int random_index = m_use_random ? get_random_index() : 0;

        for (int& i : new_reel)
        {
            i = long_symbol[random_index];
            random_index = (random_index + 1) % static_cast<int>(long_symbol.size());
        }
    }

    return new_reel;
}

bool LongSymbolsNormalizer::IsNextSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos) const
{
    return pos + 1 < reel.size() && reel[pos] == reel[pos + 1];
}

bool LongSymbolsNormalizer::IsNextPartOfSymbol(const VCORE_Reels::Reel_t& reel, size_t pos) const
{
    if (pos + 1 >= reel.size())
    {
        return false;
    }

    for (const auto& pair : m_long_symbols)
    {
        const auto& long_symbol = pair.second;
        const auto first_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[pos]);
        if (first_symbol_it != long_symbol.end())
        {
            const auto second_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[pos + 1]);
            if (second_symbol_it != long_symbol.end())
            {
                return first_symbol_it < second_symbol_it;
            }

            return false;
        }
    }

    return false;
}

size_t LongSymbolsNormalizer::GetSymbolLength(const VCORE_Reels::Reel_t& reel, size_t start_pos) const
{
    size_t length = 1;
    while (IsNextSymbolSame(reel, start_pos) || IsNextPartOfSymbol(reel, start_pos))
    {
        ++length;
        ++start_pos;
    }
    return length;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GenerateReelWithLongSymbols(const VCORE_Reels::Reel_t& original_reel) const
{
    VCORE_Reels::Reel_t new_reel = original_reel;

    for (size_t symbol_pos = 0; symbol_pos < original_reel.size();)
    {
        auto original_symbol = original_reel[symbol_pos];

        if (IsLongSymbol(original_symbol))
        {
            const size_t symbol_length = GetSymbolLength(original_reel, symbol_pos);
            size_t start_index = symbol_pos == 0 ? std::max(0, static_cast<int>(m_long_symbols.at(original_symbol).size() - symbol_length)) : 0;

            for (size_t count = 0; count < symbol_length && symbol_pos < original_reel.size() && start_index < m_long_symbols.at(original_symbol).size(); ++symbol_pos, ++start_index, ++count)
            {
                new_reel[symbol_pos] = m_long_symbols.at(original_symbol).at(start_index);
            }
        }
        else
        {
            ++symbol_pos;
        }
    }
    return new_reel;
}

bool LongSymbolsNormalizer::HasLongSymbolOnReel(const VCORE_Reels::Reel_t& reel) const
{
    return std::any_of(cbegin(reel), cend(reel), [&](auto symbol_id) {
        return IsLongSymbol(symbol_id) || IsPartOfLongSymbol(symbol_id);
        });
}

bool LongSymbolsNormalizer::IsLongSymbol(VCORE_Figure::Identity_t symbol_id) const
{
    return m_long_symbols.find(symbol_id) != m_long_symbols.end();
}

bool LongSymbolsNormalizer::IsCompleteSymbol(VCORE_Figure::Identity_t symbol_id, size_t length) const
{
    for (const auto& long_symbol : m_long_symbols)
    {
        const auto& symbols = long_symbol.second;
        const auto it = std::find(symbols.begin(), symbols.end(), symbol_id);
        if (it != symbols.end())
        {
            return symbols.size() == length;
        }
    }

    return false;
}

bool LongSymbolsNormalizer::IsPartOfLongSymbol(VCORE_Figure::Identity_t symbol_id) const
{
    return FindLongSymbolId(symbol_id).first;
}

VCORE_Figure::Identity_t LongSymbolsNormalizer::GetOriginalSymbolId(VCORE_Figure::Identity_t modified_symbol_id) const
{
    const auto result = FindLongSymbolId(modified_symbol_id);
    return result.first ? result.second : modified_symbol_id;
}

std::pair<bool, VCORE_Figure::Identity_t> LongSymbolsNormalizer::FindLongSymbolId(VCORE_Figure::Identity_t symbol_id) const
{
    for (const auto& pair : m_long_symbols)
    {
        if (std::find(pair.second.begin(), pair.second.end(), symbol_id) != pair.second.end())
        {
            return { true, pair.first };
        }
    }

    return { false, {} };
}

bool LongSymbolsNormalizer::IsEqualAllId(const VCORE_Reels::Reel_t& original_reel) const
{
    if (original_reel.empty())
    {
        return true;
    }

    return std::all_of(cbegin(original_reel), cend(original_reel), [first_symbol_id = original_reel.front()](int symbol_id) { return symbol_id == first_symbol_id; });
}
