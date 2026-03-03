// src/ui/review/ShoQReviewTab.cpp
#include "ShoQReviewTab.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>

// ─── Helpers ──────────────────────────────────────────────────────────────────

// One bordered section box: title + divider + content added via returned layout
static QWidget* makeSectionBox(const QString& title, QVBoxLayout*& body, QWidget* parent) {
    auto* box = new QWidget(parent);
    box->setAttribute(Qt::WA_StyledBackground, true);
    box->setStyleSheet(AppTheme::summaryBox());
    body = new QVBoxLayout(box);
    body->setContentsMargins(16, 14, 16, 16);
    body->setSpacing(10);
    auto* t = new QLabel(title, box);
    t->setStyleSheet(AppTheme::sectionTitle());
    body->addWidget(t);
    auto* d = new QWidget(box);
    d->setFixedHeight(1);
    d->setStyleSheet(AppTheme::divider());
    body->addWidget(d);
    box->setLayout(body);
    return box;
}

// Key-value row inside a transparent parent
static void addRow(QVBoxLayout* vb, QWidget* parent,
                   const QString& key, const QString& val,
                   int keyWidth = 0)
{
    auto* row = new QWidget(parent);
    row->setStyleSheet(AppTheme::transparent());
    auto* rl = new QHBoxLayout(row);
    rl->setContentsMargins(0, 2, 0, 2);
    rl->setSpacing(12);
    auto* k = new QLabel(key, row);
    k->setStyleSheet(AppTheme::summaryRowLabel());
    if (keyWidth > 0) k->setFixedWidth(keyWidth);
    auto* v = new QLabel(val, row);
    v->setStyleSheet(AppTheme::summaryRowValue());
    v->setWordWrap(true);
    rl->addWidget(k);
    rl->addWidget(v, 1);
    row->setLayout(rl);
    vb->addWidget(row);
}

// ─── Metric card: ONE summaryBox border, no inner nested boxes/borders ────────
static QWidget* makeMetricCard(const ShoQMetric& m, QWidget* parent) {
    auto* card = new QWidget(parent);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setStyleSheet(AppTheme::summaryBox());
    auto* vb = new QVBoxLayout(card);
    vb->setContentsMargins(16, 14, 16, 14);
    vb->setSpacing(4);

    // scoreValue() = large white number, transparent bg — guaranteed no border
    auto* pct = new QLabel(QString::number(m.pct) + "%", card);
    pct->setStyleSheet(AppTheme::scoreValue());

    auto* name = new QLabel(m.name, card);
    name->setStyleSheet(AppTheme::sectionTitle());

    auto* desc = new QLabel(m.desc, card);
    desc->setStyleSheet(AppTheme::labelMuted());
    desc->setWordWrap(true);

    vb->addWidget(pct);
    vb->addWidget(name);
    vb->addWidget(desc);
    card->setLayout(vb);
    return card;
}

// ─── ShoQReviewTab ────────────────────────────────────────────────────────────
ShoQReviewTab::ShoQReviewTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    m_content = new QWidget;
    m_content->setStyleSheet(AppTheme::transparent());
    m_content->setLayout(new QVBoxLayout(m_content));
    scroll->setWidget(m_content);

    auto* vb = new QVBoxLayout(this);
    vb->setContentsMargins(0, 0, 0, 0);
    vb->addWidget(scroll);
    setLayout(vb);
}

