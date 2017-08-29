#ifndef SUPERCONTROL_H
#define SUPERCONTROL_H

#include <QObject>

class SuperControl : public QObject
{
    Q_OBJECT
public:
    explicit SuperControl(QObject *parent = 0);

signals:

public slots:
};

#endif // SUPERCONTROL_H