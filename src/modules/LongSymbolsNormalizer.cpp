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
    if (IsEqualAllId(reel))
    {
        const auto original_reel = GetOriginalReel(reel);
        return GenerateRandomReel(original_reel.front(), original_reel.size());
    }

    auto new_reel = reel;

    for (size_t symbol_pos = 0; symbol_pos < new_reel.size(); ++symbol_pos)
    {
        HandleSymbolModification(new_reel, symbol_pos);
    }

    return new_reel;
}

void LongSymbolsNormalizer::HandleSymbolModification(VCORE_Reels::Reel_t& new_reel, size_t symbol_pos) const
{
    const auto long_symbols = GetLongSymbol(new_reel[symbol_pos]);
    UpdateReelWithLongSymbols(new_reel, long_symbols, symbol_pos);
}

void LongSymbolsNormalizer::UpdateReelWithLongSymbols(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos) const
{
    if (symbol_pos == 0 && symbol_pos + 1 < reel.size())
    {
        const auto long_next_symbol = GetLongSymbol(reel[symbol_pos + 1]);
        if (long_symbols.empty() && long_next_symbol.empty())
        {
            return;
        }

        const size_t symbol_length = GetSymbolLength(reel, symbol_pos + 1);
        if (!long_next_symbol.empty() && long_next_symbol.size() == symbol_length)
        {
            if (!long_symbols.empty())
            {
                reel[symbol_pos] = long_symbols.at(long_symbols.size() - 1);
            }
        }
        else if(!long_next_symbol.empty() && long_next_symbol.size() != symbol_length)
        {
            const size_t index = std::distance(long_next_symbol.begin(), std::find(long_next_symbol.begin(), long_next_symbol.end(), reel[symbol_pos + 1]));
            reel[symbol_pos] = long_next_symbol.at(index == 0 ? long_next_symbol.size() - 1 : index - 1);
        }
        else if (long_symbols.empty() && !long_next_symbol.empty())
        {
            const size_t index = std::distance(long_next_symbol.begin(), std::find(long_next_symbol.begin(), long_next_symbol.end(), reel[symbol_pos + 1]));
            reel[symbol_pos] = long_next_symbol.at(index == 0 ? long_next_symbol.size() - 1 : index - 1);
        }
        else if (long_symbols == long_next_symbol)
        {
            const size_t index = std::distance(long_symbols.begin(), std::find(long_symbols.begin(), long_symbols.end(), reel[symbol_pos + 1]));
            reel[symbol_pos] = long_symbols.at(index == 0 ? long_symbols.size() - 1 : index - 1);
        }
        else if (!long_symbols.empty())
        {
            reel[symbol_pos] = long_symbols.at(long_symbols.size() - 1);
        }
    }
    else if (symbol_pos == reel.size() - 1 && symbol_pos - 1 > 0)
    {
        const auto long_prev_symbol = GetLongSymbol(reel[symbol_pos - 1]);
        if (long_symbols.empty() && long_prev_symbol.empty())
        {
            return;
        }

        const size_t symbol_length = GetPrevSymbolLength(reel, symbol_pos - 1);
        if (!long_prev_symbol.empty() && long_prev_symbol.size() == symbol_length)
        {
            if (!long_symbols.empty())
            {
                reel[symbol_pos] = long_symbols.at(0);
            }
        }
        else if (!long_prev_symbol.empty() && long_prev_symbol.size() != symbol_length)
        {
            const size_t index = std::distance(long_prev_symbol.begin(), std::find(long_prev_symbol.begin(), long_prev_symbol.end(), reel[symbol_pos - 1]));
            reel[symbol_pos] = long_prev_symbol.at(index == long_prev_symbol.size() - 1 ? 0 : index + 1);
        }
        else if (long_symbols.empty() && !long_prev_symbol.empty())
        {
            const size_t index = std::distance(long_prev_symbol.begin(), std::find(long_prev_symbol.begin(), long_prev_symbol.end(), reel[symbol_pos - 1]));
            reel[symbol_pos] = long_prev_symbol.at(index == long_prev_symbol.size() - 1 ? 0 : index + 1);
        }
        else if (long_symbols == long_prev_symbol)
        {
            const size_t index = std::distance(long_symbols.begin(), std::find(long_symbols.begin(), long_symbols.end(), reel[symbol_pos - 1]));
            reel[symbol_pos] = long_symbols.at(index == long_symbols.size() - 1 ? 0 : index + 1);
        }
        else if (!long_symbols.empty())
        {
            reel[symbol_pos] = long_symbols.at(0);
        }
    }
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
        if (IsSymbolPartOfLongSymbol(symbol_id))
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

bool LongSymbolsNormalizer::IsSymbolPartOfLongSymbol(VCORE_Figure::Identity_t symbol_id) const
{
    for (const auto& pair : m_long_symbols) 
    {
        if (std::find(pair.second.begin(), pair.second.end(), symbol_id) != pair.second.end()) 
        {
            return true;
        }
    }
    return false;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetLongSymbol(VCORE_Figure::Identity_t symbol_id) const
{
    for (const auto& pair : m_long_symbols)
    {
        if (symbol_id == pair.first || std::find(pair.second.begin(), pair.second.end(), symbol_id) != pair.second.end())
        {
            return pair.second;
        }
    }

    return { };
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

bool LongSymbolsNormalizer::IsPrevSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos) const
{
    return pos > 0 && reel[pos] == reel[pos - 1];
}

bool LongSymbolsNormalizer::IsAdjacentSymbolPartOfSameLongSymbol(const VCORE_Reels::Reel_t& reel, size_t pos, int direction) const
{
    if (direction == NEXT_SYMBOL && pos + 1 >= reel.size()) 
        return false;

    if (direction == PREV_SYMBOL && pos == 0) 
        return false;

    const auto adjacent_pos = pos + direction;
    for (const auto& pair : m_long_symbols) 
    {
        const auto& long_symbol = pair.second;
        const auto current_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[pos]);
        if (current_symbol_it != long_symbol.end()) 
        {
            const auto adj_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[adjacent_pos]);
            return adj_symbol_it != long_symbol.end() && (adj_symbol_it - current_symbol_it) == direction;
        }
    }
    return false;
}

