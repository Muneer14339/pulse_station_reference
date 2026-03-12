// src/ui/review/AlbumLightboxScreen.cpp
#include "AlbumLightboxScreen.h"
#include "common/AppTheme.h"
#include "common/theme/Icons.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QWheelEvent>
#include <QGestureEvent>
#include <QPinchGesture>

static QPushButton* makeArrowBtn(bool isLeft, QWidget* parent) {
    auto* btn = new QPushButton(isLeft ? AppIcons::PrevArrow : AppIcons::NextArrow, parent);
    btn->setFixedSize(52, 80);
    btn->setStyleSheet(AppTheme::navArrowButton());
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFocusPolicy(Qt::NoFocus);
    return btn;
}

AlbumLightboxScreen::AlbumLightboxScreen(QWidget* parent) : QWidget(parent) {
    using namespace AppTheme;

    setAttribute(Qt::WA_StyledBackground, true);
    setAttribute(Qt::WA_AcceptTouchEvents, true);
    grabGesture(Qt::PinchGesture);
    setStyleSheet(AppTheme::lightboxBg());

    auto* root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Left: image area ──────────────────────────────────────────────────
    auto* imgPanel = new QWidget(this);
    imgPanel->setStyleSheet(AppTheme::transparent());
    auto* imgVb = new QVBoxLayout(imgPanel);
    imgVb->setContentsMargins(0, 0, 0, 0);
    imgVb->setSpacing(0);

    // Zoom hint bar
    auto* hintBar = new QWidget(imgPanel);
    hintBar->setStyleSheet(AppTheme::transparent());
    auto* hbl = new QHBoxLayout(hintBar);
    hbl->setContentsMargins(ContentH, ItemGap, ContentH, SpaceS);
    m_zoomHint = new QLabel("Pinch or scroll to zoom  (1.0\u00D7)", hintBar);
    m_zoomHint->setStyleSheet(AppTheme::labelMuted());
    hbl->addWidget(m_zoomHint);
    hbl->addStretch();
    hintBar->setLayout(hbl);
    imgVb->addWidget(hintBar);

    // Nav: prev | scroll-area | next
    auto* navRow = new QWidget(imgPanel);
    navRow->setStyleSheet(AppTheme::transparent());
    auto* nl = new QHBoxLayout(navRow);
    nl->setContentsMargins(ItemGap, 0, ItemGap, ContentV);
    nl->setSpacing(ItemGap);

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

    // ── Right: details panel ──────────────────────────────────────────────
    auto* detailPanel = new QWidget(this);
    detailPanel->setMinimumWidth(SidebarMinW - SpaceXXXL);
    detailPanel->setAttribute(Qt::WA_StyledBackground, true);
    detailPanel->setStyleSheet(AppTheme::sidePanel());

    auto* dl = new QVBoxLayout(detailPanel);
    dl->setContentsMargins(0, 0, 0, 0);

    auto* infoBox = new QWidget(detailPanel);
    infoBox->setAttribute(Qt::WA_StyledBackground, true);
    infoBox->setStyleSheet(AppTheme::summaryBox());
    infoBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* il = new QVBoxLayout(infoBox);
    il->setContentsMargins(CardPadH, CardPadV, CardPadH, CardPadV);
    il->setSpacing(ItemGap);

    // Title row + close button
    auto* titleRow = new QWidget(infoBox);
    titleRow->setStyleSheet(AppTheme::transparent());
    auto* trl = new QHBoxLayout(titleRow);
    trl->setContentsMargins(0, 0, 0, 0);
    trl->setSpacing(InlineGap);
    auto* titleLbl = new QLabel("Shot Details", infoBox);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());
    auto* closeBtn = new QPushButton(AppIcons::Close, infoBox);
    closeBtn->setStyleSheet(AppTheme::iconButton());
    closeBtn->setFixedSize(IconBtnSz - SpaceXS, IconBtnSz - SpaceXS);
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &AlbumLightboxScreen::closeRequested);
    trl->addWidget(titleLbl, 1);
    trl->addWidget(closeBtn);
    titleRow->setLayout(trl);
    il->addWidget(titleRow);

    auto* div = new QWidget(infoBox);
    div->setAttribute(Qt::WA_StyledBackground, true);
    div->setFixedHeight(1);
    div->setStyleSheet(AppTheme::divider());
    il->addWidget(div);

    auto makeRow = [&](const QString& key) -> QLabel* {
        auto* block = new QWidget(infoBox);
        block->setStyleSheet(AppTheme::transparent());
        auto* bl = new QVBoxLayout(block);
        bl->setContentsMargins(0, RowPad, 0, RowPad);
        bl->setSpacing(RowPad);
        auto* k = new QLabel(key, block);
        k->setStyleSheet(AppTheme::labelSecondary());
        auto* v = new QLabel(AppIcons::NoValue, block);
        v->setStyleSheet(AppTheme::summaryRowValue());
        v->setWordWrap(true);
        v->setTextInteractionFlags(Qt::TextSelectableByMouse);
        bl->addWidget(k);
        bl->addWidget(v);
        block->setLayout(bl);
        il->addWidget(block);
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
    m_detailSession->setText(sessionId.isEmpty() ? AppIcons::NoValue : sessionId);
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
    m_zoomHint->setText(QString("Pinch or scroll to zoom  (%1\u00D7)")
                        .arg(QString::number(m_zoom, 'f', 1)));

    if (m_pixmap.isNull()) {
        m_imageLabel->setPixmap(QPixmap());
        m_imageLabel->setText(m_shots.isEmpty()
            ? "No Image"
            : QString("Shot #%1 %2 No Image").arg(m_shots.value(m_idx).number).arg(AppIcons::NoValue));
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