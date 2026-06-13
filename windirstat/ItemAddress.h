#pragma once

#include "pch.h"
#include "TreeListControl.h"

using ITEMADDRESSCOLUMNS = enum : std::uint8_t
{
    COL_ITEMADDR_NAME,
    COL_ITEMADDR_ADDRESS,
    COL_ITEMADDR_SIZE,
    COL_ITEMADDR_SECTION,
    COL_ITEMADDR_GAP,
};

class CItemAddress final : public CTreeListItem
{
    std::shared_mutex m_protect;
    std::vector<CItemAddress*> m_children;
    CItem* m_item = nullptr;
    ULONGLONG m_address = 0;
    ULONGLONG m_size = 0;
    ULONGLONG m_gap = 0;
    std::wstring m_section;

public:
    CItemAddress() = default;
    CItemAddress(CItem* item, ULONGLONG address, ULONGLONG size, ULONGLONG gap, std::wstring section);
    ~CItemAddress() override;

    bool DrawSubItem(int subitem, CDC* pdc, CRect rc, UINT state, int* width, int* focusLeft) override;
    std::wstring GetText(int subitem) const override;
    int CompareSibling(const CTreeListItem* tlib, int subitem) const override;
    int GetTreeListChildCount() const override { return static_cast<int>(m_children.size()); }
    CTreeListItem* GetTreeListChild(int i) const override { return m_children[i]; }
    HICON GetIcon() override;
    CItem* GetLinkedItem() noexcept override { return m_item; }
    CTreeListItem* GetAncestorCheckItem() override { return m_item; }

    void AddAddressChild(CItemAddress* child);
};
