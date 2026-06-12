#include "pch.h"
#include "DiffEngine.h"
#include "Item.h"
#include "MapLoader.h"

namespace
{
    void IndexTreeByName(CItem* item, std::unordered_map<std::wstring, CItem*>& index, int depth)
    {
        if (!item) return;

        const std::wstring name = item->GetName();
        if (!name.empty())
        {
            index[name] = item;
        }

        if (!item->IsLeaf())
        {
            for (CItem* child : item->GetChildren())
            {
                IndexTreeByName(child, index, depth + 1);
            }
        }
    }

    void CollectAllItems(CItem* item, std::vector<CItem*>& items)
    {
        if (!item) return;
        items.push_back(item);
        if (!item->IsLeaf())
        {
            for (CItem* child : item->GetChildren())
            {
                CollectAllItems(child, items);
            }
        }
    }

    std::wstring GetSymbolName(CItem* item)
    {
        const MapItemDetails* details = GetMapItemDetails(item);
        if (details && !details->title.empty())
        {
            return details->title;
        }
        return item->GetName();
    }

    bool IsMapAnalysis(CItem* root)
    {
        return GetMapItemDetails(root) != nullptr;
    }
}

std::vector<DiffEntry> CDiffEngine::ComputeDiff(CItem* oldRoot, CItem* newRoot)
{
    std::vector<DiffEntry> results;

    if (!oldRoot && !newRoot) return results;
    if (!oldRoot || !newRoot) return results;

    const bool mapMode = IsMapAnalysis(oldRoot) || IsMapAnalysis(newRoot);

    if (mapMode)
    {
        std::vector<CItem*> oldItems;
        std::vector<CItem*> newItems;
        CollectAllItems(oldRoot, oldItems);
        CollectAllItems(newRoot, newItems);

        std::unordered_map<std::wstring, CItem*> oldIndex;
        std::unordered_map<std::wstring, CItem*> newIndex;

        for (CItem* item : oldItems)
        {
            const std::wstring name = GetSymbolName(item);
            if (!name.empty())
            {
                oldIndex[name] = item;
            }
        }

        for (CItem* item : newItems)
        {
            const std::wstring name = GetSymbolName(item);
            if (!name.empty())
            {
                newIndex[name] = item;
            }
        }

        for (const auto& [name, oldItem] : oldIndex)
        {
            DiffEntry entry;
            entry.name = name;
            entry.oldItem = oldItem;
            entry.oldSize = oldItem->GetSizeLogical();
            entry.oldAddress = oldItem->GetIndex();

            const MapItemDetails* details = GetMapItemDetails(oldItem);
            if (details)
            {
                entry.section = GetMapItemSectionName(oldItem);
                for (const auto& field : details->fields)
                {
                    if (field.label == L"Library") entry.library = field.value;
                    else if (field.label == L"Object") entry.objectPath = field.value;
                }
            }

            auto it = newIndex.find(name);
            if (it != newIndex.end())
            {
                entry.newItem = it->second;
                entry.newSize = it->second->GetSizeLogical();
                entry.newAddress = it->second->GetIndex();

                if (entry.oldSize != entry.newSize)
                {
                    entry.type = DiffType::SizeChanged;
                }
                else if (entry.oldAddress != entry.newAddress)
                {
                    entry.type = DiffType::AddressChanged;
                }
                else
                {
                    entry.type = DiffType::Unchanged;
                }
            }
            else
            {
                entry.type = DiffType::Removed;
            }

            results.push_back(std::move(entry));
        }

        for (const auto& [name, newItem] : newIndex)
        {
            if (oldIndex.find(name) == oldIndex.end())
            {
                DiffEntry entry;
                entry.name = name;
                entry.newItem = newItem;
                entry.newSize = newItem->GetSizeLogical();
                entry.newAddress = newItem->GetIndex();
                entry.type = DiffType::Added;

                const MapItemDetails* details = GetMapItemDetails(newItem);
                if (details)
                {
                    entry.section = GetMapItemSectionName(newItem);
                    for (const auto& field : details->fields)
                    {
                        if (field.label == L"Library") entry.library = field.value;
                        else if (field.label == L"Object") entry.objectPath = field.value;
                    }
                }

                results.push_back(std::move(entry));
            }
        }
    }
    else
    {
        std::vector<CItem*> oldItems;
        std::vector<CItem*> newItems;
        CollectAllItems(oldRoot, oldItems);
        CollectAllItems(newRoot, newItems);

        std::unordered_map<std::wstring, CItem*> oldIndex;
        std::unordered_map<std::wstring, CItem*> newIndex;

        for (CItem* item : oldItems)
        {
            const std::wstring path = item->GetPath();
            if (!path.empty())
            {
                oldIndex[path] = item;
            }
        }

        for (CItem* item : newItems)
        {
            const std::wstring path = item->GetPath();
            if (!path.empty())
            {
                newIndex[path] = item;
            }
        }

        for (const auto& [path, oldItem] : oldIndex)
        {
            DiffEntry entry;
            entry.name = oldItem->GetName();
            entry.oldItem = oldItem;
            entry.oldSize = oldItem->GetSizeLogical();

            auto it = newIndex.find(path);
            if (it != newIndex.end())
            {
                entry.newItem = it->second;
                entry.newSize = it->second->GetSizeLogical();

                if (entry.oldSize != entry.newSize)
                {
                    entry.type = DiffType::SizeChanged;
                }
                else
                {
                    entry.type = DiffType::Unchanged;
                }
            }
            else
            {
                entry.type = DiffType::Removed;
            }

            results.push_back(std::move(entry));
        }

        for (const auto& [path, newItem] : newIndex)
        {
            if (oldIndex.find(path) == oldIndex.end())
            {
                DiffEntry entry;
                entry.name = newItem->GetName();
                entry.newItem = newItem;
                entry.newSize = newItem->GetSizeLogical();
                entry.type = DiffType::Added;
                results.push_back(std::move(entry));
            }
        }
    }

    std::sort(results.begin(), results.end(), [](const DiffEntry& a, const DiffEntry& b)
    {
        if (a.type != b.type)
        {
            if (a.type == DiffType::Added) return false;
            if (b.type == DiffType::Added) return true;
            if (a.type == DiffType::Removed) return false;
            if (b.type == DiffType::Removed) return true;
        }
        const ULONGLONG sizeA = a.newSize > a.oldSize ? a.newSize : a.oldSize;
        const ULONGLONG sizeB = b.newSize > b.oldSize ? b.newSize : b.oldSize;
        return sizeA > sizeB;
    });

    return results;
}

