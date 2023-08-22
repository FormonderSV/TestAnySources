#include <modules/LongSymbolsNormalizer.hpp>
#include <algorithm>
#include <cassert>
#include <random>

using namespace vcore;

LongSymbolsNormalizer::LongSymbolsNormalizer()
    : LongSymbolsNormalizer({}, {}, false)
{
}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, bool use_random_sequence)
    : LongSymbolsNormalizer(long_symbols, {}, use_random_sequence)
{
}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbols_t& long_symbols, const VCORE_Reels::Reel_t& replace_symbols, bool use_random_sequence)
    : m_rd{}
    , m_gen{ m_rd() }
    , m_distributor{ 0, std::numeric_limits<int>::max() }
    , m_long_symbols{ long_symbols }
    , m_replace_symbols{ replace_symbols }
    , m_use_random_sequence{ use_random_sequence }
{
}

LongSymbolsNormalizer::LongSymbolsNormalizer(const LongSymbolsNormalizer& rhs)
    : LongSymbolsNormalizer(rhs.m_long_symbols, rhs.m_replace_symbols, rhs.m_use_random_sequence)
{
    m_gen = rhs.m_gen;
    m_distributor = rhs.m_distributor;
}

LongSymbolsNormalizer& LongSymbolsNormalizer::operator=(const LongSymbolsNormalizer& rhs)
{
    if (this == &rhs)
    {
        return *this;
    }

    m_distributor = rhs.m_distributor;
    m_long_symbols = rhs.m_long_symbols;
    m_replace_symbols = rhs.m_replace_symbols;
    m_use_random_sequence = rhs.m_use_random_sequence;

    return *this;
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
    const auto long_symbols = GetLongSymbolFor(new_reel[symbol_pos]);
    UpdateReelWithLongSymbols(new_reel, long_symbols, symbol_pos);
}

size_t LongSymbolsNormalizer::GetAdjacentIndex(const VCORE_Reels::Reel_t& long_symbol, const VCORE_Reels::Reel_t& reel, size_t adj_pos) const
{
    return std::distance(long_symbol.begin(), std::find(long_symbol.begin(), long_symbol.end(), reel[adj_pos]));
}

size_t LongSymbolsNormalizer::GetLongSymbolIndex(const VCORE_Reels::Reel_t& long_symbol, VCORE_Figure::Identity_t symbol_id) const
{
    return std::distance(long_symbol.begin(), std::find(long_symbol.begin(), long_symbol.end(), symbol_id));
}

