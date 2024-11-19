// Created by Ian McCubbin, 10/28/2024
// Dialog box used to set user controls for gameplay.

#ifndef SETKEYDIALOG_H
#define SETKEYDIALOG_H

#include <QDialog>
#include <QKeyEvent>
#include <QMap>

namespace Ui {
class SetKeyDialog;  // Forward declaration of the UI class
}

/**
 * @brief The SetKeyDialog class provides a dialog for reassigning keys to actions.
 */
class SetKeyDialog : public QDialog {
    Q_OBJECT

public:
    /**
     * @brief Constructor for SetKeyDialog.
     * @param action The name of the action to reassign.
     * @param currentKey The current key assigned to the action.
     * @param keyMappings A map of existing key assignments.
     * @param parent The parent widget.
     */
    explicit SetKeyDialog(const QString &action, int currentKey, const QMap<QString, int> &keyMappings, QWidget *parent = nullptr);

    /**
     * @brief Destructor for SetKeyDialog.
     */
    ~SetKeyDialog();

    /**
     * @brief Retrieves the newly assigned key.
     * @return The reassigned key code.
     */
    int getReassignedKey() const;

protected:
    /**
     * @brief Captures and processes key press events for reassigning keys.
     * @param event The key event.
     */
    void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::SetKeyDialog *ui;  // Pointer to the UI
    QString actionName;    // Name of the action to reassign
    int currentKey;        // Current key assigned to the action
    int reassignedKey = 0; // Newly assigned key
    QMap<QString, int> keyMappings; // Existing key assignments
};

#endif // SETKEYDIALOG_H
