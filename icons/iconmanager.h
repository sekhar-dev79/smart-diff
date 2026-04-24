#pragma once

#include <QIcon>
#include <QHash>
#include <QString>
#include <QSize>
#include <QColor>

class IconManager
{
public:
    static IconManager* instance();

    QIcon icon(const QString& name,
               const QColor&  color,
               const QSize&   size = QSize(16, 16)) const;

    QIcon icon(const QString& name,
               const QSize&   size = QSize(16, 16)) const;

    QIcon mutedIcon(const QString& name,
                    const QSize&   size = QSize(16, 16)) const;

    void clearCache();

private:
    IconManager() = default;

    QIcon renderSvg(const QString& resourcePath,
                    const QColor&  color,
                    const QSize&   size) const;

    mutable QHash<QString, QIcon> m_cache;
    static IconManager* s_instance;
};
