#ifndef SETTINGS_H
#define SETTINGS_H

// Created by Ian McCubbin, 10/25/2024
// Settings page used to set game controls.

#include <QDialog>
#include <QMap>
#include <QKeyEvent>
#include <QJsonObject>
#include <QAbstractButton>

namespace Ui {
class Settings;
}

/**
 * @class Settings
 * @brief The Settings class provides a dialog for configuring key mappings.
 *
 * This class allows users to view, modify, and save key mappings for various actions.
 * It supports loading existing mappings from a JSON file and saving changes back to the file.
 */
class Settings : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a Settings dialog.
     * @param parent The parent widget. Defaults to nullptr.
     */
    explicit Settings(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the Settings dialog.
     */
    ~Settings();

    /**
     * @brief Initializes the key mappings by loading them from a JSON file.
     * @return A QJsonObject containing the key mappings.
     *
     * This function attempts to load key mappings from a predefined JSON file.
     * If the file exists and is valid, the mappings are loaded into a QJsonObject.
     * If the file does not exist or is invalid, default mappings are used.
     */
    QJsonObject initializeKeyMap();

    /**
     * @brief Saves the current key mappings to a JSON file.
     *
     * This function writes the current key mappings to a predefined JSON file,
     * ensuring that any changes made by the user are persisted.
     */
    void saveKeyMappings();

private slots:
    /**
     * @brief Slot triggered when a key mapping is to be set for a specific action.
     * @param action The name of the action for which the key is to be set.
     *
     * This slot is connected to UI elements that allow the user to set or change
     * the key mapping for a specific action. It handles the process of capturing
     * the new key and updating the mapping.
     */
    void onSetKeyClicked(const QString &action);

    /**
     * @brief Slot triggered when a button in the dialog's button box is clicked.
     * @param button The button that was clicked.
     *
     * This slot handles the logic for different buttons in the dialog's button box,
     * such as OK, Cancel, or Apply, and performs the appropriate actions based on
     * which button was clicked.
     */
    void OnDialogButtonBoxClicked(QAbstractButton *button);

    /**
     * @brief Slot triggered when the Cancel button is clicked.
     *
     * This slot reverts any unsaved changes to the key mappings and closes the dialog.
     */
    void onCancelButtonClicked();

    /**
     * @brief Slot triggered when the OK button is clicked.
     *
     * This slot saves any changes to the key mappings and closes the dialog.
     */
    void onOkButtonClicked();

private:
    Ui::Settings *ui; ///< Pointer to the UI elements of the dialog.
    QJsonObject originalKeymap; ///< Stores the original key mappings for comparison.
    QJsonObject currentKeymap; ///< Stores the current key mappings being edited.

    /**
     * @brief Populates the key mapping fields in the UI with the provided mappings.
     * @param map A QJsonObject containing the key mappings to display.
     *
     * This function updates the UI elements to reflect the key mappings provided
     * in the map, allowing the user to view and edit them.
     */
    void populateKeyFields(QJsonObject map);

    /**
     * @brief Sets the key mappings to their default values.
     *
     * This function resets all key mappings to their default values, overwriting
     * any current mappings.
     */
    void setDefaultKeyMap();

    /**
     * @brief Retrieves the key code associated with a specific action.
     * @param action The name of the action.
     * @return The key code associated with the action, or -1 if not found.
     *
     * This function looks up the key code assigned to the specified action in the
     * current key mappings.
     */
    int getKeyForAction(const QString &action);

    /**
     * @brief Updates the key mapping for a specific action.
     * @param action The name of the action.
     * @param newKey The new key code to assign to the action.
     *
     * This function updates the key mapping for the specified action with the new
     * key code provided by the user.
     */
    void updateKeyForAction(const QString &action, int newKey);
};

#endif // SETTINGS_H
