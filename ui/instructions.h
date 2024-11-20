#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <QDialog>

namespace Ui {
class Instructions;
}

/**
 * @class Instructions
 * @brief A dialog that displays the game instructions.
 *
 * The Instructions class provides a simple dialog window
 * to display instructions or other information for the user.
 * It inherits from QDialog and manages its own UI components.
 */
class Instructions : public QDialog
{
    Q_OBJECT

public:
    /**
     * @brief Constructs the Instructions dialog.
     * @param parent Pointer to the parent widget. Defaults to nullptr.
     *
     * Initializes the dialog and its UI components, setting up any
     * required styles or configurations.
     */
    explicit Instructions(QWidget *parent = nullptr);

    /**
     * @brief Destructor for the Instructions dialog.
     *
     * Cleans up the allocated resources for the UI components.
     */
    ~Instructions();

private:
    Ui::Instructions *ui;  ///< Pointer to the UI components of the Instructions dialog.
};

#endif // INSTRUCTIONS_H
