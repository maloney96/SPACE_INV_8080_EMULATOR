#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QMap>
#include <QKeyEvent>
#include <QJsonObject>
#include <QAbstractButton>

namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    ~Settings();

    QJsonObject initializeKeyMap();  // Load key mappings from the JSON file, if exists
    void saveKeyMappings();  // Save key mappings to the JSON file

private slots:
    void onSetKeyClicked(const QString &action);
    void OnDialogButtonBoxClicked(QAbstractButton *button);
    void onCancelButtonClicked();
    void onOkButtonClicked();

private:
    Ui::Settings *ui;
    QJsonObject originalKeymap;
    QJsonObject currentKeymap;
    void populateKeyFields(QJsonObject map);
    void setDefaultKeyMap();
    int getKeyForAction(const QString &action);
    void updateKeyForAction(const QString &action, int newKey);

};

#endif // SETTINGS_H
