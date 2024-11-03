#ifndef SETKEYDIALOG_H
#define SETKEYDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QMap>

namespace Ui {
class SetKeyDialog;  // Forward declaration of the UI class
}

class SetKeyDialog : public QDialog {
    Q_OBJECT

public:
    explicit SetKeyDialog(const QString &action, int currentKey, const QMap<QString, int> &keyMappings, QWidget *parent = nullptr);
    ~SetKeyDialog();

    int getReassignedKey() const;

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::SetKeyDialog *ui;  // Pointer to the UI
    QString actionName;
    int currentKey;
    int reassignedKey = 0;
    QMap<QString, int> keyMappings;
};

#endif // SETKEYDIALOG_H
