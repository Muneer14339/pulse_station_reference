#pragma once
// src/ui/review/SessionAlbumTab.h
#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include "training/data/ReviewDataTypes.h"
#include "ui/review/AlbumLightboxScreen.h"

/**
 * Session album: 3-col grid of square shot cards.
 * Click → opens AlbumLightboxScreen via QStackedWidget (index 0=grid, 1=lightbox).
 */
class SessionAlbumTab : public QWidget {
    Q_OBJECT
public:
    explicit SessionAlbumTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);

private:
    static constexpr int COLS       = 3;
    static constexpr int CARD_SIZE  = 200;
    static constexpr int CAPTION_H  = 34;

    QWidget* makeCard(const ShotRecord& s, int index);
    void     openLightbox(int index);

    QStackedWidget*      m_stack   = nullptr;
    QGridLayout*         m_grid    = nullptr;
    AlbumLightboxScreen* m_lightbox= nullptr;
    QVector<ShotRecord>  m_shots;
    QString              m_sessionId;
};