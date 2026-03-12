#pragma once
#include "ui/widgets/HourglassWidget.h"

namespace AppIcons {

// ── Pending hourglass — flyweight, 1 render shared across all usages ──────────
inline QLabel* pendingLabel(QWidget* parent = nullptr) {
    return HourglassAnimator::instance().createLabel(parent);
}

} // namespace AppIcons