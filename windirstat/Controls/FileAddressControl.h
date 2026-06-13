#pragma once

#include "pch.h"
#include "ItemAddress.h"
#include "TreeListControl.h"

class CFileAddressControl final : public CTreeListControl
{
public:
    CFileAddressControl();
    ~CFileAddressControl() override { m_singleton = nullptr; }
    bool GetAscendingDefault(int column) override { return column == COL_ITEMADDR_ADDRESS; }
    static CFileAddressControl* Get() { return m_singleton; }
    CItemAddress* GetRootItem() const { return m_rootItem; }
    void PopulateAddressList(CItem* root);
    void AfterDeleteAllItems() override;

protected:
    inline static CFileAddressControl* m_singleton = nullptr;
    CItemAddress* m_rootItem = nullptr;
    std::vector<int> m_columnOrderStorage;
    std::vector<int> m_columnWidthsStorage;

    DECLARE_MESSAGE_MAP()
};
