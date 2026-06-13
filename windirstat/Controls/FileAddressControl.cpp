#include "pch.h"
#include "ItemAddress.h"
#include "FileAddressControl.h"
#include "MapLoader.h"

CFileAddressControl::CFileAddressControl()
    : CTreeListControl(&m_columnOrderStorage, &m_columnWidthsStorage, LF_NONE, false)
{
    m_singleton = this;
}

BEGIN_MESSAGE_MAP(CFileAddressControl, CTreeListControl)
END_MESSAGE_MAP()

void CFileAddressControl::PopulateAddressList(CItem* root)
{
    struct AddrEntry
    {
        CItem* item;
        ULONGLONG address;
        ULONGLONG size;
        std::wstring section;
    };

    std::vector<AddrEntry> entries;
    std::vector<CItem*> stack;
    if (root != nullptr) stack.push_back(root);

    while (!stack.empty())
    {
        CItem* current = stack.back();
        stack.pop_back();

        if (current->IsLeaf())
        {
            if (current->GetSizeLogical() > 0)
            {
                const auto* details = GetMapItemDetails(current);
                entries.push_back({
                    current,
                    current->GetIndex(),
                    current->GetSizeLogical(),
                    details ? GetMapItemSectionName(current) : std::wstring{}
                });
            }
        }
        else
        {
            for (CItem* child : current->GetChildren())
            {
                stack.push_back(child);
            }
        }
    }

    // Sort by address ascending
    std::ranges::sort(entries, {}, &AddrEntry::address);

    // Clear list and create fresh root
    DeleteAllItems();
    delete m_rootItem;
    m_rootItem = new CItemAddress();
    InsertItem(0, m_rootItem);

    // Add items and compute gaps
    ULONGLONG prevEnd = 0;
    for (const auto& entry : entries)
    {
        const ULONGLONG gap = (entry.address > prevEnd && prevEnd > 0) ? entry.address - prevEnd : 0;
        auto* addrItem = new CItemAddress(entry.item, entry.address, entry.size, gap, entry.section);
        m_rootItem->AddAddressChild(addrItem);
        prevEnd = entry.address + entry.size;
    }

    // Force expand root so children become visible in the tree list
    ExpandItem(0);
    CTreeListControl::SortItems();
}

void CFileAddressControl::AfterDeleteAllItems()
{
    delete m_rootItem;
    m_rootItem = new CItemAddress();
    InsertItem(0, m_rootItem);
}
