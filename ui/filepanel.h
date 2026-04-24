#pragma once

#include <QWidget>
#include <QLabel>
#include "linenumgutter.h"

class FilePanel : public QWidget
{
    Q_OBJECT

public:
    explicit FilePanel(const QString& side, QWidget* parent = nullptr);

    EditorWithGutter* editor() const { return m_editor; }
    void setFileName(const QString& name);
    void setMetaInfo(const QString& info);

private:
    void buildLayout(const QString& side);

    EditorWithGutter* m_editor    { nullptr };
    LineNumGutter* m_gutter    { nullptr };
    QLabel* m_fileLabel { nullptr };
    QLabel* m_metaLabel { nullptr };
    QWidget* m_header    { nullptr };
};
