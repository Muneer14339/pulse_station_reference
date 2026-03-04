#include "CameraPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"

CameraPanel::CameraPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    m_scanPanel = new ScanPanel("Camera Connections", this);
    layout->addWidget(m_scanPanel, 1);
    setLayout(layout);

    connect(m_scanPanel, &ScanPanel::refreshClicked, this, &CameraPanel::scan);
}

void CameraPanel::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    if (m_firstShow) {
        m_firstShow = false;
        scan();
    }
}

void CameraPanel::scan() {
    if (m_connected) return;

    m_scanPanel->clearDevices();
    m_scanPanel->startScanning();

    QTimer::singleShot(800, this, [this]() {
        const auto cameras = get_available_cameras();

        if (cameras.empty()) {
            m_scanPanel->stopScanning();
            m_scanPanel->setEmptyMessage(
                "No camera found.\nKindly connect a camera and click refresh.");
            return;
        }

        const int pos        = qBound(1, kAutoConnectIndex, (int)cameras.size()) - 1;
        int       targetIdx  = cameras[pos].index;
        QString   targetName = QString::fromStdString(cameras[pos].name);
        autoConnect(targetIdx, targetName);
    });
}

void CameraPanel::autoConnect(int cameraIndex, const QString& name) {
    m_scanPanel->showConnecting("Auto-connecting\u2026");

    QTimer::singleShot(600, this, [this, cameraIndex, name]() {
        m_selectedIndex = cameraIndex;
        m_connected     = true;
        m_scanPanel->showConnected(name, false);
        SnackBar::show(window(),
            QString("\u2713 Camera connected: %1").arg(name), SnackBar::Success);
        emit connectionChanged(true, m_selectedIndex);
    });
}