#include "include/ui/setting/Icon.hpp"

#include "include/global/Configs.hpp"

#include <QPainter>

QPixmap Icon::GetTrayIcon(TrayIconStatus status) {
    QPixmap pixmap;

    if (status == NONE)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Off" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == RUNNING)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "nekoray" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == SYSTEM_PROXY_DNS)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Proxy-Dns" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == SYSTEM_PROXY)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Proxy" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == DNS)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Dns" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else if (status == VPN)
    {
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Tun" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    } else
    {
        MW_show_log("Icon::GetTrayIcon: Unknown status");
        auto pixmap_read = QPixmap(QString(":/nekoray/") + "Off" + ".png");
        if (!pixmap_read.isNull()) pixmap = pixmap_read;
    }

    return pixmap;
}
