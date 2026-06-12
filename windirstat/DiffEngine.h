#pragma once

#include "pch.h"

class CItem;

enum class DiffType : uint8_t
{
    Unchanged,
    Added,
    Removed,
    SizeChanged,
    AddressChanged
};

struct DiffEntry
{
    std::wstring name;
    DiffType type = DiffType::Unchanged;
    ULONGLONG oldSize = 0;
    ULONGLONG newSize = 0;
    ULONGLONG oldAddress = 0;
    ULONGLONG newAddress = 0;
    CItem* oldItem = nullptr;
    CItem* newItem = nullptr;
    std::wstring section;
    std::wstring library;
    std::wstring objectPath;
    int depth = 0;
};

struct DiffSummary
{
    ULONGLONG totalOldSize = 0;
    ULONGLONG totalNewSize = 0;
    int addedCount = 0;
    int removedCount = 0;
    int sizeChangedCount = 0;
    int unchangedCount = 0;
    ULONGLONG addedBytes = 0;
    ULONGLONG removedBytes = 0;
    ULONGLONG increasedBytes = 0;
    ULONGLONG decreasedBytes = 0;
};

class CDiffEngine
{
public:
    static std::vector<DiffEntry> ComputeDiff(CItem* oldRoot, CItem* newRoot);
    static DiffSummary ComputeSummary(const std::vector<DiffEntry>& entries);
    static std::vector<DiffEntry*> BuildDiffTree(const std::vector<DiffEntry>& flatEntries);
};
