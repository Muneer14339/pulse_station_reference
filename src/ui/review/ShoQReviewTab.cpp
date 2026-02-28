#include "ShoQReviewTab.h"
#include "common/AppTheme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>
#include <QPushButton>

static QWidget* makeDivider(QWidget* parent) {
    auto* d = new QWidget(parent);
    d->setFixedHeight(1);
    d->setStyleSheet(AppTheme::divider());
    return d;
}

static QWidget* makeInsightBox(const ShoQReviewData& d, QWidget* parent) {
    auto* box = new QWidget(parent);
    box->setAttribute(Qt::WA_StyledBackground, true);
    box->setStyleSheet(AppTheme::summaryBox());
    auto* vb = new QVBoxLayout(box);
    vb->setContentsMargins(16, 14, 16, 14);
    vb->setSpacing(8);

    auto* title = new QLabel("ShoQ Insight", box);
    title->setStyleSheet(AppTheme::sectionTitle());
    vb->addWidget(title);
    vb->addWidget(makeDivider(box));

    auto addInsight = [&](const QString& label, const QString& val) {
        auto* row = new QWidget(box);
        row->setStyleSheet(AppTheme::transparent());
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(0, 2, 0, 2);
        auto* k = new QLabel(label, row);
        k->setStyleSheet(AppTheme::summaryRowLabel());
        auto* v = new QLabel(val, row);
        v->setStyleSheet(AppTheme::summaryRowValue());
        v->setWordWrap(true);
        rl->addWidget(k, 0);
        rl->addSpacing(12);
        rl->addWidget(v, 1);
        row->setLayout(rl);
        vb->addWidget(row);
    };

    addInsight("STRENGTH",  d.insightStrength);
    addInsight("FOCUS",     d.insightFocus);
    addInsight("CHALLENGE", d.insightChallenge);
    box->setLayout(vb);
    return box;
}

static QWidget* makeMetricCard(const ShoQMetric& m, QWidget* parent) {
    auto* card = new QWidget(parent);
    card->setAttribute(Qt::WA_StyledBackground, true);
    card->setStyleSheet(AppTheme::summaryBox());
    auto* vb = new QVBoxLayout(card);
    vb->setContentsMargins(14, 12, 14, 12);
    vb->setSpacing(4);

    auto* pctLbl = new QLabel(QString::number(m.pct) + "%", card);
    pctLbl->setStyleSheet(AppTheme::scoreValue());

    auto* nameLbl = new QLabel(m.name, card);
    nameLbl->setStyleSheet(AppTheme::sectionTitle());

    auto* descLbl = new QLabel(m.desc, card);
    descLbl->setStyleSheet(AppTheme::labelMuted());
    descLbl->setWordWrap(true);

    vb->addWidget(pctLbl);
    vb->addWidget(nameLbl);
    vb->addWidget(descLbl);
    card->setLayout(vb);
    return card;
}

static QWidget* makeRatingBox(const ShoQReviewData& d, QWidget* parent) {
    auto* box = new QWidget(parent);
    box->setAttribute(Qt::WA_StyledBackground, true);
    box->setStyleSheet(AppTheme::summaryBox());
    auto* hl = new QHBoxLayout(box);
    hl->setContentsMargins(16, 14, 16, 14);
    hl->setSpacing(20);

    // Rating letter — use summaryRowHighlight (orange) for the grade
    auto* ratingLbl = new QLabel(d.rating, box);
    ratingLbl->setStyleSheet(AppTheme::heroTitle());
    ratingLbl->setFixedWidth(70);
    ratingLbl->setAlignment(Qt::AlignCenter);

    // Wrap in a small panel so the label stands out with orange accent
    auto* ratingWrap = new QWidget(box);
    ratingWrap->setAttribute(Qt::WA_StyledBackground, true);
    ratingWrap->setStyleSheet(AppTheme::summaryBox());
    ratingWrap->setFixedWidth(80);
    auto* rwl = new QVBoxLayout(ratingWrap);
    rwl->setContentsMargins(4, 8, 4, 8);
    // Override text color to accent orange via summaryRowHighlight font size match
    auto* gradeLabel = new QLabel(d.rating, ratingWrap);
    gradeLabel->setStyleSheet(AppTheme::summaryRowHighlight() + " font-size: 34px; font-weight: 600;");
    gradeLabel->setAlignment(Qt::AlignCenter);
    rwl->addWidget(gradeLabel);
    ratingWrap->setLayout(rwl);

    auto* vb = new QVBoxLayout;
    vb->setSpacing(4);
    auto* titleLbl = new QLabel("Session Rating", box);
    titleLbl->setStyleSheet(AppTheme::sectionTitle());
    auto* descLbl = new QLabel(d.ratingDesc, box);
    descLbl->setStyleSheet(AppTheme::labelSecondary());
    descLbl->setWordWrap(true);
    vb->addWidget(titleLbl);
    vb->addWidget(descLbl);

    hl->addWidget(ratingWrap);
    hl->addLayout(vb, 1);
    box->setLayout(hl);
    return box;
}

