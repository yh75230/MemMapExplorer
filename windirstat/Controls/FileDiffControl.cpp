#include "pch.h"
#include "ItemDiff.h"

CFileDiffControl::CFileDiffControl() : CTreeListControl(&m_columnOrderStorage, &m_columnWidthsStorage, LF_NONE, false)
{
    m_singleton = this;
}

BEGIN_MESSAGE_MAP(CFileDiffControl, CTreeListControl)
END_MESSAGE_MAP()

const DiffEntry* CFileDiffControl::GetDiffEntry(int index) const
{
    if (index < 0 || index >= static_cast<int>(m_diffEntries.size()))
    {
        return nullptr;
    }
    return &m_diffEntries[index];
}

void CFileDiffControl::LoadDiff(const std::vector<DiffEntry>& entries)
{
    m_diffEntries = entries;

    auto* root = new CDiffRootItem();

    for (const auto& entry : m_diffEntries)
    {
        if (entry.type == DiffType::Unchanged) continue;
        auto* diffItem = new CDiffItem(entry);
        root->AddDiffChild(diffItem);
    }

    SetRootItem(root);
    SortItems();
    Invalidate();
}

void CFileDiffControl::ClearDiff()
{
    m_diffEntries.clear();
    SetRootItem(nullptr);
    Invalidate();
}

void CFileDiffControl::AfterDeleteAllItems()
{
}
