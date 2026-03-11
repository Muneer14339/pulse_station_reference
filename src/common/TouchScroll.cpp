#include "TouchScroll.h"
#include <QEvent>
#include <QScrollArea>
#include <QScroller>
#include <QScrollerProperties>

// ── Tunables ─────────────────────────────────────────────────────────────
static constexpr double TouchDecelerationFactor = 0.1;
// ─────────────────────────────────────────────────────────────────────────

class TouchScrollInstaller : public QObject {
public:
    explicit TouchScrollInstaller(QObject* parent = nullptr) : QObject(parent) {}
    bool eventFilter(QObject* obj, QEvent* e) override {
        if (e->type() == QEvent::Polish)
            if (auto* area = qobject_cast<QScrollArea*>(obj)) {
                auto* s = QScroller::scroller(area->viewport());
                QScrollerProperties p = s->scrollerProperties();
                p.setScrollMetric(QScrollerProperties::DecelerationFactor, TouchDecelerationFactor);
                s->setScrollerProperties(p);
                QScroller::grabGesture(area->viewport(), QScroller::TouchGesture);
            }
        return false;
    }
};

void installTouchScroll(QApplication& app) {
    app.installEventFilter(new TouchScrollInstaller(&app));
}