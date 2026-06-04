#pragma once

#include "pch.h"
#include "Layout.h"
#include "MapLoader.h"

class CSelectRegionDlg final : public CLayoutDialogEx
{
	DECLARE_DYNAMIC(CSelectRegionDlg)

public:
	explicit CSelectRegionDlg(const std::vector<MapRegionInfo>& regions, CWnd* pParent = nullptr);
	~CSelectRegionDlg() override = default;

	std::optional<std::wstring> GetSelectedRegion() const;

protected:
	enum : std::uint8_t { IDD = IDD_SELECTREGION };

	BOOL OnInitDialog() override;
	void DoDataExchange(CDataExchange* pDX) override;
	void OnOK() override;
	void UpdateControls();

	DECLARE_MESSAGE_MAP()

private:
	std::vector<MapRegionInfo> m_regions;
	std::optional<std::wstring> m_selectedRegion;
	int m_targetMode = 0;
	CListBox m_listBox;

	afx_msg void OnBnClickedTargetMode();
	afx_msg void OnLbnDblclkRegionList();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