// ─────────────────────────────────────────────────────────────────────────────

ShoQReviewTab::ShoQReviewTab(QWidget* parent) : QWidget(parent) {
    setStyleSheet(AppTheme::transparent());
    auto* outerVb = new QVBoxLayout(this);
    outerVb->setContentsMargins(0, 0, 0, 0);
    outerVb->setSpacing(0);

    auto* scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setStyleSheet(AppTheme::scrollArea());
    scroll->setFrameShape(QFrame::NoFrame);

    m_content = new QWidget;
    m_content->setStyleSheet(AppTheme::transparent());
    m_content->setLayout(new QVBoxLayout(m_content));
    scroll->setWidget(m_content);
    outerVb->addWidget(scroll);
    setLayout(outerVb);
}

void ShoQReviewTab::populate(const SessionResult& r) {
    delete m_content->layout();
    const auto children = m_content->children();
    for (auto* c : children)
        if (qobject_cast<QWidget*>(c)) delete c;

    auto* vb = new QVBoxLayout(m_content);
    vb->setContentsMargins(20, 20, 20, 32);
    vb->setSpacing(16);

    // Header
    {
        auto* row = new QWidget(m_content);
        row->setStyleSheet(AppTheme::transparent());
        auto* hl = new QHBoxLayout(row);
        hl->setContentsMargins(0, 0, 0, 0);
        auto* sessionLbl = new QLabel(
            QString("Session: <b>%1</b> · Shooter: <b>%2</b>")
                .arg(r.params.sessionId, r.params.shooterId), row);
        sessionLbl->setStyleSheet(AppTheme::labelSecondary());
        hl->addWidget(sessionLbl);
        hl->addStretch();
        row->setLayout(hl);
        vb->addWidget(row);
    }

    // Review text
    {
        auto* box = new QWidget(m_content);
        box->setAttribute(Qt::WA_StyledBackground, true);
        box->setStyleSheet(AppTheme::summaryBox());
        auto* bvb = new QVBoxLayout(box);
        bvb->setContentsMargins(16, 14, 16, 14);
        bvb->setSpacing(8);
        auto* t = new QLabel("AI Review", box);
        t->setStyleSheet(AppTheme::sectionTitle());
        bvb->addWidget(t);
        bvb->addWidget(makeDivider(box));
        auto* txt = new QLabel(r.shoq.reviewText, box);
        txt->setStyleSheet(AppTheme::labelSecondary());
        txt->setWordWrap(true);
        bvb->addWidget(txt);
        box->setLayout(bvb);
        vb->addWidget(box);
    }

    vb->addWidget(makeInsightBox(r.shoq, m_content));

    // Metric cards 2-column
    {
        auto* sectionLbl = new QLabel("Performance Metrics", m_content);
        sectionLbl->setStyleSheet(AppTheme::sectionTitle());
        vb->addWidget(sectionLbl);

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

    // Skill focus
    if (!r.shoq.skillFocusTip.isEmpty()) {
        auto* box = new QWidget(m_content);
        box->setAttribute(Qt::WA_StyledBackground, true);
        box->setStyleSheet(AppTheme::summaryBox());
        auto* bvb = new QVBoxLayout(box);
        bvb->setContentsMargins(16, 14, 16, 14); bvb->setSpacing(8);
        auto* t = new QLabel("Skill Focus", box);
        t->setStyleSheet(AppTheme::sectionTitle());
        bvb->addWidget(t);
        bvb->addWidget(makeDivider(box));
        auto* tip = new QLabel(r.shoq.skillFocusTip, box);
        tip->setStyleSheet(AppTheme::labelSecondary());
        tip->setWordWrap(true);
        bvb->addWidget(tip);
        box->setLayout(bvb);
        vb->addWidget(box);
    }

    vb->addWidget(makeRatingBox(r.shoq, m_content));

    // Find a Coach CTA
    {
        auto* row = new QWidget(m_content);
        row->setStyleSheet(AppTheme::transparent());
        auto* hl = new QHBoxLayout(row);
        hl->setContentsMargins(0, 0, 0, 0);
        hl->addStretch();
        auto* btn = new QPushButton("Find a Coach", row);
        btn->setStyleSheet(AppTheme::buttonPrimary());
        btn->setCursor(Qt::PointingHandCursor);
        hl->addWidget(btn);
        row->setLayout(hl);
        vb->addWidget(row);
    }

    vb->addStretch();
    m_content->setLayout(vb);
}