/*  Created by Ian McCubbin, 10/25/2024
 *  Settings page used to set game controls
 *
 *  MODIFIED by Ian McCubbin, 11/9/2024
 *  BUGFIX- on set default press, currentkey was not updating to reflect deafault keys.
 */

#include "settings.h"
#include "ui_settings.h"
#include "setkeydialog.h"
#include "../inputmanager/keymap.h"

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QMessageBox>


#ifdef Q_OS_WIN
#include <windows.h>
#endif

/**
 * @brief Constructor for the Settings dialog.
 *
 * The Settings class provides a dialog for configuring user key mappings.
 * Initializes the dialog, connects signals to slots for key reassignments, 
 * and loads existing key mappings or sets defaults if unavailable.
 * 
 * @param parent Pointer to the parent widget, default is nullptr.
 */
Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    currentKeymap = originalKeymap = initializeKeyMap();

    // Connect the "Set" buttons to the slot for changing key mappings
    connect(ui->buttonMoveLeftSet, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("left")); });
    connect(ui->buttonMoveRightSet, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("right")); });
    connect(ui->buttonFireSet, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("fire")); });
    connect(ui->buttonP1Set, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("p1_button")); });
    connect(ui->buttonP2Set, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("p2_button")); });
    connect(ui->buttonInsertCoinSet, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("insert_coin")); });
    connect(ui->buttonExitGameSet, &QPushButton::clicked, this, [this]() { onSetKeyClicked(QString("exit_game")); });

    connect(ui->dialogButtonBox, &QDialogButtonBox::clicked, this, [&](QAbstractButton *button)
            {
                if (button == ui->dialogButtonBox->button(QDialogButtonBox::Ok)) { onOkButtonClicked(); }
                else if (button == ui->dialogButtonBox->button(QDialogButtonBox::Cancel)) { onCancelButtonClicked(); }
                else if (button == ui->dialogButtonBox->button(QDialogButtonBox::RestoreDefaults)) {
                    setDefaultKeyMap();
                    QMessageBox::information(this, "Default Keys Restored", "The default key mappings have been restored successfully.");
                }
            });
}

/**
 * @brief Destructor for the Settings dialog.
 */
Settings::~Settings() { delete ui; }

/**
 * @brief Populates the key fields with current key mappings.
 * 
 * Updates the UI fields to display the key assignments from the provided JSON map.
 * 
 * @param map JSON object containing the key mappings.
 */
void Settings::populateKeyFields(QJsonObject map)
{

    ui->lineEditFire->setText(QKeySequence(map["fire"].toInt()).toString());
    ui->lineEditMoveLeft->setText(QKeySequence(map["left"].toInt()).toString());
    ui->lineEditMoveRight->setText(QKeySequence(map["right"].toInt()).toString());
    ui->lineEditP1->setText(QKeySequence(map["p1_button"].toInt()).toString());
    ui->lineEditP2->setText(QKeySequence(map["p2_button"].toInt()).toString());
    ui->lineEditInsertCoin->setText(QKeySequence(map["insert_coin"].toInt()).toString());
    ui->lineEditExitGame->setText(QKeySequence(map["exit_game"].toInt()).toString());
}

/**
 * @brief Slot to initiate key reassignment.
 * On OK- save new keymap and exit
 * On Cancel- restore old keymap and exit
 * On RestoreDefualts- restore default keys, DO NOT EXIT
 * @param action The action name whose key assignment is being modified.
 */
void Settings::OnDialogButtonBoxClicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton sb = ui->dialogButtonBox->standardButton(button);

    if (sb == QDialogButtonBox::Cancel) {
        onCancelButtonClicked();
    }
    if (sb == QDialogButtonBox::RestoreDefaults) {
        setDefaultKeyMap();
    }
    if (sb == QDialogButtonBox::Ok) {
        onOkButtonClicked();
    }
}

/**
 * @brief Sets the default key mappings and updates the UI.
 * 
 * Resets the key mappings to their default values, saves them to the keymap file,
 * and updates the UI fields to reflect these defaults.
 */
