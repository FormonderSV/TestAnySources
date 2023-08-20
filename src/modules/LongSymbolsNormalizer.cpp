#include <modules/LongSymbolsNormalizer.hpp>
#include <algorithm>
#include <cassert>
#include <random>

using namespace vcore;

LongSymbolsNormalizer::LongSymbolsNormalizer()
    : m_rd{}
    , m_gen{ m_rd() }
    , m_distributor{ 0, std::numeric_limits<int>::max() }
{

}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, bool use_random)
    : m_rd{}
    , m_gen{ m_rd() }
    , m_distributor{ 0, std::numeric_limits<int>::max() }
    , m_long_symbols{ long_symbols }
    , m_replace_symbols{}
    , m_use_random{ use_random }
{

}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, const VCORE_Reels::Reel_t& replace_symbols, bool use_random)
    : m_rd{}
    , m_gen{ m_rd() }
    , m_distributor{ 0, std::numeric_limits<int>::max() }
    , m_long_symbols{ long_symbols }
    , m_replace_symbols{ replace_symbols }
    , m_use_random{ use_random }
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

size_t LongSymbolsNormalizer::GetAdjacentIndex(const VCORE_Reels::Reel_t& adjacent_symbol, const VCORE_Reels::Reel_t& reel, size_t adj_pos) const
{
    return std::distance(adjacent_symbol.begin(), std::find(adjacent_symbol.begin(), adjacent_symbol.end(), reel[adj_pos]));
}

void LongSymbolsNormalizer::UpdateSymbolAtPosition(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos, bool is_next) const
{
    const auto adjacent_pos = is_next ? symbol_pos + 1 : symbol_pos - 1;
    const auto adjacent_symbol = GetLongSymbol(reel[adjacent_pos]);

    if (long_symbols.empty() && adjacent_symbol.empty())
    {
        return;
    }

    const size_t symbol_length = is_next ? GetSymbolLength(reel, adjacent_pos) : GetPrevSymbolLength(reel, adjacent_pos);

    if (!adjacent_symbol.empty())
    {
        if (adjacent_symbol.size() == symbol_length)
        {
            if (!long_symbols.empty())
            {
                reel[symbol_pos] = is_next ? long_symbols.back() : long_symbols.front();
            }
            return;
        }

        const size_t index = GetAdjacentIndex(adjacent_symbol, reel, adjacent_pos);

        if (adjacent_symbol.size() != symbol_length)
        {
            if (is_next)
            {
                reel[symbol_pos] = (index == 0) ? adjacent_symbol.back() : adjacent_symbol[index - 1];
            }
            else
            {
                reel[symbol_pos] = (index == adjacent_symbol.size() - 1) ? adjacent_symbol.front() : adjacent_symbol[index + 1];
            }
            return;
        }
    }

    if (long_symbols == adjacent_symbol)
    {
        const size_t index = GetAdjacentIndex(long_symbols, reel, adjacent_pos);
        reel[symbol_pos] = (is_next && index == 0) ? long_symbols.back() : long_symbols[index - 1 + is_next];
        return;
    }

    if (!long_symbols.empty())
    {
        reel[symbol_pos] = is_next ? long_symbols.back() : long_symbols.front();
    }
}

void LongSymbolsNormalizer::UpdateReelWithLongSymbols(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos) const
{
    if (symbol_pos == 0 && symbol_pos + 1 < reel.size())
    {
        UpdateSymbolAtPosition(reel, long_symbols, symbol_pos, true);
    }
    else if (symbol_pos == reel.size() - 1 && symbol_pos - 1 > 0)
    {
        UpdateSymbolAtPosition(reel, long_symbols, symbol_pos, false);
    }
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedFakeRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const
{
    // В данном коде сразу приходит модифицированная матрица, все длынные символы преобразованы в нужные ID
    // Данная роллинг матрица будет крутиться циклично начиная с последнего id до reel_contents.size(). Т.е. если длина роллинга 30 а reel_contents.size() == 6,
    // то для роллинга будут использоватьcя ID с 29 по 7. reel.size() - reel_contents.size();
    // Важно соблюдать цикличность в данном роллинге и не допустить разрыва длинных символов.
    // При старте роллинга берётся первый символ с reel_contents и соединяется с последним символом reel
    // reel_contents используется только вначале крутки, после прокрутки reel_contents.size() символов, reel_contents, более использоваться не будет.

    auto left_index = current_contents.size();
    auto current_figure_id = current_contents.front();

    VCORE_Reels::Reel_t new_reel = reel;
    return new_reel;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedTrueRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const
{
    VCORE_Reels::Reel_t new_reel = reel;
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

        int random_index = m_use_random ? GenerateRandomNumberWithinRange(long_symbol.size() - 1) : 0;

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

bool LongSymbolsNormalizer::IsAdjacentSymbolPartOfSameLongSymbol(const VCORE_Reels::Reel_t& reel, size_t pos, Direction_t direction) const
{
    if (!(direction == Direction_t::LEFT || direction == Direction_t::RIGHT))
        return false;

    if (direction == Direction_t::RIGHT && pos + 1 >= reel.size())
        return false;

    if (direction == Direction_t::LEFT && pos == 0)
        return false;

    const auto adjacent_pos = pos + static_cast<int>(direction);
    for (const auto& pair : m_long_symbols)
    {
        const auto& long_symbol = pair.second;
        const auto current_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[pos]);
        if (current_symbol_it != long_symbol.end())
        {
            const auto adj_symbol_it = std::find(long_symbol.begin(), long_symbol.end(), reel[adjacent_pos]);
            return adj_symbol_it != long_symbol.end() && (adj_symbol_it - current_symbol_it) == static_cast<int>(direction);
        }
    }
    return false;
}

size_t LongSymbolsNormalizer::GetSymbolLength(const VCORE_Reels::Reel_t& reel, size_t start_pos) const
{
    size_t length = 1;
    while (IsNextSymbolSame(reel, start_pos) || IsAdjacentSymbolPartOfSameLongSymbol(reel, start_pos, Direction_t::RIGHT))
    {
        ++length;
        ++start_pos;
    }
    return length;
}

size_t LongSymbolsNormalizer::GetPrevSymbolLength(const VCORE_Reels::Reel_t& reel, size_t start_pos) const
{
    size_t length = 1;
    while (IsPrevSymbolSame(reel, start_pos) || IsAdjacentSymbolPartOfSameLongSymbol(reel, start_pos, Direction_t::LEFT))
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

VCORE_Figure::Identity_t LongSymbolsNormalizer::GetRandomReplaceSymbol() const
{
    assert(m_replace_symbols.size() > 0);
    const auto index = GenerateRandomNumberWithinRange(m_replace_symbols.size() - 1);
    return m_replace_symbols[index];
}

int LongSymbolsNormalizer::GenerateRandomNumberWithinRange(size_t max_value) const
{
    m_distributor.param(std::uniform_int_distribution<int>::param_type(0, static_cast<int>(max_value)));
    return m_distributor(m_gen);
}
