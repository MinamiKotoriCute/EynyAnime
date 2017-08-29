#ifndef MYQSETTINGS_H
#define MYQSETTINGS_H

#include <QSettings>

class MyQSettings : public QSettings
{
    Q_OBJECT
public:
    MyQSettings();
    void* operator new(std::size_t) = delete;

signals:

public slots:
};

#endif // MYQSETTINGS_H
