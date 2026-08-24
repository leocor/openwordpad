#pragma once

#include <QDialog>

namespace OpenWordPad {

class AboutDialog : public QDialog {
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = nullptr);
};

} // namespace OpenWordPad