size_t LongSymbolsNormalizer::GetSymbolLength(const VCORE_Reels::Reel_t& reel, size_t start_pos) const
{
    size_t length = 1;
    while (IsNextSymbolSame(reel, start_pos) || IsAdjacentSymbolPartOfSameLongSymbol(reel, start_pos, 1))
    {
        ++length;
        ++start_pos;
    }
    return length;
}

size_t LongSymbolsNormalizer::GetPrevSymbolLength(const VCORE_Reels::Reel_t& reel, size_t start_pos) const
{
    size_t length = 1;
    while (IsPrevSymbolSame(reel, start_pos) || IsAdjacentSymbolPartOfSameLongSymbol(reel, start_pos, -1))
    {
        ++length;
        --start_pos;
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
        return IsLongSymbol(symbol_id) || IsSymbolPartOfLongSymbol(symbol_id);
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

VCORE_Figure::Identity_t LongSymbolsNormalizer::GetOriginalSymbolId(VCORE_Figure::Identity_t modified_symbol_id) const
{
    for (const auto& pair : m_long_symbols) 
    {
        if (std::find(pair.second.begin(), pair.second.end(), modified_symbol_id) != pair.second.end()) 
        {
            return pair.first;
        }
    }
    return modified_symbol_id;
}

bool LongSymbolsNormalizer::IsEqualAllId(const VCORE_Reels::Reel_t& reel) const
{
    if (reel.empty())
    {
        return true;
    }

    const auto original_reel = GetOriginalReel(reel);
    return std::all_of(cbegin(original_reel), cend(original_reel), [first_symbol_id = original_reel.front()](int symbol_id) { return symbol_id == first_symbol_id; });
}
