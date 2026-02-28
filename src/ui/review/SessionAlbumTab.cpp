#include "SessionAlbumTab.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFrame>
#include <QWheelEvent>
#include <QPushButton>

// ── Lightbox overlay ──────────────────────────────────────────────────────────
class AlbumLightbox : public QWidget {
public:
    AlbumLightbox(const QVector<ShotRecord>& shots, int startIdx, QWidget* parent)
        : QWidget(parent), m_shots(shots), m_idx(startIdx)
    {
        setAttribute(Qt::WA_StyledBackground, true);
        setStyleSheet(AppTheme::panel());
        setGeometry(parent->rect());
        raise();
        show();
        buildUI();
        loadShot(m_idx);
    }

private:
    void buildUI() {
        auto* root = new QVBoxLayout(this);
        root->setContentsMargins(40, 30, 40, 30);
        root->setSpacing(16);

        auto* topRow = new QWidget(this);
        topRow->setStyleSheet(AppTheme::transparent());
        auto* tl = new QHBoxLayout(topRow);
        tl->setContentsMargins(0, 0, 0, 0);
        tl->addStretch();
        auto* closeBtn = new QPushButton("✕", topRow);
        closeBtn->setStyleSheet(AppTheme::refreshButton());
        closeBtn->setFixedSize(36, 36);
        closeBtn->setCursor(Qt::PointingHandCursor);
        connect(closeBtn, &QPushButton::clicked, this, &QWidget::deleteLater);
        tl->addWidget(closeBtn);
        topRow->setLayout(tl);
        root->addWidget(topRow);

        auto* midRow = new QWidget(this);
        midRow->setStyleSheet(AppTheme::transparent());
        auto* ml = new QHBoxLayout(midRow);
        ml->setContentsMargins(0, 0, 0, 0);
        ml->setSpacing(12);

        m_prevBtn = new QPushButton("‹", midRow);
        m_prevBtn->setStyleSheet(AppTheme::buttonGhost());
        m_prevBtn->setFixedWidth(48);
        m_prevBtn->setCursor(Qt::PointingHandCursor);
        connect(m_prevBtn, &QPushButton::clicked, this, [this]{ loadShot(m_idx - 1); });

        m_imageLabel = new QLabel(midRow);
        m_imageLabel->setAlignment(Qt::AlignCenter);
        m_imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        m_imageLabel->setStyleSheet(AppTheme::cameraView());
        m_imageLabel->setMinimumSize(400, 300);

        m_nextBtn = new QPushButton("›", midRow);
        m_nextBtn->setStyleSheet(AppTheme::buttonGhost());
        m_nextBtn->setFixedWidth(48);
        m_nextBtn->setCursor(Qt::PointingHandCursor);
        connect(m_nextBtn, &QPushButton::clicked, this, [this]{ loadShot(m_idx + 1); });

        ml->addWidget(m_prevBtn);
        ml->addWidget(m_imageLabel, 1);
        ml->addWidget(m_nextBtn);
        midRow->setLayout(ml);
        root->addWidget(midRow, 1);

        auto* detailBox = new QWidget(this);
        detailBox->setAttribute(Qt::WA_StyledBackground, true);
        detailBox->setStyleSheet(AppTheme::summaryBox());
        auto* dl = new QHBoxLayout(detailBox);
        dl->setContentsMargins(16, 10, 16, 10);

        m_shotNumLbl = new QLabel(detailBox);
        m_shotNumLbl->setStyleSheet(AppTheme::sectionTitle());
        m_scoreLbl = new QLabel(detailBox);
        m_scoreLbl->setStyleSheet(AppTheme::summaryRowHighlight());
        m_zoomLbl = new QLabel("Scroll to zoom", detailBox);
        m_zoomLbl->setStyleSheet(AppTheme::labelMuted());

        dl->addWidget(m_shotNumLbl);
        dl->addSpacing(24);
        dl->addWidget(m_scoreLbl);
        dl->addStretch();
        dl->addWidget(m_zoomLbl);
        detailBox->setLayout(dl);
        root->addWidget(detailBox);
        setLayout(root);
    }

