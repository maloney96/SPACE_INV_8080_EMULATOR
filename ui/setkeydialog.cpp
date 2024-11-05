#include "setkeydialog.h"
#include "ui_SetKeyDialog.h"  // Include the generated UI header file
#include <QMessageBox>

// Constructor
SetKeyDialog::SetKeyDialog(const QString &action, int currentKey, const QMap<QString, int> &keyMappings, QWidget *parent)
    : QDialog(parent), ui(new Ui::SetKeyDialog), actionName(action), currentKey(currentKey), keyMappings(keyMappings)
{
    ui->setupUi(this);

    // Set the dialog title
    setWindowTitle("Set Key: " + actionName);

    // Display the current key assignment
    ui->currentKeyLabel->setText("Current key: " + QKeySequence(currentKey).toString());

    // Connect the Cancel button
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

// Destructor to clean up the UI
SetKeyDialog::~SetKeyDialog() {
    delete ui;
}

// Override keyPressEvent to capture key press
void SetKeyDialog::keyPressEvent(QKeyEvent *event)
{
    int newKey = event->key();  // Get the pressed key

    // Prevent default behavior for navigation/system keys
    if (newKey == Qt::Key_Up || newKey == Qt::Key_Down ||
        newKey == Qt::Key_Left || newKey == Qt::Key_Right)
    {
        event->accept();  // Prevent navigation
        // Continue to reassign the arrow key (no early return here)
    }

    // Check if the new key is the same as the current key
    if (newKey == currentKey) {
        // No change, just accept the dialog without any conflict check
        reassignedKey = newKey;
        accept();
        return;
    }

    // Check if the new key is already assigned to another action
    for (auto it = keyMappings.constBegin(); it != keyMappings.constEnd(); ++it) {
        if (it.value() == newKey && it.key() != actionName) {
            // Display error if the key is already assigned
            QMessageBox::warning(this, "Key Conflict", "The key is already assigned to the action: " + it.key());
            return;  // Prevent reassignment
        }
    }

    // If no conflict, assign the new key and accept the dialog
    reassignedKey = newKey;
    accept();
}

int SetKeyDialog::getReassignedKey() const { return reassignedKey; }
