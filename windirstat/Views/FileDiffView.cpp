#include "pch.h"
#include "ItemDiff.h"
#include "FileDiffView.h"

IMPLEMENT_DYNCREATE(CFileDiffView, CControlView)

BEGIN_MESSAGE_MAP(CFileDiffView, CControlView)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int CFileDiffView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CControlView::OnCreate(lpCreateStruct) == -1)
        return -1;

    constexpr RECT rect = { 0, 0, 0, 0 };
    m_control.CreateExtended(LVS_EX_HEADERDRAGDROP, LVS_OWNERDATA | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, rect, this, ID_WDS_CONTROL);
    m_control.ShowGrid(COptions::ListGrid);
    m_control.ShowStripes(COptions::ListStripes);
    m_control.ShowFullRowSelection(COptions::ListFullRowSelection);

    CreateColumns();
    return 0;
}

void CFileDiffView::CreateColumns()
{
    auto& ctrl = m_control;
    ctrl.InsertColumn(DCOL_DIFF_NAME, L"Name", LVCFMT_LEFT, DpiRest(200), DCOL_DIFF_NAME);
    ctrl.InsertColumn(DCOL_DIFF_TYPE, L"Type", LVCFMT_LEFT, DpiRest(80), DCOL_DIFF_TYPE);
    ctrl.InsertColumn(DCOL_DIFF_OLD_SIZE, L"Old Size", LVCFMT_RIGHT, DpiRest(90), DCOL_DIFF_OLD_SIZE);
    ctrl.InsertColumn(DCOL_DIFF_NEW_SIZE, L"New Size", LVCFMT_RIGHT, DpiRest(90), DCOL_DIFF_NEW_SIZE);
    ctrl.InsertColumn(DCOL_DIFF_DELTA, L"Delta", LVCFMT_RIGHT, DpiRest(90), DCOL_DIFF_DELTA);
    ctrl.InsertColumn(DCOL_DIFF_PERCENT, L"Percent", LVCFMT_RIGHT, DpiRest(80), DCOL_DIFF_PERCENT);
    ctrl.InsertColumn(DCOL_DIFF_SECTION, L"Section", LVCFMT_LEFT, DpiRest(100), DCOL_DIFF_SECTION);
    ctrl.InsertColumn(DCOL_DIFF_LIBRARY, L"Library", LVCFMT_LEFT, DpiRest(150), DCOL_DIFF_LIBRARY);
    m_control.OnColumnsInserted();
}

void CFileDiffView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
    CControlView::OnUpdate(pSender, lHint, pHint);
}