    void loadShot(int idx) {
        if (m_shots.isEmpty()) return;
        m_idx = qBound(0, idx, m_shots.size() - 1);
        m_zoom = 1.0;
        const ShotRecord& s = m_shots[m_idx];
        m_shotNumLbl->setText(QString("Shot #%1").arg(s.number));
        m_scoreLbl->setText(QString("Score: %1").arg(s.score));
        m_zoomLbl->setText("Scroll to zoom  (1.0×)");
        m_prevBtn->setEnabled(m_idx > 0);
        m_nextBtn->setEnabled(m_idx < m_shots.size() - 1);

        if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath)) {
            m_pixmap.load(s.imagePath);
            updateImage();
        } else {
            m_imageLabel->setPixmap(QPixmap());
            m_imageLabel->setText(QString("Shot #%1 — No Image").arg(s.number));
        }
    }

    void updateImage() {
        if (m_pixmap.isNull()) return;
        QSize sz = m_imageLabel->size() * m_zoom;
        m_imageLabel->setPixmap(
            m_pixmap.scaled(sz, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    void wheelEvent(QWheelEvent* e) override {
        m_zoom = qBound(0.5, m_zoom + (e->angleDelta().y() > 0 ? 0.1 : -0.1), 4.0);
        m_zoomLbl->setText(QString("Scroll to zoom  (%1×)")
                           .arg(QString::number(m_zoom, 'f', 1)));
        updateImage();
    }

    const QVector<ShotRecord>& m_shots;
    int          m_idx   = 0;
    double       m_zoom  = 1.0;
    QPixmap      m_pixmap;
    QLabel*      m_imageLabel  = nullptr;
    QLabel*      m_shotNumLbl  = nullptr;
    QLabel*      m_scoreLbl    = nullptr;
    QLabel*      m_zoomLbl     = nullptr;
    QPushButton* m_prevBtn     = nullptr;
    QPushButton* m_nextBtn     = nullptr;
};

// ─────────────────────────────────────────────────────────────────────────────

SessionAlbumTab::SessionAlbumTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());
    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(20, 20, 20, 20);
    vb->setSpacing(12);

    auto* title = new QLabel("Session Album", this);
    title->setStyleSheet(AppTheme::sectionTitle());
    vb->addWidget(title);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    auto* container = new QWidget;
    container->setStyleSheet(AppTheme::transparent());
    m_grid = new QGridLayout(container);
    m_grid->setContentsMargins(0, 0, 0, 0);
    m_grid->setSpacing(12);
    container->setLayout(m_grid);
    scroll->setWidget(container);
    vb->addWidget(scroll, 1);
    setLayout(vb);
}

QWidget* SessionAlbumTab::makeCard(const ShotRecord& s, int index) {
    auto* card = new QWidget(this);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setStyleSheet(AppTheme::deviceCard());
    card->setCursor(Qt::PointingHandCursor);
    auto* vb = new QVBoxLayout(card);
    vb->setContentsMargins(8, 8, 8, 8);
    vb->setSpacing(6);

    auto* imgLbl = new QLabel(card);
    imgLbl->setFixedHeight(120);
    imgLbl->setAlignment(Qt::AlignCenter);
    imgLbl->setStyleSheet(AppTheme::cameraView());

    if (!s.imagePath.isEmpty() && QFile::exists(s.imagePath)) {
        QPixmap px(s.imagePath);
        if (!px.isNull())
            imgLbl->setPixmap(
                px.scaled(imgLbl->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        else
            imgLbl->setText(QString("Shot #%1").arg(s.number));
    } else {
        imgLbl->setText(QString("Shot #%1").arg(s.number));
    }

    auto* caption = new QLabel(
        QString("Shot %1  ·  Score %2").arg(s.number).arg(s.score), card);
    caption->setStyleSheet(AppTheme::labelMuted());
    caption->setAlignment(Qt::AlignCenter);

    vb->addWidget(imgLbl);
    vb->addWidget(caption);
    card->setLayout(vb);

    // Click overlay
    auto* clickArea = new QPushButton(card);
    clickArea->setGeometry(0, 0, 999, 999);
    clickArea->setStyleSheet(AppTheme::transparent());
    clickArea->lower();
    int i = index;
    connect(clickArea, &QPushButton::clicked, this, [this, i]{ showLightbox(i); });

    return card;
}

void SessionAlbumTab::populate(const SessionResult& r) {
    while (m_grid->count()) {
        auto* item = m_grid->takeAt(0);
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }
    m_shots = r.shots;

    const int cols = 5;
    for (int i = 0; i < m_shots.size(); ++i)
        m_grid->addWidget(makeCard(m_shots[i], i), i / cols, i % cols);
}

void SessionAlbumTab::showLightbox(int index) {
    new AlbumLightbox(m_shots, index, this);
}