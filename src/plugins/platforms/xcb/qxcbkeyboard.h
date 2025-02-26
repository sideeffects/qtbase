// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QXCBKEYBOARD_H
#define QXCBKEYBOARD_H

#include "qxcbobject.h"

#include <xcb/xcb_keysyms.h>
#define explicit dont_use_cxx_explicit
#include <xcb/xkb.h>
#undef explicit

#include <QtGui/private/qxkbcommon_p.h>
#include <xkbcommon/xkbcommon-x11.h>

#include <QEvent>

QT_BEGIN_NAMESPACE

class QXcbKeyboard : public QXcbObject
{
public:
    QXcbKeyboard(QXcbConnection *connection);

    ~QXcbKeyboard();

    void initialize();
    void selectEvents();

    void handleKeyPressEvent(const xcb_key_press_event_t *event);
    void handleKeyReleaseEvent(const xcb_key_release_event_t *event);

    Qt::KeyboardModifiers translateModifiers(int s) const;
    void updateKeymap(xcb_mapping_notify_event_t *event);
    void updateKeymap();
    QList<int> possibleKeys(const QKeyEvent *event) const;

    void updateXKBMods();
    xkb_mod_mask_t xkbModMask(quint16 state);
    void updateXKBStateFromCore(quint16 state);
    void updateXKBStateFromXI(void *modInfo, void *groupInfo);

    int coreDeviceId() const { return core_device_id; }
    void updateXKBState(xcb_xkb_state_notify_event_t *state);

    void handleStateChanges(xkb_state_component changedComponents);

protected:
    void handleKeyEvent(xcb_window_t sourceWindow, QEvent::Type type, xcb_keycode_t code,
                        quint16 state, xcb_timestamp_t time, bool fromSendEvent);

    void resolveMaskConflicts();

    typedef QMap<xcb_keysym_t, int> KeysymModifierMap;
    struct xkb_keymap *keymapFromCore(const KeysymModifierMap &keysymMods);

    void updateModifiers(const KeysymModifierMap &keysymMods);
    KeysymModifierMap keysymsToModifiers();

    void updateVModMapping();
    void updateVModToRModMapping();

private:
    bool m_config = false;
    bool m_isAutoRepeat = false;
    xcb_keycode_t m_autoRepeatCode = 0;

    struct _mod_masks {
        uint alt;
        uint altgr;
        uint meta;
        uint super;
        uint hyper;
        uint sidefx_numlock;
    };

    _mod_masks rmod_masks;

    xcb_key_symbols_t *m_key_symbols = nullptr;
    struct _xkb_mods {
        xkb_mod_index_t shift;
        xkb_mod_index_t lock;
        xkb_mod_index_t control;
        xkb_mod_index_t mod1;
        xkb_mod_index_t mod2;
        xkb_mod_index_t mod3;
        xkb_mod_index_t mod4;
        xkb_mod_index_t mod5;
    };
    _xkb_mods xkb_mods;

    _mod_masks vmod_masks;
    int core_device_id;

    QXkbCommon::ScopedXKBState m_xkbState;
    // Special keyboard state object that excludes any modifiers changes we
    // don't care about (almost all) and keeps layout/group indices in sync
    // with m_xkbState.
    QXkbCommon::ScopedXKBState m_sidefx_xkbStateNoModifiers;
    // Masks for the modifiers we care about with m_sidefx_xkbStateNoModifiers.
    //
    // Use m_sidefx_noModifiersRModMask when dealing with X key events and
    // m_sidefx_noModifiersXKBModMask when updating the XKB keyboard state
    // itself.
    quint16 m_sidefx_noModifiersRModMask;
    xkb_mod_mask_t m_sidefx_noModifiersXKBModMask;
    QXkbCommon::ScopedXKBKeymap m_xkbKeymap;
    QXkbCommon::ScopedXKBContext m_xkbContext;

    bool m_superAsMeta = false;
    bool m_hyperAsMeta = false;
};

QT_END_NAMESPACE

#endif
