#include "HourglassWidget.h"
#include "common/AppColors.h"
#include <QPainter>
#include <QPainterPath>
#include <QPen>

HourglassAnimator& HourglassAnimator::instance() {
    static HourglassAnimator inst;
    return inst;
}

HourglassAnimator::HourglassAnimator(QObject* parent) : QObject(parent) {
    m_ringTimer = new QTimer(this);
    m_ringTimer->setInterval(80);
    connect(m_ringTimer, &QTimer::timeout, this, &HourglassAnimator::renderFrame);
    m_ringTimer->start();

    m_sandTimer = new QTimer(this);
    m_sandTimer->setInterval(800);
    connect(m_sandTimer, &QTimer::timeout, this, [this]() {
        m_sandFrame = (m_sandFrame + 1) % 4;
    });
    m_sandTimer->start();
}

QLabel* HourglassAnimator::createLabel(QWidget* parent) {
    auto* lbl = new QLabel(parent);
    lbl->setFixedSize(Size, Size);
    lbl->setAlignment(Qt::AlignCenter);
    // Label updates automatically when new frame is ready
    connect(this, &HourglassAnimator::frameReady, lbl, &QLabel::setPixmap);
    return lbl;
}

void HourglassAnimator::renderFrame() {
    m_ringAngle = (m_ringAngle + 15) % 360;

    // ── Draw once ─────────────────────────────────────────────────────────────
    QPixmap pm(Size, Size);
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    p.setRenderHint(QPainter::Antialiasing);

    const qreal W  = Size, H = Size, cx = W/2.0;

    const QColor ringColor  = AppColors::Primary();
    const QColor ringTrack  = AppColors::withAlpha(AppColors::Primary(), 40);
    const QColor innerBg    = AppColors::withAlpha(AppColors::Surface(), 220);
    const QColor frameColor = AppColors::PrimaryDark();
    const QColor frameBg    = AppColors::withAlpha(AppColors::PrimaryDark(), 50);
    const QColor sandColor  = AppColors::Primary();
    const QColor sandHilit  = AppColors::PrimaryLight();

    // Outer ring
    const qreal rPad = 2.0, rW = 3.5;
    const QRectF ringRect(rPad, rPad, W-2*rPad, H-2*rPad);
    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(ringTrack, rW, Qt::SolidLine, Qt::RoundCap));
    p.drawEllipse(ringRect);
    p.setPen(QPen(ringColor, rW, Qt::SolidLine, Qt::RoundCap));
    p.drawArc(ringRect, m_ringAngle * 16, 300 * 16);

    // Inner circle
    const qreal iPad = rPad + rW + 2.5;
    const QRectF iRect(iPad, iPad, W-2*iPad, H-2*iPad);
    p.setPen(Qt::NoPen);
    p.setBrush(innerBg);
    p.drawEllipse(iRect);

    // Clip to inner circle
    QPainterPath clipPath; clipPath.addEllipse(iRect);
    p.setClipPath(clipPath);

    const qreal hL  = iRect.left()   + iRect.width()  * 0.10;
    const qreal hR  = iRect.right()  - iRect.width()  * 0.10;
    const qreal hT  = iRect.top()    + iRect.height() * 0.08;
    const qreal hB  = iRect.bottom() - iRect.height() * 0.08;
    const qreal hW  = hR - hL;
    const qreal hcx = iRect.left()   + iRect.width()  * 0.5;
    const qreal hcy = iRect.top()    + iRect.height() * 0.5;
    const qreal nk  = hW * 0.07;

    QPainterPath topShape;
    topShape.moveTo(hL, hT); topShape.lineTo(hR, hT);
    topShape.cubicTo(hR, hT+(hcy-hT)*0.6, hcx+nk, hcy-(hcy-hT)*0.1, hcx+nk, hcy);
    topShape.lineTo(hcx-nk, hcy);
    topShape.cubicTo(hcx-nk, hcy-(hcy-hT)*0.1, hL, hT+(hcy-hT)*0.6, hL, hT);
    topShape.closeSubpath();

    QPainterPath botShape;
    botShape.moveTo(hcx-nk, hcy);
    botShape.cubicTo(hcx-nk, hcy+(hB-hcy)*0.1, hL, hB-(hB-hcy)*0.6, hL, hB);
    botShape.lineTo(hR, hB); botShape.lineTo(hcx+nk, hcy); // shortcut via lineTo is fine
    botShape.cubicTo(hcx+nk, hcy+(hB-hcy)*0.1, hR, hB-(hB-hcy)*0.6, hR, hB);
    // rebuild properly
    botShape = QPainterPath();
    botShape.moveTo(hcx-nk, hcy);
    botShape.cubicTo(hcx-nk, hcy+(hB-hcy)*0.1, hL, hB-(hB-hcy)*0.6, hL, hB);
    botShape.lineTo(hR, hB);
    botShape.cubicTo(hR, hB-(hB-hcy)*0.6, hcx+nk, hcy+(hB-hcy)*0.1, hcx+nk, hcy);
    botShape.closeSubpath();

    p.setBrush(frameBg); p.setPen(Qt::NoPen);
    p.drawPath(topShape); p.drawPath(botShape);

    static const qreal sFracTop[4] = { 0.85, 0.55, 0.25, 0.0 };
    static const qreal sFracBot[4] = { 0.15, 0.45, 0.75, 1.0 };

    if (sFracTop[m_sandFrame] > 0.01) {
        const qreal fillY = hT + (hcy-hT)*(1.0 - sFracTop[m_sandFrame]*0.9);
        QPainterPath c; c.addRect(QRectF(hL-2, fillY, hW+4, hcy-fillY+2));
        p.setBrush(sandColor);
        p.drawPath(topShape.intersected(c));
        p.setPen(QPen(sandHilit, 1.0, Qt::SolidLine, Qt::RoundCap));
        p.drawLine(QPointF(hcx-hW*0.2, fillY+1), QPointF(hcx+hW*0.15, fillY+1));
        p.setPen(Qt::NoPen);
    }

    if (sFracBot[m_sandFrame] > 0.01) {
        const qreal fillY = hB - (hB-hcy)*(sFracBot[m_sandFrame]*0.9);
        QPainterPath c; c.addRect(QRectF(hL-2, fillY-2, hW+4, hB-fillY+4));
        p.setBrush(sandColor);
        p.drawPath(botShape.intersected(c));
    }

    p.setBrush(Qt::NoBrush);
    p.setPen(QPen(frameColor, 1.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    p.drawPath(topShape); p.drawPath(botShape);

    p.setPen(Qt::NoPen); p.setBrush(frameColor);
    const qreal capH = iRect.height() * 0.10;
    p.drawRoundedRect(QRectF(hL-1, hT-capH, hW+2, capH+1), 2, 2);
    p.drawRoundedRect(QRectF(hL-1, hB,      hW+2, capH+1), 2, 2);

    p.setClipping(false);
    if (m_sandFrame == 1 || m_sandFrame == 2) {
        p.setBrush(sandHilit); p.setPen(Qt::NoPen);
        p.drawEllipse(QPointF(cx, hcy + (m_sandFrame==2 ? iRect.height()*0.06 : 0.0)), 1.8, 1.8);
    }

    p.end();

    // ── Broadcast to ALL labels — zero extra render cost ──────────────────────
    emit frameReady(pm);
}