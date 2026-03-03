// src/ui/review/AlbumLightboxScreen.cpp
#include "AlbumLightboxScreen.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QWheelEvent>

AlbumLightboxScreen::AlbumLightboxScreen(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(AppTheme::panel());  // app standard dark panel — no hardcoded color

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left: image area ─────────────────────────────────────────────────────
    auto* imgPanel = new QWidget(this);
    imgPanel->setStyleSheet(AppTheme::transparent());
    auto* imgVb = new QVBoxLayout(imgPanel);
    imgVb->setContentsMargins(0, 0, 0, 0);
    imgVb->setSpacing(0);

    // Zoom hint bar at top
    auto* hintBar = new QWidget(imgPanel);
    hintBar->setStyleSheet(AppTheme::transparent());
    auto* hl = new QHBoxLayout(hintBar);
    hl->setContentsMargins(16, 12, 16, 8);
    m_zoomHint = new QLabel("Scroll to zoom  (1.0×)", hintBar);
    m_zoomHint->setStyleSheet(AppTheme::labelMuted());
    hl->addWidget(m_zoomHint);
    hl->addStretch();
    hintBar->setLayout(hl);
    imgVb->addWidget(hintBar);

    // Nav row: prev | scroll-area | next
    auto* navRow = new QWidget(imgPanel);
    navRow->setStyleSheet(AppTheme::transparent());
    auto* nl = new QHBoxLayout(navRow);
    nl->setContentsMargins(12, 0, 12, 16);
    nl->setSpacing(12);

    m_prevBtn = new QPushButton("←", navRow);
    m_prevBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_prevBtn->setFixedSize(48, 48);
    m_prevBtn->setCursor(Qt::PointingHandCursor);
    m_prevBtn->setFocusPolicy(Qt::NoFocus);
    connect(m_prevBtn, &QPushButton::clicked, this, [this]{ loadShot(m_idx - 1); });

    // QScrollArea: image label inside — zoom only changes label/pixmap size
    m_scrollArea = new QScrollArea(navRow);
    m_scrollArea->setStyleSheet(AppTheme::cameraView());
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setAlignment(Qt::AlignCenter);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setWidgetResizable(false);  // we control size manually

    m_imageLabel = new QLabel;
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setStyleSheet(AppTheme::transparent());
    m_scrollArea->setWidget(m_imageLabel);

    m_nextBtn = new QPushButton("→", navRow);
    m_nextBtn->setStyleSheet(AppTheme::buttonPrimary());
    m_nextBtn->setFixedSize(48, 48);
    m_nextBtn->setCursor(Qt::PointingHandCursor);
    m_nextBtn->setFocusPolicy(Qt::NoFocus);
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
    dl->setSpacing(0);

    // Details card — full width, starts at top, stretches full height
    auto* infoBox = new QWidget(detailPanel);
    infoBox->setAttribute(Qt::WA_StyledBackground, true);
    infoBox->setStyleSheet(AppTheme::summaryBox());
    infoBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* il = new QVBoxLayout(infoBox);
    il->setContentsMargins(16, 14, 16, 14);
    il->setSpacing(10);

    // Title row: "Shot Details" + close button on same line
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

    auto* infoDiv = new QWidget(infoBox);
    infoDiv->setFixedHeight(1);
    infoDiv->setStyleSheet(AppTheme::divider());
    il->addWidget(infoDiv);

    // Detail rows
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
    il->addStretch();  // push rows to top, panel stretches below
    infoBox->setLayout(il);
    dl->addWidget(infoBox, 1);  // fill full height
    detailPanel->setLayout(dl);

    root->addWidget(imgPanel, 1);
    root->addWidget(detailPanel);
    setLayout(root);
}

void AlbumLightboxScreen::open(const QVector<ShotRecord>& shots,
                                const QString& sessionId,
                                int startIndex)
{
    m_shots = shots;
    m_detailSession->setText(sessionId.isEmpty() ? "—" : sessionId);
    loadShot(startIndex);
}

void AlbumLightboxScreen::loadShot(int idx) {
    if (m_shots.isEmpty()) return;
    m_idx  = qBound(0, idx, m_shots.size() - 1);
    m_zoom = 1.0;
    m_zoomHint->setText("Scroll to zoom  (1.0×)");

    const ShotRecord& s = m_shots[m_idx];
    m_detailShot->setText(QString::number(s.number));
    m_detailScore->setText(QString::number(s.score));

    // Hide arrows at boundaries
    m_prevBtn->setVisible(m_idx > 0);
    m_nextBtn->setVisible(m_idx < m_shots.size() - 1);

    m_pixmap = QPixmap();
    if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath))
        m_pixmap.load(s.imagePath);

    applyZoom();
}

void AlbumLightboxScreen::applyZoom() {
    if (m_pixmap.isNull()) {
        m_imageLabel->setPixmap(QPixmap());
        m_imageLabel->setText(m_shots.isEmpty()
            ? "No Image"
            : QString("Shot #%1 — No Image").arg(m_shots.value(m_idx).number));
        m_imageLabel->resize(m_scrollArea->viewport()->size());
        return;
    }

    // Scale the pixmap, then resize the label to match.
    // The scroll area scrolls if the image is larger than the viewport.
    // ONLY the image content zooms — the rest of the UI is unaffected.
    const QSize baseSize = m_pixmap.size();
    const QSize scaledSize = baseSize * m_zoom;

    const QPixmap scaled = m_pixmap.scaled(scaledSize,
                                            Qt::KeepAspectRatio,
                                            Qt::SmoothTransformation);
    m_imageLabel->setPixmap(scaled);
    m_imageLabel->resize(scaled.size());  // label is exactly the scaled image size
}

void AlbumLightboxScreen::wheelEvent(QWheelEvent* e) {
    // Only intercept scroll when cursor is over the image area
    if (!m_scrollArea->geometry().contains(e->pos()))
        return;

    const double delta = (e->angleDelta().y() > 0) ? 0.15 : -0.15;
    m_zoom = qBound(0.5, m_zoom + delta, 5.0);
    m_zoomHint->setText(QString("Scroll to zoom  (%1×)")
                        .arg(QString::number(m_zoom, 'f', 1)));
    applyZoom();
    e->accept();
}