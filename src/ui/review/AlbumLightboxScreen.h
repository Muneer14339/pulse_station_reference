#pragma once
// src/ui/review/AlbumLightboxScreen.h
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include "training/data/ReviewDataTypes.h"

/**
 * Fullscreen shot image viewer.
 * Left:  image in QScrollArea (scroll-wheel zoom — only the image scales, not the UI)
 *        prev/next arrows; boundary arrow hidden automatically.
 * Right: details panel (full height = image area), close button inside panel header row.
 */
class AlbumLightboxScreen : public QWidget {
    Q_OBJECT
public:
    explicit AlbumLightboxScreen(QWidget* parent = nullptr);
    void open(const QVector<ShotRecord>& shots, const QString& sessionId, int startIndex);

signals:
    void closeRequested();

private:
    void loadShot(int idx);
    void applyZoom();
    void wheelEvent(QWheelEvent* event) override;

    // data
    QVector<ShotRecord> m_shots;
    int    m_idx  = 0;
    double m_zoom = 1.0;
    QPixmap m_pixmap;

    // image side
    QScrollArea* m_scrollArea  = nullptr;
    QLabel*      m_imageLabel  = nullptr;
    QLabel*      m_zoomHint    = nullptr;
    QPushButton* m_prevBtn     = nullptr;
    QPushButton* m_nextBtn     = nullptr;

    // details side
    QLabel* m_detailSession = nullptr;
    QLabel* m_detailShot    = nullptr;
    QLabel* m_detailScore   = nullptr;
};