#include "romassembler.h"

#include <QFile>
#include <QDir>
#include <QDebug>

RomAssembler::RomAssembler() {
    // Define the output file path for the combined ROM
    QString outputFilePath = QDir::currentPath() + "/invaders.rom";
    QFile outputFile(outputFilePath);

    // Check if the output ROM file already exists
    if (outputFile.exists()) {
        qDebug() << "ROM file already exists at:" << outputFilePath;
        return;
    }

    // Attempt to open the output file for writing
    if (!outputFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to open output file:" << outputFilePath;
        return;
    }

    // Iterate over each ROM file and append its contents to the output file
    for (const QString &romFile : romFiles) {
        QFile file(romFile);

        // Check if the input ROM file exists
        if (!file.exists()) {
            qDebug() << "Resource file does not exist:" << romFile;
            outputFile.close();
            return;
        }

        // Attempt to open the ROM file for reading
        if (!file.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open ROM file:" << romFile;
            outputFile.close();
            return;
        }

        // Append the file contents to the output ROM file
        QByteArray data = file.readAll();
        outputFile.write(data);
        file.close();
        qDebug() << "Appended ROM file:" << romFile;
    }

    // Close the output file after writing all contents
    outputFile.close();
    qDebug() << "ROM files concatenated successfully into:" << outputFilePath;
}

RomAssembler::~RomAssembler() { qDebug() << "RomAssembler instance destroyed."; }