DiffSummary CDiffEngine::ComputeSummary(const std::vector<DiffEntry>& entries)
{
    DiffSummary summary;

    for (const auto& entry : entries)
    {
        if (entry.oldSize > 0) summary.totalOldSize += entry.oldSize;
        if (entry.newSize > 0) summary.totalNewSize += entry.newSize;

        switch (entry.type)
        {
        case DiffType::Added:
            summary.addedCount++;
            summary.addedBytes += entry.newSize;
            break;
        case DiffType::Removed:
            summary.removedCount++;
            summary.removedBytes += entry.oldSize;
            break;
        case DiffType::SizeChanged:
        case DiffType::AddressChanged:
            summary.sizeChangedCount++;
            if (entry.newSize > entry.oldSize)
            {
                summary.increasedBytes += (entry.newSize - entry.oldSize);
            }
            else
            {
                summary.decreasedBytes += (entry.oldSize - entry.newSize);
            }
            break;
        case DiffType::Unchanged:
            summary.unchangedCount++;
            break;
        }
    }

    return summary;
}

std::vector<DiffEntry*> CDiffEngine::BuildDiffTree(const std::vector<DiffEntry>& flatEntries)
{
    std::vector<DiffEntry*> result;
    result.reserve(flatEntries.size());
    for (auto& entry : const_cast<std::vector<DiffEntry>&>(flatEntries))
    {
        result.push_back(&entry);
    }
    return result;
}
