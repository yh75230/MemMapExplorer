#pragma once

#include "pch.h"
#include "TreeListControl.h"
#include "DiffEngine.h"

enum DIFFCOLUMNS : std::uint8_t
{
    DCOL_DIFF_NAME,
    DCOL_DIFF_TYPE,
    DCOL_DIFF_OLD_SIZE,
    DCOL_DIFF_NEW_SIZE,
    DCOL_DIFF_DELTA,
    DCOL_DIFF_PERCENT,
    DCOL_DIFF_SECTION,
    DCOL_DIFF_LIBRARY,
};

class CDiffItem;

class CDiffRootItem final : public CTreeListItem
{
public:
    void AddDiffChild(CDiffItem* child);
    void RemoveAllDiffChildren();

    int GetTreeListChildCount() const override { return static_cast<int>(m_children.size()); }
    CTreeListItem* GetTreeListChild(int i) const override { return m_children[i]; }
    std::wstring GetText(int) const override { return L""; }
    bool DrawSubItem(int, CDC*, CRect, UINT, int*, int*) override { return false; }
    int CompareSibling(const CTreeListItem*, int) const override { return 0; }
    HICON GetIcon() override { return nullptr; }

private:
    std::vector<CTreeListItem*> m_children;
};

class CDiffItem final : public CTreeListItem
{
public:
    CDiffItem() = default;
    CDiffItem(const DiffEntry& entry);

    bool DrawSubItem(int subitem, CDC* pdc, CRect rc, UINT state, int* width, int* focusLeft) override;
    std::wstring GetText(int subitem) const override;
    int CompareSibling(const CTreeListItem* tlib, int subitem) const override;
    int GetTreeListChildCount() const override { return 0; }
    CTreeListItem* GetTreeListChild(int) const override { return nullptr; }
    HICON GetIcon() override;
    CItem* GetLinkedItem() noexcept override;

    const DiffEntry& GetDiffEntry() const { return m_entry; }

private:
    DiffEntry m_entry;

    std::wstring FormatSize(ULONGLONG size) const;
    COLORREF GetTypeColor() const;
};
