#pragma once

#include <unordered_map>
#include <vector>
#include <list>
#include <random>

namespace vcore
{
    class VCORE_Figure
    {
    public:
        using Identity_t = int;
        using Paytable_t = std::vector<unsigned int>;
        using Mask_t = int;

        enum {
            mask = 0
        };

        VCORE_Figure() = default;

        VCORE_Figure(Identity_t figure_identity, Paytable_t const& paytable, Mask_t mask)
            : m_identity(figure_identity)
            , m_paytable(paytable)
            , m_mask(mask) { }

        VCORE_Figure(VCORE_Figure const& figure)
            : m_identity(figure.GetIdentity())
            , m_paytable(figure.GetPayTable())
            , m_mask(figure.GetMask()) { };

        Identity_t GetIdentity() const { return m_identity; };
        Paytable_t::value_type GetPayment(Paytable_t::size_type count) const { return 1; };
        Paytable_t::size_type GetPayTableSize() const { return m_paytable.size(); };
        const Paytable_t& GetPayTable() const { return m_paytable; };
        Mask_t GetMask() const { return m_mask; };
    private:
        Identity_t m_identity;
        Paytable_t m_paytable;
        Mask_t m_mask;
    };

    class VCORE_Reels
    {
    public:
        using Reel_t = std::vector<VCORE_Figure::Identity_t>;
        using Matrix_t = std::vector<Reel_t>;

        VCORE_Reels() = default;
        VCORE_Reels(const Matrix_t& matrix, const Matrix_t& rolling_matrix)
            : m_Matrix(matrix)
            , m_RollingMatrix(rolling_matrix) {};

        Matrix_t const& GetMatrix() const { return m_Matrix; };
        Matrix_t const& GetRollingMatrix() const { return m_RollingMatrix; };;

    private:
        Matrix_t m_Matrix;
        Matrix_t m_RollingMatrix;
    };

    class VCORE_Game
    {
    public:
        using Figures_t = std::list<VCORE_Figure>;
    };

    using LongSymbols_t = std::unordered_map<int, VCORE_Reels::Reel_t>;

    enum class Direction_t
    {
        LEFT = -1,
        RIGHT = 1,
    };

    class LongSymbolsNormalizer
    {
    public:
        LongSymbolsNormalizer();

        explicit LongSymbolsNormalizer(const LongSymbols_t& long_symbols, bool use_random_sequence = false);

        explicit LongSymbolsNormalizer(const LongSymbols_t& long_symbols, const VCORE_Reels::Reel_t& replace_symbols, bool use_random_sequence = false);

        LongSymbolsNormalizer(const LongSymbolsNormalizer& rhs);

        LongSymbolsNormalizer& operator=(const LongSymbolsNormalizer& rhs);

        // Modify Reels
        VCORE_Reels GetModifiedReels(const VCORE_Reels& original_reels) const;

        VCORE_Reels::Matrix_t GetModifiedMatrix(const VCORE_Reels::Matrix_t& original_matrix) const;

        VCORE_Reels::Reel_t GetModifiedReel(const VCORE_Reels::Reel_t& original_reel) const;

        VCORE_Reels::Reel_t GetModifiedContents(const VCORE_Reels::Reel_t& reel_contents) const;

        void HandleSymbolModification(VCORE_Reels::Reel_t& new_reel, size_t symbol_pos) const;

        size_t GetAdjacentIndex(const VCORE_Reels::Reel_t& long_symbol, const VCORE_Reels::Reel_t& reel, size_t adj_pos) const;

        size_t GetLongSymbolIndex(const VCORE_Reels::Reel_t& long_symbol, VCORE_Figure::Identity_t symbol_id) const;

        void UpdateSymbolAtPosition(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos, bool is_next) const;

        void UpdateReelWithLongSymbols(VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& long_symbols, size_t symbol_pos) const;

        void UpdateReelForFrontSymbol(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& current_contents, size_t& left_index, size_t& right_index) const;

