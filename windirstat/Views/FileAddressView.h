#pragma once

#include "pch.h"
#include "ControlView.h"
#include "FileAddressControl.h"

class CFileAddressView final : public CControlView
{
protected:
    DECLARE_DYNCREATE(CFileAddressView)
    CFileAddressView() = default;
    ~CFileAddressView() override = default;

    CTreeListControl& GetControl() override { return m_control; }
    const CTreeListControl& GetControl() const override { return m_control; }

    CFileAddressControl m_control;

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
