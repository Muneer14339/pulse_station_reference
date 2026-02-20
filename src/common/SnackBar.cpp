#include "SnackBar.h"
#include <QHBoxLayout>
#include <QGraphicsOpacityEffect>

SnackBar::SnackBar(QWidget* parent, const QString& message, Type type, int duration)
    : QWidget(parent), m_message(message), m_type(type), m_duration(duration) {
    setupUI();
    showAnimation();
}

void SnackBar::show(QWidget* parent, const QString& message, Type type, int duration) {
    new SnackBar(parent, message, type, duration);
}

void SnackBar::setupUI() {
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_StyledBackground, true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);

    m_label = new QLabel(m_message, this);
    m_label->setStyleSheet("color: rgb(230, 233, 255); font-size: 13px; background: transparent; border: none;");
    layout->addWidget(m_label);

    QString borderColor = m_type == Success ? "rgba(79, 209, 197, 180)"
                        : m_type == Error   ? "rgba(255, 100, 100, 180)"
                        :                     "rgba(255, 255, 255, 36)";

    setStyleSheet(QString(
        "QWidget { background: rgb(9, 13, 30); border: 1px solid %1; border-radius: 12px; }"
    ).arg(borderColor));

    adjustSize();

    QWidget* p = parentWidget();
    if (p) move(p->width() - width() - 20, 20);

    raise();

    m_timer = new QTimer(this);
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &QWidget::close);
    m_timer->start(m_duration);
}

void SnackBar::showAnimation() {
    auto* effect = new QGraphicsOpacityEffect(this);
    setGraphicsEffect(effect);
    
    auto* anim = new QPropertyAnimation(effect, "opacity", this);
    anim->setDuration(300);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    QWidget::show();
}