void LongSymbolsNormalizer::UpdateSymbolAtPosition(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos, bool is_next) const
{
    const auto adjacent_pos = is_next ? symbol_pos + 1 : symbol_pos - 1;
    const auto adjacent_long_symbols = GetLongSymbolFor(reel[adjacent_pos]);

    if (long_symbols.empty() && adjacent_long_symbols.empty())
    {
        return;
    }

    const size_t symbol_length = is_next ? GetSymbolLength(reel, adjacent_pos) : GetPrevSymbolLength(reel, adjacent_pos);

    if (!adjacent_long_symbols.empty())
    {
        if (adjacent_long_symbols.size() == symbol_length)
        {
            if (!long_symbols.empty())
            {
                reel[symbol_pos] = is_next ? long_symbols.back() : long_symbols.front();
            }
            return;
        }

        const size_t index = GetAdjacentIndex(adjacent_long_symbols, reel, adjacent_pos);

        if (adjacent_long_symbols.size() != symbol_length)
        {
            if (is_next)
            {
                reel[symbol_pos] = (index == 0) ? adjacent_long_symbols.back() : adjacent_long_symbols[index - 1];
            }
            else
            {
                reel[symbol_pos] = (index == adjacent_long_symbols.size() - 1) ? adjacent_long_symbols.front() : adjacent_long_symbols[index + 1];
            }
            return;
        }
    }

    if (long_symbols == adjacent_long_symbols)
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

void LongSymbolsNormalizer::UpdateReelForFrontSymbol(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& current_contents, size_t& left_index, size_t& right_index) const
{
    const auto& long_symbol = GetLongSymbolFor(current_contents.front());
    const auto symbol_length = GetSymbolLength(current_contents, 0);
    const auto index = GetLongSymbolIndex(long_symbol, current_contents.front());

    if (symbol_length == long_symbol.size())
    {
        return;
    }

    for (int i = static_cast<int>(index) - 1; i >= 0; --i)
    {
        new_reel[right_index] = long_symbol[i];
        --right_index;
    }

    for (int i = static_cast<int>(index); i < static_cast<int>(long_symbol.size()); ++i)
    {
        new_reel[left_index] = long_symbol[i];
        ++left_index;
    }
}

void LongSymbolsNormalizer::UpdateReelForOtherSymbols(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& reel, size_t& left_index, size_t right_index) const
{
    const auto& long_symbol = GetLongSymbolFor(reel[left_index]);
    const auto symbol_length = GetSymbolLength(new_reel, left_index);
    const auto half_long_symbol_size = long_symbol.size() / 2;

    if (symbol_length < half_long_symbol_size)
    {
        ReplaceWithRandomSymbols(new_reel, left_index, right_index, symbol_length);
    }
    else
    {
        ReplaceOrExtendLongSymbol(new_reel, long_symbol, left_index, right_index);
    }
}

void LongSymbolsNormalizer::ReplaceWithRandomSymbols(VCORE_Reels::Reel_t& new_reel, size_t& left_index, size_t right_index, size_t symbol_length) const
{
    const size_t end_pos = left_index + symbol_length;
    for (; left_index <= right_index && left_index < end_pos; ++left_index)
    {
        new_reel[left_index] = GetRandomReplaceSymbol();
    }
}

void LongSymbolsNormalizer::ReplaceOrExtendLongSymbol(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& long_symbol, size_t& left_index, size_t right_index) const
{
    if (right_index - left_index + 1 >= long_symbol.size())
    {
        for (const auto symbol_id : long_symbol)
        {
            new_reel[left_index] = symbol_id;
            ++left_index;
        }
    }
    else
    {
        for (; left_index <= right_index; ++left_index)
        {
            new_reel[left_index] = GetRandomReplaceSymbol();
        }
    }
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedFakeRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const
{
    VCORE_Reels::Reel_t new_reel = reel;
    size_t left_index = current_contents.size() + 1;
    size_t right_index = reel.size() - 1;

    if (IsPartOfLongSymbol(current_contents.front()))
    {
        UpdateReelForFrontSymbol(new_reel, current_contents, left_index, right_index);
    }

    while (left_index <= right_index)
    {
        if (IsPartOfLongSymbol(reel[left_index]))
        {
            UpdateReelForOtherSymbols(new_reel, reel, left_index, right_index);
        }
        else
        {
            left_index++;
        }
    }

    return new_reel;
}


VCORE_Reels::Reel_t LongSymbolsNormalizer::GetModifiedTrueRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const
{
    VCORE_Reels::Reel_t new_reel = reel;

    auto left_index = current_contents.size() - 2;
    auto right_index = reel.size() - 1;

    if (IsPartOfLongSymbol(current_contents.front()))
    {
        const auto& long_symbol = GetLongSymbolFor(current_contents.front());
        const auto symbol_length = GetSymbolLength(current_contents, 0);

        if (symbol_length != long_symbol.size())
        {
            const auto index = GetLongSymbolIndex(long_symbol, current_contents.front());

            for (int i = static_cast<int>(index) - 1; i >= 0; --i)
            {
                new_reel[right_index] = long_symbol[i];
                --right_index;
            }
        }
    }

    if (IsPartOfLongSymbol(new_reel[left_index]))
    {
        const auto& long_symbol = GetLongSymbolFor(new_reel[left_index]);
        const auto symbol_length = GetSymbolLength(new_reel, left_index);

        if (symbol_length != long_symbol.size())
        {
            const auto index = GetLongSymbolIndex(long_symbol, new_reel[left_index]);

            for (int i = static_cast<int>(index); i < static_cast<int>(long_symbol.size()); ++i)
            {
                new_reel[left_index] = long_symbol[i];
                ++left_index;
            }
        }
    }

    while (left_index <= right_index)
    {
        if (IsPartOfLongSymbol(new_reel[left_index]))
        {
            const auto& long_symbol = GetLongSymbolFor(reel[left_index]);
            const auto symbol_length = GetSymbolLength(new_reel, left_index);
            const auto& half_long_symbol_size = long_symbol.size() / 2;

            if (symbol_length < half_long_symbol_size)
            {
                const auto end_pos = left_index + symbol_length;

                for (; left_index <= right_index && left_index < end_pos; ++left_index)
                {
                    new_reel[left_index] = GetRandomReplaceSymbol();
                }
            }
            else if (symbol_length != long_symbol.size())
            {
                if (right_index - left_index > long_symbol.size())
                {
                    for (int i = 0; i < static_cast<int>(long_symbol.size()); ++i)
                    {
                        new_reel[left_index] = long_symbol[i];
                        ++left_index;
                    }
                }
                else
                {
                    for (; left_index <= right_index; ++left_index)
                    {
                        new_reel[left_index] = GetRandomReplaceSymbol();
                    }
                }
            }
            else if (symbol_length == long_symbol.size())
            {
                left_index += symbol_length;
            }
            else
            {
                ++left_index;
            }
        }
        else
        {
            ++left_index;
        }
    }

    const auto& long_symbol = GetLongSymbolFor(new_reel[1]);
    const auto symbol_length = GetSymbolLength(new_reel, 1);

    if (IsPartOfLongSymbol(new_reel[1]) && long_symbol.size() != symbol_length)
    {
        if (!IsAdjacentSymbolPartOfSameLongSymbol(new_reel, 1, Direction_t::LEFT))
        {
            const auto index = GetLongSymbolIndex(long_symbol, new_reel[1]);
            new_reel[0] = (index == 0) ? long_symbol.back() : long_symbol[index - 1];
        }
    }
    else if (IsPartOfLongSymbol(new_reel[0]))
    {
        const auto& first_long_symbol = GetLongSymbolFor(new_reel[0]);
        new_reel[0] = first_long_symbol[first_long_symbol.size() - 1];
    }

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

bool LongSymbolsNormalizer::IsPartOfLongSymbol(VCORE_Figure::Identity_t symbol_id) const
{
    for (const auto& pair : m_long_symbols)
    {
        if (symbol_id == pair.first || std::find(pair.second.begin(), pair.second.end(), symbol_id) != pair.second.end())
        {
            return true;
        }
    }

    return false;
}

VCORE_Reels::Reel_t LongSymbolsNormalizer::GetLongSymbolFor(VCORE_Figure::Identity_t symbol_id) const
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

        int random_index = m_use_random_sequence ? GenerateRandomNumberWithinRange(long_symbol.size() - 1) : 0;

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

    if (IsPartOfLongSymbol(original_reel.back()))
    {
        ProcessReelStart(new_reel);
    }
    else
    {
        ProcessReelEnd(new_reel);
    }

    return new_reel;
}

void LongSymbolsNormalizer::ProcessReelStart(VCORE_Reels::Reel_t& reel) const
{
    for (size_t symbol_pos = 0; symbol_pos < reel.size();)
    {
        auto original_symbol = reel[symbol_pos];

        if (IsLongSymbol(original_symbol))
        {
            const size_t symbol_length = GetSymbolLength(reel, symbol_pos);
            size_t start_index = symbol_pos == 0 ? std::max(0, static_cast<int>(m_long_symbols.at(original_symbol).size() - symbol_length)) : 0;

            for (size_t count = 0; count < symbol_length && symbol_pos < reel.size() && start_index < m_long_symbols.at(original_symbol).size(); ++symbol_pos, ++start_index, ++count)
            {
                reel[symbol_pos] = m_long_symbols.at(original_symbol).at(start_index);
            }
        }
        else
        {
            ++symbol_pos;
        }
    }
}

void LongSymbolsNormalizer::ProcessReelEnd(VCORE_Reels::Reel_t& reel) const
{
    for (size_t symbol_pos = reel.size() - 1; symbol_pos < reel.size();)
    {
        auto original_symbol = reel[symbol_pos];

        if (IsLongSymbol(original_symbol))
        {
            const size_t symbol_length = GetPrevSymbolLength(reel, symbol_pos);
            size_t start_index = m_long_symbols.at(original_symbol).size() - 1;

            for (size_t count = 0; count < symbol_length && symbol_pos < reel.size(); --symbol_pos, ++count)
            {
                reel[symbol_pos] = m_long_symbols.at(original_symbol).at(start_index);
                start_index = start_index == 0 ? m_long_symbols.at(original_symbol).size() - 1 : start_index - 1;
            }

            symbol_pos -= symbol_length - 1;
        }
        else
        {
            if (symbol_pos == 0)
            {
                break;
            }

            --symbol_pos;
        }
    }
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

bool LongSymbolsNormalizer::IsUseRandomSequence() const
{
    return m_use_random_sequence;
}
