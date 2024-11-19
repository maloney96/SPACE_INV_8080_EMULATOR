#include <QDebug>
#include <QString>
#include <cstdarg>

extern "C" void qdebug_log(const char *format, ...) {
    char buffer[1024];
    va_list args;

    // Format the string
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    // Convert the formatted C string to a QString for proper handling (accepts escape chars (i.e. /n, /t
    QString message = QString::fromUtf8(buffer);

    // Use QDebug to print the formatted string
    qDebug().noquote() << message;
}
