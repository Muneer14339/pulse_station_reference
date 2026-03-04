#include "SnackBar.h"
#include "AppTheme.h"
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>

void SnackBar::show(QWidget* parent, const QString& message, Type type, int duration) {
    new SnackBar(parent, message, type, duration);
}

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
    layout->setContentsMargins(AppTheme::SpaceXXL, AppTheme::SpaceM,
                               AppTheme::SpaceXXL, AppTheme::SpaceM);

    m_label = new QLabel(m_message, this);
    m_label->setStyleSheet(AppTheme::snackBarText());
    layout->addWidget(m_label);

    // Use AppColors directly — avoids the QColor("rgba(...)") parse failure
    const QColor borderColor =
        (m_type == Success) ? AppColors::withAlpha(AppColors::Accent(),       180)
      : (m_type == Error)   ? AppColors::withAlpha(AppColors::Error(),        180)
                            : AppColors::withAlpha(AppColors::TextPrimary(),   36);

    setStyleSheet(AppTheme::snackBar(borderColor));
    adjustSize();

    if (QWidget* p = parentWidget())
        move(p->width() - width() - AppTheme::SpaceXXL, AppTheme::SpaceXXL);

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