#pragma once
// src/ui/review/SessionAlbumTab.h
#include <QWidget>
#include <QGridLayout>
#include <QStackedWidget>
#include <QResizeEvent>
#include "training/data/ReviewDataTypes.h"
#include "ui/review/AlbumLightboxScreen.h"

/**
 * Session album: responsive 3-col grid.
 * Cards fill available width equally; height = width (square).
 * Click → AlbumLightboxScreen via QStackedWidget.
 */
class SessionAlbumTab : public QWidget {
    Q_OBJECT
public:
    explicit SessionAlbumTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);

protected:
    void resizeEvent(QResizeEvent* e) override;

private:
    static constexpr int  COLS       = 3;
    static constexpr int  SPACING    = 10;
    static constexpr int  MARGINS    = 16;
    static constexpr int  CAPTION_H  = 40;
    static constexpr int  CAPTION_MX = 8;   // caption left/right margin from card edge
    static constexpr int  CAPTION_MB = 8;   // caption bottom margin from card edge

    QWidget* makeCard(const ShotRecord& s, int index, int cardSize);
    void     openLightbox(int index);
    void     rebuildGrid();          // called on first populate + resize
    int      cardSize() const;       // computed from current width

    QStackedWidget*      m_stack    = nullptr;
    QWidget*             m_gridPage = nullptr;
    QGridLayout*         m_grid     = nullptr;
    AlbumLightboxScreen* m_lightbox = nullptr;
    QVector<ShotRecord>  m_shots;
    QString              m_sessionId;
    bool                 m_populated = false;
};