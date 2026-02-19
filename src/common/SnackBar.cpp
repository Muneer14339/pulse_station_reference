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
    setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setAttribute(Qt::WA_DeleteOnClose);
    
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(16, 12, 16, 12);
    
    m_label = new QLabel(m_message, this);
    m_label->setStyleSheet("color: rgb(245, 245, 255); font-size: 13px; background: transparent; border: none;");
    layout->addWidget(m_label);
    
    QString bgColor;
    if (m_type == Success) bgColor = "rgba(46, 204, 113, 242)";
    else if (m_type == Error) bgColor = "rgba(231, 76, 60, 242)";
    else bgColor = "rgba(52, 73, 94, 242)";
    
    setStyleSheet(QString(
        "QWidget { background: %1; border: 1px solid rgba(255,255,255,77); border-radius: 10px; }"
    ).arg(bgColor));
    
    adjustSize();
    
    QWidget* p = parentWidget();
    if (p) {
        int x = p->width() - width() - 20;
        int y = 80;
        move(p->mapToGlobal(QPoint(x, y)));
    }
    
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