#include "pch.h"
#include "ItemAddress.h"
#include "FileAddressView.h"

IMPLEMENT_DYNCREATE(CFileAddressView, CControlView)

BEGIN_MESSAGE_MAP(CFileAddressView, CControlView)
    ON_WM_CREATE()
END_MESSAGE_MAP()

int CFileAddressView::OnCreate(const LPCREATESTRUCT lpCreateStruct)
{
    if (CControlView::OnCreate(lpCreateStruct) == -1) return -1;

    constexpr RECT rect = { 0, 0, 0, 0 };
    m_control.CreateExtended(LVS_EX_HEADERDRAGDROP, LVS_OWNERDATA | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_SHOWSELALWAYS, rect, this, ID_WDS_CONTROL);
    m_control.ShowGrid(COptions::ListGrid);
    m_control.ShowStripes(COptions::ListStripes);
    m_control.ShowFullRowSelection(COptions::ListFullRowSelection);

    auto& ctrl = m_control;
    ctrl.InsertColumn(COL_ITEMADDR_NAME, L"Name", LVCFMT_LEFT, DpiRest(250), COL_ITEMADDR_NAME);
    ctrl.InsertColumn(COL_ITEMADDR_ADDRESS, L"Address", LVCFMT_LEFT, DpiRest(110), COL_ITEMADDR_ADDRESS);
    ctrl.InsertColumn(COL_ITEMADDR_SIZE, L"Size", LVCFMT_RIGHT, DpiRest(90), COL_ITEMADDR_SIZE);
    ctrl.InsertColumn(COL_ITEMADDR_SECTION, L"Section", LVCFMT_LEFT, DpiRest(100), COL_ITEMADDR_SECTION);
    ctrl.InsertColumn(COL_ITEMADDR_GAP, L"Gap", LVCFMT_RIGHT, DpiRest(120), COL_ITEMADDR_GAP);
    m_control.OnColumnsInserted();
    m_control.SetSorting(COL_ITEMADDR_ADDRESS, true);

    return 0;
}