void Settings::setDefaultKeyMap() {
    // Define the keymap file path
    QString keymapPath = QDir::currentPath() + "/.keymap.json";
    QFile keymapFile(keymapPath);

    // Create a JSON object for the default keymap
    QJsonObject defaultKeymapJson;
    defaultKeymapJson["fire"] = DEFAULT_FIRE;
    defaultKeymapJson["left"] = DEFAULT_LEFT;
    defaultKeymapJson["right"] = DEFAULT_RIGHT;
    defaultKeymapJson["p1_button"] = DEFAULT_P1_BUTTON;
    defaultKeymapJson["p2_button"] = DEFAULT_P2_BUTTON;
    defaultKeymapJson["insert_coin"] = DEFAULT_INSERT_COIN;
    defaultKeymapJson["exit_game"] = DEFAULT_EXIT;

    //Extra Life and Score defaults
    defaultKeymapJson["extra_lives"] = DEFAULT_EXTRA_LIVES;
    defaultKeymapJson["extra_life_at"] = DEFAULT_EXTRA_LIFE_AT;

    // Try to open the keymap file for writing
    if (keymapFile.open(QIODevice::WriteOnly)) {
        // Convert the JSON object to a JSON document and write it to the file
        QJsonDocument jsonDoc(defaultKeymapJson);
        keymapFile.write(jsonDoc.toJson());

        // Close the file after writing
        keymapFile.close();
    } else {
        // Handle error if the file couldn't be opened
        qWarning() << "Failed to open keymap file for writing defaults: " << keymapPath;
    }

    populateKeyFields(defaultKeymapJson);
    currentKeymap = defaultKeymapJson;
}

/**
 * @brief Initializes the key mappings.
 * 
 * Loads key mappings from a JSON file if it exists; otherwise, sets default key mappings.
 * @return A QJsonObject containing the key mappings.
 */
QJsonObject Settings::initializeKeyMap()
{
    QString keymapPath = QDir::currentPath() + "/.keymap.json";  // Hidden file in the home directory
    QFile keymapFile(keymapPath);

    QJsonObject keymapJson;

    // Check if the keymap.json file exists
    if (keymapFile.exists()) {
        // If file exists, read the key mappings
        if (keymapFile.open(QIODevice::ReadOnly)) {
            QByteArray data = keymapFile.readAll();
            QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
            keymapJson = jsonDoc.object();
            keymapFile.close();
        }
    } else {
        // If the file doesn't exist, set and save default keymap
        setDefaultKeyMap();

        // After calling setDefaultKeyMap, read the default values back into keymapJson
        if (keymapFile.open(QIODevice::ReadOnly)) {
            QByteArray data = keymapFile.readAll();
            QJsonDocument jsonDoc(QJsonDocument::fromJson(data));
            keymapJson = jsonDoc.object();
            keymapFile.close();
        }
    }

    // Populate the lineEdit fields with the key mappings
    populateKeyFields(keymapJson);

    return keymapJson;
}

/**
 * @brief Slot to handle the Cancel button click.
 * 
 * Restores the original key mappings and closes the dialog without saving changes.
 */
void Settings::onCancelButtonClicked()
{
    QString keymapPath = QDir::currentPath() + "/.keymap.json";
    QFile keymapFile(keymapPath);

    // Check if the file can be opened for writing
    if (keymapFile.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(originalKeymap);
        keymapFile.write(jsonDoc.toJson());
        keymapFile.close();
    } else {
        // Handle case where the file doesn't exist or can't be opened
        qWarning() << "Failed to open the keymap file for writing: " << keymapPath;

        // Attempt to create a new file and write the original keymap
        if (keymapFile.open(QIODevice::WriteOnly | QIODevice::NewOnly)) {
            QJsonDocument jsonDoc(originalKeymap);
            keymapFile.write(jsonDoc.toJson());
            keymapFile.close();
        } else {
            qCritical() << "Failed to create a new keymap file: " << keymapPath;
        }
    }

    // Close the dialog
    reject();
}

/**
 * @brief Slot to handle the OK button click.
 * 
 * Saves the current key mappings and closes the dialog.
 */
