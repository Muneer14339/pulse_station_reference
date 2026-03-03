#pragma once
// src/ui/review/AlbumLightboxScreen.h
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include "training/data/ReviewDataTypes.h"

/**
 * Fullscreen shot image viewer.
 * Zoom: scroll-wheel + pinch gesture (touchscreen).
 * Nav: QPushButton with large guillemet arrows — reliable on all platforms.
 */
class AlbumLightboxScreen : public QWidget {
    Q_OBJECT
public:
    explicit AlbumLightboxScreen(QWidget* parent = nullptr);
    void open(const QVector<ShotRecord>& shots, const QString& sessionId, int startIndex);

signals:
    void closeRequested();

protected:
    void wheelEvent(QWheelEvent* e) override;
    bool event(QEvent* e) override;

private:
    void loadShot(int idx);
    void applyZoom(double newZoom);

    QVector<ShotRecord> m_shots;
    int     m_idx  = 0;
    double  m_zoom = 1.0;
    QPixmap m_pixmap;

    QScrollArea* m_scrollArea  = nullptr;
    QLabel*      m_imageLabel  = nullptr;
    QLabel*      m_zoomHint    = nullptr;
    QPushButton* m_prevBtn     = nullptr;
    QPushButton* m_nextBtn     = nullptr;
    QLabel* m_detailSession = nullptr;
    QLabel* m_detailShot    = nullptr;
    QLabel* m_detailScore   = nullptr;
};