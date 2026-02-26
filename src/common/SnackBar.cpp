#include "SnackBar.h"
#include "AppTheme.h"
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>

// ── Public factory ────────────────────────────────────────────────────────────
void SnackBar::show(QWidget* parent, const QString& message, Type type, int duration) {
    new SnackBar(parent, message, type, duration);
}

// ── Private constructor ───────────────────────────────────────────────────────
SnackBar::SnackBar(QWidget* parent, const QString& message, Type type, int duration)
    : QWidget(parent), m_message(message), m_type(type), m_duration(duration)
{
    buildUI();
    animateIn();
}

void SnackBar::buildUI() {
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(18, 13, 18, 13);

    m_label = new QLabel(m_message, this);
    m_label->setStyleSheet(AppTheme::snackBarText());
    layout->addWidget(m_label);

    const QString borderColor = (m_type == Success) ? "rgba(79, 209, 197, 180)"
                              : (m_type == Error)   ? "rgba(255, 100, 100, 180)"
                                                    : "rgba(255, 255, 255, 36)";
    setStyleSheet(AppTheme::snackBar(borderColor));
    adjustSize();

    if (QWidget* p = parentWidget())
        move(p->width() - width() - 20, 20);

    raise();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &QWidget::close);
    m_timer->start(m_duration);
}

void SnackBar::animateIn() {
    auto* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);

    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(300);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::show();
}