        void UpdateReelForOtherSymbols(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& reel, size_t& left_index, size_t right_index) const;

        void ReplaceWithRandomSymbols(VCORE_Reels::Reel_t& new_reel, size_t& left_index, size_t right_index, size_t symbol_length) const;

        void ReplaceOrExtendLongSymbol(VCORE_Reels::Reel_t& new_reel, const VCORE_Reels::Reel_t& long_symbol, size_t& left_index, size_t right_index) const;

        VCORE_Reels::Reel_t GetModifiedFakeRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const;

        VCORE_Reels::Reel_t GetModifiedTrueRolling(const VCORE_Reels::Reel_t& reel, const VCORE_Reels::Reel_t& current_contents) const;

        // Restore original reels
        VCORE_Reels GetOriginalReels(const VCORE_Reels& modified_reels) const;

        VCORE_Reels::Matrix_t GetOriginalMatrix(const VCORE_Reels::Matrix_t& modified_matrix) const;

        VCORE_Reels::Reel_t GetOriginalReel(const VCORE_Reels::Reel_t& modified_reel) const;

        // Other data
        void SetLongSymbols(const LongSymbols_t& long_symbols);

        void SetSymbolsForReplace(const VCORE_Reels::Reel_t& replace_symbols);

        const LongSymbols_t& GetLongSymbols() const;

        const VCORE_Reels::Reel_t& GetReplaceSymbols() const;

        VCORE_Game::Figures_t GetAdditionalPayTableSymbols(const VCORE_Game::Figures_t& current_symbols) const;

        bool IsPartOfLongSymbol(VCORE_Figure::Identity_t symbol_id) const;

        VCORE_Reels::Reel_t GetLongSymbolFor(VCORE_Figure::Identity_t symbol_id) const;

    protected:

        VCORE_Reels::Reel_t GenerateRandomReel(int symbol_id, size_t new_size) const;

        bool IsNextSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos) const;

        bool IsPrevSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos) const;

        bool IsAdjacentSymbolPartOfSameLongSymbol(const VCORE_Reels::Reel_t& reel, size_t pos, Direction_t direction) const;

        VCORE_Figure::Identity_t GetAdjacentSymbol(const VCORE_Reels::Reel_t& reel, size_t pos, Direction_t direction) const;

        bool IsAdjacentSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos, Direction_t direction) const;

        size_t GetSymbolLength(const VCORE_Reels::Reel_t& reel, size_t pos, Direction_t direction = Direction_t::RIGHT) const;

        VCORE_Reels::Reel_t GenerateReelWithLongSymbols(const VCORE_Reels::Reel_t& original_reel) const;

        void ProcessReelStart(VCORE_Reels::Reel_t& reel) const;

        void ProcessReelEnd(VCORE_Reels::Reel_t& reel) const;

        bool HasLongSymbolOnReel(const VCORE_Reels::Reel_t& reel) const;

        bool IsLongSymbolDefined(VCORE_Figure::Identity_t symbol_id) const;

        bool IsCompleteSymbol(VCORE_Figure::Identity_t symbol_id, size_t length) const;

        VCORE_Figure::Identity_t GetOriginalSymbolId(VCORE_Figure::Identity_t modified_symbol_id) const;

        bool IsEqualAllId(const VCORE_Reels::Reel_t& reel) const;

        int GenerateRandomNumber(size_t max_value, const VCORE_Reels::Reel_t& symbols = {}) const;

        bool UsesRandomSequence() const;

    private:
        mutable std::random_device m_rd;
        mutable std::mt19937 m_gen;
        mutable std::uniform_int_distribution<int> m_distributor;
        LongSymbols_t m_long_symbols;
        VCORE_Reels::Reel_t m_replace_symbols{};
        bool m_use_random_sequence{ false };
        static constexpr int DEFAULT_REEL_VALUE = -1;
    };
}
