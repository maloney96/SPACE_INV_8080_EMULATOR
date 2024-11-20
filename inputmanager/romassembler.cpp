#include "romassembler.h"

#include <QFile>
#include <QDir>
#include <QDebug>
#include <QCryptographicHash>

RomAssembler::RomAssembler() {
    // Define the output file path for the combined ROM
    QString outputFilePath = QDir::currentPath() + "/invaders.rom";
    QFile outputFile(outputFilePath);

    // Check if the output ROM file already exists
    if (outputFile.exists()) {
        qDebug() << "ROM file already exists at:" << outputFilePath;

        // Compute hash of the existing file
        if (!outputFile.open(QIODevice::ReadOnly)) {
            qDebug() << "Failed to open existing output file for hash calculation.";
            return;
        }
        QByteArray existingFileHash = QCryptographicHash::hash(outputFile.readAll(), QCryptographicHash::Sha256);
        outputFile.close();

        // Create the ROM file in memory to compute its hash
        QByteArray newRomData;

        for (const QString &romFile : romFiles) {
            QFile file(romFile);

            if (!file.exists()) {
                qDebug() << "Resource file does not exist:" << romFile;
                return;
            }

            if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << "Failed to open ROM file:" << romFile;
                return;
            }

            QByteArray data = file.readAll();
            newRomData.append(data);
            file.close();
            qDebug() << "Appended ROM file:" << romFile;
        }

        // Compute hash of the new ROM data
        QByteArray newFileHash = QCryptographicHash::hash(newRomData, QCryptographicHash::Sha256);

        // Compare the hashes
        if (existingFileHash == newFileHash) {
            qDebug() << "The existing ROM file matches the newly generated ROM file.";
            return; // No need to overwrite
        } else {
            qDebug() << "The existing ROM file differs from the newly generated ROM file. Overwriting.";
            if (!outputFile.open(QIODevice::WriteOnly)) {
                qDebug() << "Failed to open existing ROM file for overwriting.";
                return;
            }
            outputFile.write(newRomData);
            outputFile.close();
            qDebug() << "Existing ROM file has been overwritten.";
        }
    } else {
        // If no file exists, create the ROM file directly
        if (!outputFile.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open output file:" << outputFilePath;
            return;
        }

        for (const QString &romFile : romFiles) {
            QFile file(romFile);

            if (!file.exists()) {
                qDebug() << "Resource file does not exist:" << romFile;
                outputFile.close();
                return;
            }

            if (!file.open(QIODevice::ReadOnly)) {
                qDebug() << "Failed to open ROM file:" << romFile;
                outputFile.close();
                return;
            }

            QByteArray data = file.readAll();
            outputFile.write(data);
            file.close();
            qDebug() << "Appended ROM file:" << romFile;
        }

        outputFile.close();
        qDebug() << "ROM files concatenated successfully into:" << outputFilePath;
    }
}

RomAssembler::~RomAssembler() {
    qDebug() << "RomAssembler instance destroyed.";
}
