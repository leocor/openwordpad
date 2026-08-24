#pragma once

#include <QDialog>
#include <QListWidget>

namespace OpenWordPad {

class DateTimeDialog : public QDialog {
    Q_OBJECT

public:
    explicit DateTimeDialog(QWidget *parent = nullptr);

    QString selectedDateTimeString() const;

private:
    QListWidget *m_listFormats;
};

} // namespace OpenWordPad
