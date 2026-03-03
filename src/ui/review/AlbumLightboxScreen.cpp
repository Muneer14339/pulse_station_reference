// src/ui/review/AlbumLightboxScreen.cpp
#include "AlbumLightboxScreen.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QPinchGesture>

// Nav arrow button: QPushButton with large guillemet characters.
// QToolButton::setArrowType draws tiny arrows when a QSS stylesheet is applied —
// using text + font-size is the reliable cross-platform solution.
static QPushButton* makeArrowBtn(bool isLeft, QWidget* parent) {
    // ‹ / › (single guillemets) render well at large font sizes on all platforms
    auto* btn = new QPushButton(isLeft ? "\u2039" : "\u203A", parent);
    btn->setFixedSize(52, 80);
    btn->setStyleSheet(AppTheme::navArrowButton());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFocusPolicy(Qt::NoFocus);
    return btn;
}

AlbumLightboxScreen::AlbumLightboxScreen(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    grabGesture(Qt::PinchGesture);
    setStyleSheet(AppTheme::lightboxBg());

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left: image area ─────────────────────────────────────────────────────
    auto* imgPanel = new QWidget(this);
    imgPanel->setStyleSheet(AppTheme::transparent());
    auto* imgVb = new QVBoxLayout(imgPanel);
    imgVb->setContentsMargins(0, 0, 0, 0);
    imgVb->setSpacing(0);

    // Zoom hint
    auto* hintBar = new QWidget(imgPanel);
    hintBar->setStyleSheet(AppTheme::transparent());
    auto* hbl = new QHBoxLayout(hintBar);
    hbl->setContentsMargins(16, 12, 16, 8);
    m_zoomHint = new QLabel("Pinch or scroll to zoom  (1.0×)", hintBar);
    m_zoomHint->setStyleSheet(AppTheme::labelMuted());
    hbl->addWidget(m_zoomHint);
    hbl->addStretch();
    hintBar->setLayout(hbl);
    imgVb->addWidget(hintBar);

    // Nav: prev | scroll-area | next
    auto* navRow = new QWidget(imgPanel);
    navRow->setStyleSheet(AppTheme::transparent());
    auto* nl = new QHBoxLayout(navRow);
    nl->setContentsMargins(12, 0, 12, 16);
    nl->setSpacing(12);

    m_prevBtn = makeArrowBtn(true, navRow);
    connect(m_prevBtn, &QPushButton::clicked, this, [this]{ loadShot(m_idx - 1); });

    m_scrollArea = new QScrollArea(navRow);
    m_scrollArea->setStyleSheet(AppTheme::cameraView());
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(false);

    m_imageLabel = new QLabel;
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet(AppTheme::transparent());
    m_scrollArea->setWidget(m_imageLabel);

    m_nextBtn = makeArrowBtn(false, navRow);
    connect(m_nextBtn, &QPushButton::clicked, this, [this]{ loadShot(m_idx + 1); });

    nl->addWidget(m_prevBtn, 0, Qt::AlignVCenter);
    nl->addWidget(m_scrollArea, 1);
    nl->addWidget(m_nextBtn, 0, Qt::AlignVCenter);
    navRow->setLayout(nl);
    imgVb->addWidget(navRow, 1);
    imgPanel->setLayout(imgVb);

    // ── Right: details panel (full height) ───────────────────────────────────
    auto* detailPanel = new QWidget(this);
    detailPanel->setFixedWidth(270);
    detailPanel->setAttribute(Qt::WA_StyledBackground, true);
    detailPanel->setStyleSheet(AppTheme::sidePanel());

    auto* dl = new QVBoxLayout(detailPanel);
    dl->setContentsMargins(0, 0, 0, 0);

    auto* infoBox = new QWidget(detailPanel);
    infoBox->setAttribute(Qt::WA_StyledBackground, true);
    infoBox->setStyleSheet(AppTheme::summaryBox());
    infoBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* il = new QVBoxLayout(infoBox);
    il->setContentsMargins(16, 14, 16, 14);
    il->setSpacing(10);

    // Title + close on same row
    auto* titleRow = new QWidget(infoBox);
    titleRow->setStyleSheet(AppTheme::transparent());
    auto* trl = new QHBoxLayout(titleRow);
    trl->setContentsMargins(0, 0, 0, 0);
    auto* titleLbl = new QLabel("Shot Details", infoBox);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());
    auto* closeBtn = new QPushButton("✕", infoBox);
    closeBtn->setStyleSheet(AppTheme::refreshButton());
    closeBtn->setFixedSize(32, 32);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &AlbumLightboxScreen::closeRequested);
    trl->addWidget(titleLbl, 1);
    trl->addWidget(closeBtn);
    titleRow->setLayout(trl);
    il->addWidget(titleRow);

    auto* div = new QWidget(infoBox);
    div->setFixedHeight(1);
    div->setStyleSheet(AppTheme::divider());
    il->addWidget(div);

    auto makeRow = [&](const QString& key) -> QLabel* {
        auto* row = new QWidget(infoBox);
        row->setStyleSheet(AppTheme::transparent());
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 2, 0, 2);
        auto* k = new QLabel(key, row);
        k->setStyleSheet(AppTheme::labelSecondary());
        auto* v = new QLabel("—", row);
        v->setStyleSheet(AppTheme::summaryRowValue());
        v->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
        rl->addWidget(k);
        rl->addStretch();
        rl->addWidget(v);
        row->setLayout(rl);
        il->addWidget(row);
        return v;
    };

    m_detailSession = makeRow("Session ID");
    m_detailShot    = makeRow("Shot Number");
    m_detailScore   = makeRow("Score");
    il->addStretch();
    infoBox->setLayout(il);
    dl->addWidget(infoBox, 1);
    detailPanel->setLayout(dl);

    root->addWidget(imgPanel, 1);
    root->addWidget(detailPanel);
    setLayout(root);
}