void ShoQReviewTab::populate(const SessionResult& r) {
    // Wipe previous
    delete m_content->layout();
    const auto ch = m_content->children();
    for (auto* c : ch)
        if (qobject_cast<QWidget*>(c)) delete c;

    auto* vb = new QVBoxLayout(m_content);
    vb->setContentsMargins(20, 16, 20, 32);
    vb->setSpacing(14);

    // Header
    auto* hdr = new QLabel(
        QString("Session: <b>%1</b>  ·  Shooter: <b>%2</b>")
            .arg(r.params.sessionId, r.params.shooterId), m_content);
    hdr->setStyleSheet(AppTheme::labelSecondary());
    vb->addWidget(hdr);

    // AI Review
    {
        QVBoxLayout* bl = nullptr;
        auto* box = makeSectionBox("AI Review", bl, m_content);
        auto* txt = new QLabel(r.shoq.reviewText, box);
        txt->setStyleSheet(AppTheme::labelSecondary());
        txt->setWordWrap(true);
        bl->addWidget(txt);
        vb->addWidget(box);
    }

    // ShoQ Insight
    {
        QVBoxLayout* bl = nullptr;
        auto* box = makeSectionBox("ShoQ Insight", bl, m_content);
        addRow(bl, box, "STRENGTH",  r.shoq.insightStrength,  80);
        addRow(bl, box, "FOCUS",     r.shoq.insightFocus,     80);
        addRow(bl, box, "CHALLENGE", r.shoq.insightChallenge, 80);
        vb->addWidget(box);
    }

    // Performance Metrics: section label + 2-column cards
    {
        auto* lbl = new QLabel("Performance Metrics", m_content);
        lbl->setStyleSheet(AppTheme::sectionTitle());
        vb->addWidget(lbl);

        auto* grid = new QWidget(m_content);
        grid->setStyleSheet(AppTheme::transparent());
        auto* gl = new QHBoxLayout(grid);
        gl->setContentsMargins(0, 0, 0, 0);
        gl->setSpacing(12);
        auto* col1 = new QVBoxLayout; col1->setSpacing(12);
        auto* col2 = new QVBoxLayout; col2->setSpacing(12);
        for (int i = 0; i < r.shoq.metrics.size(); ++i)
            (i % 2 == 0 ? col1 : col2)->addWidget(
                makeMetricCard(r.shoq.metrics[i], grid));
        gl->addLayout(col1, 1);
        gl->addLayout(col2, 1);
        grid->setLayout(gl);
        vb->addWidget(grid);
    }

    // Skill Focus
    if (!r.shoq.skillFocusTip.isEmpty()) {
        QVBoxLayout* bl = nullptr;
        auto* box = makeSectionBox("Skill Focus", bl, m_content);
        auto* tip = new QLabel(r.shoq.skillFocusTip, box);
        tip->setStyleSheet(AppTheme::labelSecondary());
        tip->setWordWrap(true);
        bl->addWidget(tip);
        vb->addWidget(box);
    }

    // Session Rating: grade (AppTheme::gradeLabel) + description
    {
        auto* box = new QWidget(m_content);
        box->setAttribute(Qt::WA_StyledBackground, true);
        box->setStyleSheet(AppTheme::summaryBox());
        auto* hl = new QHBoxLayout(box);
        hl->setContentsMargins(16, 14, 16, 14);
        hl->setSpacing(20);

        auto* grade = new QLabel(r.shoq.rating, box);
        grade->setStyleSheet(AppTheme::gradeLabel());  // AppTheme: 34px bold orange
        grade->setFixedWidth(60);
        grade->setAlignment(Qt::AlignCenter);

        auto* infoVb = new QVBoxLayout;
        infoVb->setSpacing(4);
        auto* rTitle = new QLabel("Session Rating", box);
        rTitle->setStyleSheet(AppTheme::sectionTitle());
        auto* rDesc = new QLabel(r.shoq.ratingDesc, box);
        rDesc->setStyleSheet(AppTheme::labelSecondary());
        rDesc->setWordWrap(true);
        infoVb->addWidget(rTitle);
        infoVb->addWidget(rDesc);

        hl->addWidget(grade);
        hl->addLayout(infoVb, 1);
        box->setLayout(hl);
        vb->addWidget(box);
    }

    // CTA
    {
        auto* row = new QWidget(m_content);
        row->setStyleSheet(AppTheme::transparent());
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 0, 0, 0);
        rl->addStretch();
        auto* btn = new QPushButton("Find a Coach", row);
        btn->setStyleSheet(AppTheme::buttonPrimary());
        btn->setCursor(Qt::PointingHandCursor);
        rl->addWidget(btn);
        row->setLayout(rl);
        vb->addWidget(row);
    }

    vb->addStretch();
    m_content->setLayout(vb);
}