#pragma once

#include <unordered_map>
#include <vector>
#include <list>

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

    class LongSymbolsNormalizer
    {
    public:
        LongSymbolsNormalizer() = default;

        explicit LongSymbolsNormalizer(const LongSymbols_t& long_symbols);

        // Modify Reels
        VCORE_Reels GetModifiedReels(const VCORE_Reels& original_reels) const;

        VCORE_Reels::Matrix_t GetModifiedMatrix(const VCORE_Reels::Matrix_t& original_matrix) const;

        VCORE_Reels::Reel_t GetModifiedReel(const VCORE_Reels::Reel_t& original_reel) const;

        // Restore original reels
        VCORE_Reels GetOriginalReels(const VCORE_Reels& modified_reels) const;

        VCORE_Reels::Matrix_t GetOriginalMatrix(const VCORE_Reels::Matrix_t& modified_matrix) const;

        VCORE_Reels::Reel_t GetOriginalReel(const VCORE_Reels::Reel_t& modified_reel) const;

        // Other data
        void SetLongSymbols(const LongSymbols_t& long_symbols);

        const LongSymbols_t& GetLongSymbols() const;

        VCORE_Game::Figures_t GetAdditionalPayTableSymbols(const VCORE_Game::Figures_t& current_symbols) const;

    protected:
        VCORE_Reels::Reel_t GenerateRandomReel(int symbol_id, size_t new_size) const;

        bool IsNextSymbolSame(const VCORE_Reels::Reel_t& reel, size_t pos) const;

        size_t GetSymbolLength(const VCORE_Reels::Reel_t& reel, size_t startPos) const;

        VCORE_Reels::Reel_t GenerateReelWithLongSymbols(const VCORE_Reels::Reel_t& original_reel) const;

        bool HasLongSymbolOnReel(const VCORE_Reels::Reel_t& reel) const;

        bool IsLongSymbol(VCORE_Figure::Identity_t symbol_id) const;

        bool IsPartOfLongSymbol(VCORE_Figure::Identity_t symbol_id) const;

        VCORE_Figure::Identity_t GetOriginalSymbolId(VCORE_Figure::Identity_t modified_symbol_id) const;

    private:
        LongSymbols_t m_long_symbols;
        static constexpr int DEFAULT_REEL_VALUE = -1;
    };
}
