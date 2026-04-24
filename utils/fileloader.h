#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>

struct FileResult
{
    bool        success  { false };
    QString     filePath;
    QString     fileName;
    QStringList lines;
    QString     fullText;
    QString     encoding;
    qint64      fileSize { 0 };
    QString     errorMessage;
};

class FileLoader
{
public:
    static FileResult load(const QString& filePath);

    static FileResult openWithDialog(QWidget* parent,
                                     const QString& caption,
                                     const QString& lastDirectory = QString());

    static QString formatSize(qint64 bytes);

private:
    FileLoader() = delete;
};
