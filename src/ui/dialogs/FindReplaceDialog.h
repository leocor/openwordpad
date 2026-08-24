#pragma once

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QPushButton>
#include <QTextDocument>

namespace OpenWordPad {

class FindReplaceDialog : public QDialog {
    Q_OBJECT

public:
    explicit FindReplaceDialog(bool isReplaceMode, QWidget *parent = nullptr);

    void setReplaceMode(bool replace);
    QString findText() const;
    QString replaceText() const;
    QTextDocument::FindFlags findFlags() const;

signals:
    void findNextRequested(const QString &text, QTextDocument::FindFlags flags);
    void replaceRequested(const QString &findText, const QString &replaceText, QTextDocument::FindFlags flags);
    void replaceAllRequested(const QString &findText, const QString &replaceText, QTextDocument::FindFlags flags);

private slots:
    void onFindClicked();
    void onReplaceClicked();
    void onReplaceAllClicked();

private:
    bool m_isReplaceMode;
    QLineEdit *m_editFind;
    QLineEdit *m_editReplace;
    QCheckBox *m_chkWholeWord;
    QCheckBox *m_chkMatchCase;
    QRadioButton *m_rbUp;
    QRadioButton *m_rbDown;
    QPushButton *m_btnFindNext;
    QPushButton *m_btnReplace;
    QPushButton *m_btnReplaceAll;
    QWidget *m_replaceWidget;
};

} // namespace OpenWordPad
