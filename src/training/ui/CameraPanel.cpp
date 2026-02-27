#include "CameraPanel.h"
#include "common/AppTheme.h"
#include "common/SnackBar.h"

CameraPanel::CameraPanel(QWidget* parent) : QWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_scanPanel = new ScanPanel("Camera Connections", this);
    layout->addWidget(m_scanPanel);
    setLayout(layout);

    connect(m_scanPanel, &ScanPanel::refreshClicked,   this, &CameraPanel::scan);
    connect(m_scanPanel, &ScanPanel::connectClicked,    this, &CameraPanel::onConnectClicked);
    connect(m_scanPanel, &ScanPanel::disconnectClicked, this, &CameraPanel::onDisconnectClicked);
    // No scan here — deferred to showEvent
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
        m_scanPanel->stopScanning();
        for (const auto& cam : cameras) {
            const QString id  = QString::number(cam.index);
            const QString sub = QString("%1 × %2").arg(cam.width).arg(cam.height);
            m_scanPanel->addDevice(id, QString::fromStdString(cam.name), sub);
        }
    });
}

void CameraPanel::onConnectClicked(const QString& id) {
    SnackBar::show(window(), "Connecting camera…", SnackBar::Info);
    m_scanPanel->showConnecting();

    QTimer::singleShot(600, this, [this, id]() {
        m_selectedIndex = id.toInt();
        m_connected = true;

        const auto cameras = get_available_cameras();
        QString name = QString("Camera %1").arg(id);
        for (const auto& cam : cameras)
            if (cam.index == m_selectedIndex)
                name = QString::fromStdString(cam.name);

        m_scanPanel->showConnected(name);
        SnackBar::show(window(), QString("✓ Camera connected: %1").arg(name), SnackBar::Success);
        emit connectionChanged(true, m_selectedIndex);
    });
}

void CameraPanel::onDisconnectClicked() {
    m_scanPanel->showConnecting("Disconnecting…");
    QTimer::singleShot(400, this, [this]() {
        m_connected = false;
        m_selectedIndex = -1;
        m_scanPanel->showDisconnected();
        SnackBar::show(window(), "Camera disconnected", SnackBar::Info);
        emit connectionChanged(false, -1);
        scan();
    });
}