// MIT License
// Copyright (C) August 2016 Hotride

#include "GumpPopupMenu.h"
#include "../SelectedObject.h"
#include "../Managers/MouseManager.h"
#include "../Managers/ColorManager.h"
#include "../Managers/GumpManager.h"
#include "../Managers/IntlocManager.h"
#include "../Network/Packets.h"

CGumpPopupMenu *g_PopupMenu = nullptr;

CGumpPopupMenu::CGumpPopupMenu(uint32_t serial, short x, short y)
    : CGump(GT_POPUP_MENU, serial, x, y)
{
    DEBUG_TRACE_FUNCTION;
    NoMove = true;
    g_PopupMenu = this;
    Page = 1;

    m_Polygone = (CGUIColoredPolygone *)Add(new CGUIColoredPolygone(0, 0, 0, 0, 0, 0, 0xFFFFFFFE));
    m_Polygone->DrawOnly = true;
    m_Polygone->Visible = false;
}

CGumpPopupMenu::~CGumpPopupMenu()
{
    DEBUG_TRACE_FUNCTION;
    g_PopupMenu = nullptr;
}

void CGumpPopupMenu::Parse(Wisp::CPacketReader &reader)
{
    uint16_t mode = reader.ReadUInt16BE();
    bool isNewClilocs = (mode >= 2);
    uint32_t serial = reader.ReadUInt32BE();
    uint8_t count = reader.ReadUInt8();

    vector<CPopupMenuItemInfo> items;

    for (int i = 0; i < count; i++)
    {
        CPopupMenuItemInfo info;
        info.Color = 0xFFFE;

        if (isNewClilocs)
        {
            info.Cliloc = reader.ReadUInt32BE();
            info.Index = reader.ReadUInt16BE() + 1;
            info.Flags = reader.ReadUInt16BE();
        }
        else
        {
            info.Index = reader.ReadUInt16BE() + 1;
            info.Cliloc = reader.ReadUInt16BE() + 3000000;
            info.Flags = reader.ReadUInt16BE();

            if ((info.Flags & 0x84) != 0)
            {
                reader.Move(2);
            }

            if ((info.Flags & 0x40) != 0)
            {
                reader.Move(2);
            }

            if ((info.Flags & 0x20) != 0)
            {
                info.ReplaceColor = reader.ReadUInt16BE() & 0x3FFF;
            }
        }

        if ((info.Flags & 0x01) != 0)
        {
            info.Color = 0x0386;
        }

        items.push_back(info);
    }

    CGumpPopupMenu *menu =
        new CGumpPopupMenu(serial, g_MouseManager.Position.X, g_MouseManager.Position.Y);
    menu->Add(new CGUIPage(0));
    int width = 0;
    int height = 20;

    menu->Add(new CGUIAlphaBlending(true, 0.75f));
    CGUIResizepic *resizepic = (CGUIResizepic *)menu->Add(new CGUIResizepic(0, 0x0A3C, 0, 0, 0, 0));
    menu->Add(new CGUIAlphaBlending(false, 1.0f));

    int offsetY = 10;
    bool arrowAdded = false;

    for (const CPopupMenuItemInfo &info : items)
    {
        wstring str = g_IntlocManager.Intloc(g_Language, info.Cliloc, isNewClilocs);

        CGUITextEntry *item = new CGUITextEntry(
            info.Index,
            info.Color,
            info.Color,
            info.Color,
            10,
            offsetY,
            0,
            true,
            CONTEXT_MENU_FONT);

        if (info.ReplaceColor != 0u)
        {
            item->SetGlobalColor(
                true, g_ColorManager.Color16To32(info.ReplaceColor), 0xFFFFFFFE, 0xFFFFFFFE);
        }

        if (((info.Flags & 0x02) != 0) && !arrowAdded)
        {
            arrowAdded = true;
            menu->Add(new CGUIPage(1));
            menu->Add(new CGUIButton(
                CGumpPopupMenu::ID_GPM_MAXIMIZE, 0x15E6, 0x15E2, 0x15E2, 20, offsetY));
            menu->Add(new CGUIPage(2));

            height += 20;
        }

        menu->Add(item);

        CEntryText &entry = item->m_Entry;
        entry.SetTextW(str);
        entry.PrepareToDrawW(CONTEXT_MENU_FONT, info.Color);

        CGLTextTexture &texture = entry.m_Texture;

        menu->Add(new CGUIHitBox(info.Index, 10, offsetY, texture.Width, texture.Height, true));

        offsetY += texture.Height;

        if (!arrowAdded)
        {
            height += texture.Height;

            if (width < texture.Width)
            {
                width = texture.Width;
            }
        }
    }

    width += 20;

    if (height <= 10 || width <= 20)
    {
        delete menu;
    }
    else
    {
        resizepic->Width = width;
        resizepic->Height = height;

        QFOR(item, menu->m_Items, CBaseGUI *)
        {
            if (item->Type == GOT_PAGE && (((CGUIPage *)item)->Index != 0))
            {
                break;
            }
            if (item->Type == GOT_HITBOX)
            {
                ((CGUIHitBox *)item)->Width = width - 20;
            }
        }

        g_GumpManager.AddGump(menu);
    }
}

void CGumpPopupMenu::PrepareContent()
{
    DEBUG_TRACE_FUNCTION;
    if (g_SelectedObject.Gump == this && g_SelectedObject.Object != nullptr &&
        ((CBaseGUI *)g_SelectedObject.Object)->Type == GOT_HITBOX)
    {
        CGUIHitBox *box = (CGUIHitBox *)g_SelectedObject.Object;

        if (!m_Polygone->Visible || m_Polygone->GetY() != box->GetY())
        {
            WantRedraw = true;

            m_Polygone->SetX(box->GetX());
            m_Polygone->SetY(box->GetY());
            m_Polygone->Width = box->Width;
            m_Polygone->Height = box->Height;
            m_Polygone->Visible = true;
        }
    }
    else if (m_Polygone->Visible)
    {
        WantRedraw = true;

        m_Polygone->Visible = false;
    }
}

void CGumpPopupMenu::GUMP_BUTTON_EVENT_C
{
    DEBUG_TRACE_FUNCTION;
    if (serial == ID_GPM_MAXIMIZE)
    {
        Page = 2;
        CGUIResizepic *resizepic = nullptr;
        int width = 0;
        int height = 20;

        QFOR(item, m_Items, CBaseGUI *)
        {
            if (item->Type == GOT_RESIZEPIC)
            {
                resizepic = (CGUIResizepic *)item;
            }
            else if (item->Type == GOT_TEXTENTRY)
            {
                CGLTextTexture &texture = ((CGUITextEntry *)item)->m_Entry.m_Texture;

                if (width < texture.Width)
                {
                    width = texture.Width;
                }

                height += texture.Height;
            }
        }

        width += 20;

        QFOR(item, m_Items, CBaseGUI *)
        {
            if (item->Type == GOT_HITBOX)
            {
                ((CGUIHitBox *)item)->Width = width - 20;
            }
        }

        if (resizepic != nullptr)
        {
            resizepic->Width = width;
            resizepic->Height = height;
        }

        WantRedraw = true;
        m_Polygone->Visible = false;
    }
    else
    {
        CPacketPopupMenuSelection(Serial, serial - 1).Send();

        RemoveMark = true;
    }
}
