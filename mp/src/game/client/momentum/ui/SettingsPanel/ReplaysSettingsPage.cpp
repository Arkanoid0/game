#include "cbase.h"

#include "ReplaysSettingsPage.h"
#include "ModelPanel.h"
#include "ienginevgui.h"
#include "util/mom_util.h"

using namespace vgui;

#define SET_BUTTON_COLOR(button, color) \
    button->SetDefaultColor(color, color); \
    button->SetArmedColor(color, color); \
    button->SetSelectedColor(color, color);

ReplaysSettingsPage::ReplaysSettingsPage(Panel *pParent) : BaseClass(pParent, "ReplaysSettings"), ghost_color("mom_ghost_color"),
ghost_bodygroup("mom_ghost_bodygroup"), ghost_trail_color("mom_trail_color")
{
    // Outer frame for the model preview
    m_pModelPreviewFrame = new Frame(nullptr, "ModelPreviewFrame");
    m_pModelPreviewFrame->SetParent(enginevgui->GetPanel(PANEL_GAMEUIDLL));
    m_pModelPreviewFrame->SetSize(350, scheme()->GetProportionalScaledValue(275));
    m_pModelPreviewFrame->SetMoveable(false);
    m_pModelPreviewFrame->MoveToFront();
    m_pModelPreviewFrame->SetSizeable(false);
    m_pModelPreviewFrame->SetTitle("Preview", false); // MOM_TODO: Localize me
    m_pModelPreviewFrame->SetTitleBarVisible(true);
    m_pModelPreviewFrame->SetMenuButtonResponsive(false);
    m_pModelPreviewFrame->SetCloseButtonVisible(true);
    m_pModelPreviewFrame->SetMinimizeButtonVisible(false);
    m_pModelPreviewFrame->SetMaximizeButtonVisible(false);
    m_pModelPreviewFrame->PinToSibling("CMomentumSettingsPanel", PIN_TOPRIGHT, PIN_TOPLEFT);

    // Actual model preview
    m_pModelPreview = new CRenderPanel(m_pModelPreviewFrame, "ModelPreview");
    m_pModelPreview->SetPaintBorderEnabled(true);
    m_pModelPreview->SetBorder(scheme()->GetIScheme(GetScheme())->GetBorder("Default"));
    const bool result = m_pModelPreview->LoadModel("models/player/player_shape_base.mdl");
    if (result)
        UpdateModelSettings();

    m_pModelPreview->SetVisible(true);
    m_pModelPreview->MakeReadyForUse();


    m_pEnableTrail = FindControl<CvarToggleCheckButton<ConVarRef>>("EnableTrail");
    m_pPickTrailColorButton = FindControl<Button>("PickTrailColorButton");

    m_pPickBodyColorButton = FindControl<Button>("PickBodyColorButton");

    // Color Picker is shared for trail and 
    m_pColorPicker = new ColorPicker(this, this);
}

ReplaysSettingsPage::~ReplaysSettingsPage()
{
}

void ReplaysSettingsPage::LoadSettings()
{
    if (m_pPickTrailColorButton)
    {
        Color trailColor;
        if (g_pMomentumUtil->GetColorFromHex(ghost_trail_color.GetString(), trailColor))
        {
            SET_BUTTON_COLOR(m_pPickTrailColorButton, trailColor);
        }
    }

    if (m_pPickBodyColorButton)
    {
        Color bodyColor;
        if (g_pMomentumUtil->GetColorFromHex(ghost_color.GetString(), bodyColor))
        {
            SET_BUTTON_COLOR(m_pPickBodyColorButton, bodyColor);
        }
    }



    UpdateModelSettings();
}

void ReplaysSettingsPage::OnPageShow()
{
    if (!m_pModelPreviewFrame->IsVisible())
        m_pModelPreviewFrame->Activate();
}

void ReplaysSettingsPage::OnPageHide()
{
    if (m_pModelPreviewFrame)
        m_pModelPreviewFrame->Close();
}

void ReplaysSettingsPage::OnMainDialogClosed()
{
    OnPageHide();
}

void ReplaysSettingsPage::OnMainDialogShow()
{
    OnPageShow();
}

void ReplaysSettingsPage::OnTextChanged(Panel *p)
{
    BaseClass::OnTextChanged(p);

    
}

void ReplaysSettingsPage::OnControlModified(Panel *p)
{
    BaseClass::OnControlModified(p);

    
}

void ReplaysSettingsPage::OnColorSelected(KeyValues *pKv)
{
    Color selected = pKv->GetColor("color");

    Panel *pTarget = static_cast<Panel*>(pKv->GetPtr("targetCallback"));

    if (pTarget == m_pPickTrailColorButton)
    {
        SET_BUTTON_COLOR(m_pPickTrailColorButton, selected);

        char buf[32];
        g_pMomentumUtil->GetHexStringFromColor(selected, buf, 32);
        ghost_trail_color.SetValue(buf);
    }
    else if (pTarget == m_pPickBodyColorButton)
    {
        SET_BUTTON_COLOR(m_pPickBodyColorButton, selected);

        char buf[32];
        g_pMomentumUtil->GetHexStringFromColor(selected, buf, 32);
        ghost_color.SetValue(buf);
    }

    UpdateModelSettings();
}

void ReplaysSettingsPage::OnCommand(const char* command)
{
    if (FStrEq(command, "picker_trail"))
    {
        Color trailColor;
        if (g_pMomentumUtil->GetColorFromHex(ghost_trail_color.GetString(), trailColor))
        {
            m_pColorPicker->SetPickerColor(trailColor);
            m_pColorPicker->SetTargetCallback(m_pPickTrailColorButton);
            m_pColorPicker->Show();
        }
    }
    else if (FStrEq(command, "picker_body"))
    {
        Color bodyColor;
        if (g_pMomentumUtil->GetColorFromHex(ghost_color.GetString(), bodyColor))
        {
            m_pColorPicker->SetPickerColor(bodyColor);
            m_pColorPicker->SetTargetCallback(m_pPickBodyColorButton);
            m_pColorPicker->Show();
        }
    }


    BaseClass::OnCommand(command);
}

void ReplaysSettingsPage::ApplySchemeSettings(IScheme* pScheme)
{
    BaseClass::ApplySchemeSettings(pScheme);
    LoadSettings();
}

void ReplaysSettingsPage::UpdateModelSettings()
{
    C_BaseFlex *pModel = m_pModelPreview->GetModel();
    if (!pModel)
        return;

    // Player color
    Color ghostRenderColor;
    if (g_pMomentumUtil->GetColorFromHex(ghost_color.GetString(), ghostRenderColor))
    {
        pModel->SetRenderColor(ghostRenderColor.r(), ghostRenderColor.g(), ghostRenderColor.b(), ghostRenderColor.a());
    }
    
    // Player shape
    pModel->SetBodygroup(1, ghost_bodygroup.GetInt());
}
