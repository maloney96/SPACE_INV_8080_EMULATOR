#include "instructions.h"
#include "ui_instructions.h"

/**
 * @brief Constructs the Instructions dialog.
 * @param parent Pointer to the parent widget. Defaults to nullptr.
 *
 * Initializes the UI components and sets the background style.
 */
Instructions::Instructions(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Instructions)
{
    ui->setupUi(this);
    this->setAutoFillBackground(false);
    // TODO: Investigate why the background remains as SpaceSky
    this->setStyleSheet("background-image: none;");
}

/**
 * @brief Destructor for the Instructions dialog.
 *
 * Cleans up the UI components.
 */
Instructions::~Instructions()
{
    delete ui;
}
