#pragma once
// src/ui/review/SessionAlbumTab.h
#include <QWidget>
#include <QLabel>
#include <QScrollArea>
#include <QGridLayout>
#include "training/data/ReviewDataTypes.h"

/**
 * @brief Session album: 5-column image grid + click-to-lightbox.
 *        Lightbox: full overlay, scroll-wheel zoom, prev/next nav.
 */
class SessionAlbumTab : public QWidget {
    Q_OBJECT
public:
    explicit SessionAlbumTab(QWidget* parent = nullptr);
    void populate(const SessionResult& r);

private:
    QWidget*     makeCard(const ShotRecord& s, int index);
    void         showLightbox(int index);

    QGridLayout*        m_grid;
    QVector<ShotRecord> m_shots;
};
