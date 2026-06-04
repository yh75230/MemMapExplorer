#include "pch.h"
#include "SelectRegionDlg.h"

IMPLEMENT_DYNAMIC(CSelectRegionDlg, CLayoutDialogEx)

CSelectRegionDlg::CSelectRegionDlg(const std::vector<MapRegionInfo>& regions, CWnd* pParent)
    : CLayoutDialogEx(IDD_SELECTREGION, COptions::RegionSelectWindowRect.Ptr(), pParent)
    , m_regions(regions)
{
}

std::optional<std::wstring> CSelectRegionDlg::GetSelectedRegion() const
{
    return m_selectedRegion;
}

void CSelectRegionDlg::DoDataExchange(CDataExchange* pDX)
{
    CLayoutDialogEx::DoDataExchange(pDX);
    DDX_Radio(pDX, IDC_RADIO_TARGET_DRIVES_ALL, m_targetMode);
    DDX_Control(pDX, IDC_LIST, m_listBox);
}

BEGIN_MESSAGE_MAP(CSelectRegionDlg, CLayoutDialogEx)
    ON_BN_CLICKED(IDC_RADIO_TARGET_DRIVES_ALL, &CSelectRegionDlg::OnBnClickedTargetMode)
    ON_BN_CLICKED(IDC_RADIO_TARGET_DRIVES_SUBSET, &CSelectRegionDlg::OnBnClickedTargetMode)
    ON_LBN_DBLCLK(IDC_LIST, &CSelectRegionDlg::OnLbnDblclkRegionList)
    ON_WM_CTLCOLOR()
END_MESSAGE_MAP()

BOOL CSelectRegionDlg::OnInitDialog()
{
    CLayoutDialogEx::OnInitDialog();

    Localization::UpdateDialogs(*this);
    DarkMode::AdjustControls(GetSafeHwnd());
    ModifyStyle(0, WS_CLIPCHILDREN);

    m_layout.AddControl(IDOK, 1, 1, 0, 0);
    m_layout.AddControl(IDCANCEL, 1, 1, 0, 0);
    m_layout.AddControl(IDC_CAPTION, 0, 0, 1, 0);
    m_layout.AddControl(IDC_RADIO_TARGET_DRIVES_ALL, 0, 0, 0, 0);
    m_layout.AddControl(IDC_RADIO_TARGET_DRIVES_SUBSET, 0, 0, 0, 0);
    m_layout.AddControl(IDC_LIST, 0, 0, 1, 1);
    m_layout.OnInitDialog(true);

    for (const auto& region : m_regions)
    {
        const auto label = std::format(L"{} (origin=0x{:X}, size={}, attrs={})",
            region.name, region.origin, FormatBytes(region.length), region.attrs);
        m_listBox.AddString(label.c_str());
    }
    if (m_listBox.GetCount() > 0)
    {
        m_listBox.SetCurSel(0);
    }

    UpdateData(FALSE);
    UpdateControls();
    return TRUE;
}

void CSelectRegionDlg::OnOK()
{
    UpdateData();

    if (m_targetMode != 0)
    {
        const int sel = m_listBox.GetCurSel();
        if (sel >= 0 && sel < static_cast<int>(m_regions.size()))
        {
            m_selectedRegion = m_regions[sel].name;
        }
        else
        {
            return;
        }
    }
    else
    {
        m_selectedRegion.reset();
    }

    CLayoutDialogEx::OnOK();
}

void CSelectRegionDlg::UpdateControls()
{
    m_listBox.EnableWindow(m_targetMode != 0);
}

void CSelectRegionDlg::OnBnClickedTargetMode()
{
    UpdateData();
    UpdateControls();
}

void CSelectRegionDlg::OnLbnDblclkRegionList()
{
    if (m_targetMode != 0)
    {
        OnOK();
    }
}

HBRUSH CSelectRegionDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, const UINT nCtlColor)
{
    const HBRUSH brush = DarkMode::OnCtlColor(pDC, nCtlColor);
    return brush ? brush : CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);
}