void AlbumLightboxScreen::open(const QVector<ShotRecord>& shots,
                                const QString& sessionId, int startIndex)
{
    m_shots = shots;
    m_detailSession->setText(sessionId.isEmpty() ? "—" : sessionId);
    loadShot(startIndex);
}

void AlbumLightboxScreen::loadShot(int idx) {
    if (m_shots.isEmpty()) return;
    m_idx = qBound(0, idx, m_shots.size() - 1);

    const ShotRecord& s = m_shots[m_idx];
    m_detailShot->setText(QString::number(s.number));
    m_detailScore->setText(QString::number(s.score));

    m_prevBtn->setVisible(m_idx > 0);
    m_nextBtn->setVisible(m_idx < m_shots.size() - 1);

    m_pixmap = QPixmap();
    if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath))
        m_pixmap.load(s.imagePath);

    applyZoom(1.0);
}

void AlbumLightboxScreen::applyZoom(double newZoom) {
    m_zoom = qBound(0.5, newZoom, 5.0);
    m_zoomHint->setText(QString("Pinch or scroll to zoom  (%1×)")
                        .arg(QString::number(m_zoom, 'f', 1)));

    if (m_pixmap.isNull()) {
        m_imageLabel->setPixmap(QPixmap());
        m_imageLabel->setText(m_shots.isEmpty()
            ? "No Image"
            : QString("Shot #%1 — No Image").arg(m_shots.value(m_idx).number));
        m_imageLabel->resize(m_scrollArea->viewport()->size());
        return;
    }

    const QPixmap scaled = m_pixmap.scaled(
        m_pixmap.size() * m_zoom, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    m_imageLabel->setPixmap(scaled);
    m_imageLabel->resize(scaled.size());
}

void AlbumLightboxScreen::wheelEvent(QWheelEvent* e) {
    if (m_scrollArea->geometry().contains(e->pos())) {
        applyZoom(m_zoom + (e->angleDelta().y() > 0 ? 0.15 : -0.15));
        e->accept();
    }
}

bool AlbumLightboxScreen::event(QEvent* e) {
    if (e->type() == QEvent::Gesture) {
        auto* ge = static_cast<QGestureEvent*>(e);
        if (auto* pinch = static_cast<QPinchGesture*>(ge->gesture(Qt::PinchGesture))) {
            if (pinch->changeFlags() & QPinchGesture::ScaleFactorChanged)
                applyZoom(m_zoom * pinch->scaleFactor());
            ge->accept();
            return true;
        }
    }
    return QWidget::event(e);
}