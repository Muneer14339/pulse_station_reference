#pragma once
#include <QWidget>
#include <QLabel>
#include <QMap>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>

/**
 * @brief Reusable scan-and-connect panel (shared by BluetoothPanel and CameraPanel).
 *
 * Handles all visual states: scanning, device list, connecting, connected,
 * disconnected, empty. Callers drive state via the public API; this widget
 * only emits UI events back (refresh, connect, disconnect clicked).
 */
class ScanPanel : public QWidget {
    Q_OBJECT
public:
    explicit ScanPanel(const QString& title, QWidget* parent = nullptr);

    // ── State API ────────────────────────────────────────────────────────
    void startScanning();
    void stopScanning();
    void addDevice(const QString& id, const QString& primary, const QString& secondary);
    void clearDevices();
    void showConnecting(const QString& msg = "Connecting…");
    void showConnected(const QString& name);
    void showDisconnected();
    /** Show/hide the entire content area (e.g. when adapter is off). */
    void setContentVisible(bool visible);

signals:
    void refreshClicked();
    void connectClicked(const QString& id);
    void disconnectClicked();

private:
    void buildUI(const QString& title);
    void tickDots();

    QPushButton* m_refreshBtn;
    QWidget*     m_devicesContainer;
    QVBoxLayout* m_devicesLayout;
    QScrollArea* m_devicesScroll;
    QWidget*     m_scanLoader;
    QWidget*     m_connectLoader;
    QWidget*     m_emptyState;

    QTimer* m_dotTimer;
    int     m_dotCount = 0;
    QMap<QString, QWidget*> m_deviceWidgets;
};
