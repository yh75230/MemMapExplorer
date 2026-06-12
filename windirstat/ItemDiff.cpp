#include "pch.h"
#include "ItemDiff.h"
#include "Item.h"

void CDiffRootItem::AddDiffChild(CDiffItem* child)
{
    m_children.push_back(child);
}

void CDiffRootItem::RemoveAllDiffChildren()
{
    for (auto* child : m_children)
    {
        delete child;
    }
    m_children.clear();
}

CDiffItem::CDiffItem(const DiffEntry& entry) : m_entry(entry)
{
}

CItem* CDiffItem::GetLinkedItem() noexcept
{
    return m_entry.newItem ? m_entry.newItem : m_entry.oldItem;
}

HICON CDiffItem::GetIcon()
{
    return nullptr;
}

std::wstring CDiffItem::FormatSize(ULONGLONG size) const
{
    if (size == 0) return L"-";

    constexpr ULONGLONG KB = 1024;
    constexpr ULONGLONG MB = KB * 1024;
    constexpr ULONGLONG GB = MB * 1024;

    if (size >= GB)
    {
        return std::format(L"{:.2f} GB", static_cast<double>(size) / GB);
    }
    if (size >= MB)
    {
        return std::format(L"{:.2f} MB", static_cast<double>(size) / MB);
    }
    if (size >= KB)
    {
        return std::format(L"{:.2f} KB", static_cast<double>(size) / KB);
    }
    return std::format(L"{} B", size);
}

COLORREF CDiffItem::GetTypeColor() const
{
    switch (m_entry.type)
    {
    case DiffType::Added:       return RGB(0, 160, 0);
    case DiffType::Removed:     return RGB(200, 0, 0);
    case DiffType::SizeChanged: return RGB(180, 120, 0);
    case DiffType::AddressChanged: return RGB(0, 100, 180);
    default:                    return RGB(128, 128, 128);
    }
}

std::wstring CDiffItem::GetText(int subitem) const
{
    switch (subitem)
    {
    case DCOL_DIFF_NAME:
        return m_entry.name;

    case DCOL_DIFF_TYPE:
        switch (m_entry.type)
        {
        case DiffType::Added:         return L"Added";
        case DiffType::Removed:       return L"Removed";
        case DiffType::SizeChanged:   return L"Size Changed";
        case DiffType::AddressChanged:return L"Addr Changed";
        case DiffType::Unchanged:     return L"Unchanged";
        default:                      return L"";
        }

    case DCOL_DIFF_OLD_SIZE:
        return FormatSize(m_entry.oldSize);

    case DCOL_DIFF_NEW_SIZE:
        return FormatSize(m_entry.newSize);

    case DCOL_DIFF_DELTA:
    {
        if (m_entry.type == DiffType::Added)
            return L"+" + FormatSize(m_entry.newSize);
        if (m_entry.type == DiffType::Removed)
            return L"-" + FormatSize(m_entry.oldSize);
        if (m_entry.newSize > m_entry.oldSize)
            return L"+" + FormatSize(m_entry.newSize - m_entry.oldSize);
        if (m_entry.newSize < m_entry.oldSize)
            return L"-" + FormatSize(m_entry.oldSize - m_entry.newSize);
        return L"0";
    }

    case DCOL_DIFF_PERCENT:
    {
        if (m_entry.oldSize == 0 && m_entry.newSize == 0) return L"-";
        if (m_entry.oldSize == 0) return L"+INF%";
        const double pct = (static_cast<double>(m_entry.newSize) - static_cast<double>(m_entry.oldSize)) / m_entry.oldSize * 100.0;
        if (pct >= 0) return std::format(L"+{:.1f}%", pct);
        return std::format(L"{:.1f}%", pct);
    }

    case DCOL_DIFF_SECTION:
        return m_entry.section;

    case DCOL_DIFF_LIBRARY:
        return m_entry.library;

    default:
        return L"";
    }
}

bool CDiffItem::DrawSubItem(int subitem, CDC* pdc, CRect rc, UINT state, int* width, int* focusLeft)
{
    if (subitem == DCOL_DIFF_TYPE || subitem == DCOL_DIFF_DELTA)
    {
        const COLORREF typeColor = GetTypeColor();
        pdc->SetTextColor(typeColor);
        const std::wstring text = GetText(subitem);
        pdc->DrawText(text.c_str(), static_cast<int>(text.size()), rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        return true;
    }

    if (subitem == DCOL_DIFF_PERCENT)
    {
        const std::wstring text = GetText(subitem);
        if (text.starts_with(L"+") && text != L"+INF%")
        {
            pdc->SetTextColor(RGB(0, 160, 0));
        }
        else if (text.starts_with(L"-") && text != L"-")
        {
            pdc->SetTextColor(RGB(200, 0, 0));
        }
        pdc->DrawText(text.c_str(), static_cast<int>(text.size()), rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
        return true;
    }

    return false;
}

int CDiffItem::CompareSibling(const CTreeListItem* tlib, int subitem) const
{
    const auto* other = static_cast<const CDiffItem*>(tlib);

    switch (subitem)
    {
    case DCOL_DIFF_NAME:
        return _wcsicmp(m_entry.name.c_str(), other->m_entry.name.c_str());

    case DCOL_DIFF_TYPE:
        return static_cast<int>(m_entry.type) - static_cast<int>(other->m_entry.type);

    case DCOL_DIFF_OLD_SIZE:
    case DCOL_DIFF_NEW_SIZE:
    {
        const ULONGLONG sizeA = (subitem == DCOL_DIFF_OLD_SIZE) ? m_entry.oldSize : m_entry.newSize;
        const ULONGLONG sizeB = (subitem == DCOL_DIFF_OLD_SIZE) ? other->m_entry.oldSize : other->m_entry.newSize;
        if (sizeA < sizeB) return -1;
        if (sizeA > sizeB) return 1;
        return 0;
    }

    case DCOL_DIFF_DELTA:
    {
        auto getDelta = [](const DiffEntry& e) -> LONGLONG
        {
            if (e.type == DiffType::Added) return static_cast<LONGLONG>(e.newSize);
            if (e.type == DiffType::Removed) return -static_cast<LONGLONG>(e.oldSize);
            return static_cast<LONGLONG>(e.newSize) - static_cast<LONGLONG>(e.oldSize);
        };
        const LONGLONG deltaA = getDelta(m_entry);
        const LONGLONG deltaB = getDelta(other->m_entry);
        if (deltaA < deltaB) return -1;
        if (deltaA > deltaB) return 1;
        return 0;
    }

    case DCOL_DIFF_PERCENT:
    {
        auto getPercent = [](const DiffEntry& e) -> double
        {
            if (e.oldSize == 0) return (e.newSize > 0) ? 1000.0 : 0.0;
            return (static_cast<double>(e.newSize) - static_cast<double>(e.oldSize)) / e.oldSize * 100.0;
        };
        const double pctA = getPercent(m_entry);
        const double pctB = getPercent(other->m_entry);
        if (pctA < pctB) return -1;
        if (pctA > pctB) return 1;
        return 0;
    }

    case DCOL_DIFF_SECTION:
        return _wcsicmp(m_entry.section.c_str(), other->m_entry.section.c_str());

    case DCOL_DIFF_LIBRARY:
        return _wcsicmp(m_entry.library.c_str(), other->m_entry.library.c_str());

    default:
        return 0;
    }
}