void Settings::onOkButtonClicked()
{
    // Define the keymap file path
    QString keymapPath = QDir::currentPath() + "/.keymap.json";
    QFile keymapFile(keymapPath);

    // Create a JSON object to store the updated key mappings
    QJsonObject updatedKeymapJson;

    // Read the values from the lineEdit fields and convert them to key sequences
    QKeySequence fireKeySeq(ui->lineEditFire->text());
    QKeySequence leftKeySeq(ui->lineEditMoveLeft->text());
    QKeySequence rightKeySeq(ui->lineEditMoveRight->text());
    QKeySequence p1KeySeq(ui->lineEditP1->text());
    QKeySequence p2KeySeq(ui->lineEditP2->text());
    QKeySequence insertCoinKeySeq(ui->lineEditInsertCoin->text());
    QKeySequence exitGameKeySeq(ui->lineEditExitGame->text());

    // Store the first key from each QKeySequence (since QKeySequence can store multiple keys)
    updatedKeymapJson["fire"] = fireKeySeq[0].toCombined();  // Store the first key
    updatedKeymapJson["left"] = leftKeySeq[0].toCombined();
    updatedKeymapJson["right"] = rightKeySeq[0].toCombined();
    updatedKeymapJson["p1_button"] = p1KeySeq[0].toCombined();
    updatedKeymapJson["p2_button"] = p2KeySeq[0].toCombined();
    updatedKeymapJson["insert_coin"] = insertCoinKeySeq[0].toCombined();
    updatedKeymapJson["exit_game"] = exitGameKeySeq[0].toCombined();

    // Store values for extra lives and extra life score
    qDebug("UI lives value %d",ui->spinBoxLives->value());
    updatedKeymapJson["lives"] = ui->spinBoxLives->value();
    updatedKeymapJson["extra_life_at"] = ui->spinBoxExtraLifeScore->value();

    // Try to open the keymap file for writing
    if (keymapFile.open(QIODevice::WriteOnly)) {
        // Convert the updated JSON object to a JSON document and write it to the file
        QJsonDocument jsonDoc(updatedKeymapJson);
        keymapFile.write(jsonDoc.toJson());

        // Close the file after writing
        keymapFile.close();
    } else {
        // Handle error if the file couldn't be opened
        qWarning() << "Failed to open keymap file for writing: " << keymapPath;
    }

    // Close the dialog, since the OK button was clicked
    accept();
}

/**
 * @brief Slot to handle the "Set Key" button click for a specific action.
 * @param action The action name whose key assignment is being modified.
 */
void Settings::onSetKeyClicked(const QString &action)
{
    int currentKey = getKeyForAction(action);  // Get the current key for "Move Left"

    // Convert currentKeymap to QMap<QString, int>
    QMap<QString, int> keyMap;
    for (const QString& key : currentKeymap.keys()) {
        keyMap[key] = currentKeymap[key].toInt();
    }

    // Open SetKeyDialog to reassign the key
    SetKeyDialog dialog(action, currentKey, keyMap, this);
    if (dialog.exec() == QDialog::Accepted) {
        int newKey = dialog.getReassignedKey();
        updateKeyForAction(action, newKey);
    }
}


/**
 * @brief Retrieves the key code associated with a specific action.
 * @param action The name of the action.
 * @return The key code assigned to the action.
 */
int Settings::getKeyForAction(const QString &action)
{
    // Check if the action exists in the currentKeymap
    if (currentKeymap.contains(action)) {
        return currentKeymap[action].toInt();  // Return the current key as an integer
    } else {
        qWarning() << "Action" << action << "not found in keymap.";
        return -1;  // Return -1 to indicate the key was not found
    }
}

/**
 * @brief Updates the key mapping for a specific action.
 * @param action The name of the action to update.
 * @param newKey The new key code to assign to the action.
 */
void Settings::updateKeyForAction(const QString &action, int newKey)
{
    currentKeymap[action] = newKey;  // Update the keymap with the new key
    populateKeyFields(currentKeymap);  // Optionally update the UI to reflect the new key
}
