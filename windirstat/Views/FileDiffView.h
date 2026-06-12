#pragma once

#include "pch.h"
#include "ControlView.h"
#include "FileDiffControl.h"

class CFileDiffView final : public CControlView
{
public:
    CFileDiffControl& GetDiffControl() { return m_control; }

protected:
    CTreeListControl& GetControl() override { return m_control; }
    const CTreeListControl& GetControl() const override { return m_control; }

    DECLARE_DYNCREATE(CFileDiffView)
    CFileDiffView() = default;
    ~CFileDiffView() override = default;

    void CreateColumns();
    void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) override;

    CFileDiffControl m_control;

    DECLARE_MESSAGE_MAP()
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};
