#pragma once

#include "pch.h"
#include "DiffEngine.h"
#include "TreeListControl.h"

class CFileDiffControl final : public CTreeListControl
{
public:
    CFileDiffControl();
    ~CFileDiffControl() override { m_singleton = nullptr; }
    bool GetAscendingDefault(int column) override { return false; }
    static CFileDiffControl* Get() { return m_singleton; }

    void LoadDiff(const std::vector<DiffEntry>& entries);
    void ClearDiff();
    int GetDiffItemCount() const { return static_cast<int>(m_diffEntries.size()); }
    const DiffEntry* GetDiffEntry(int index) const;

    void AfterDeleteAllItems() override;

protected:
    enum DiffColumns : int
    {
        DCOL_NAME,
        DCOL_TYPE,
        DCOL_OLD_SIZE,
        DCOL_NEW_SIZE,
        DCOL_DELTA,
        DCOL_PERCENT,
        DCOL_SECTION,
        DCOL_LIBRARY,
    };

    inline static CFileDiffControl* m_singleton = nullptr;
    std::vector<DiffEntry> m_diffEntries;
    std::vector<int> m_columnOrderStorage;
    std::vector<int> m_columnWidthsStorage;

    DECLARE_MESSAGE_MAP()
};
