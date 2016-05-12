#ifndef SERIALLINK_OBJ_H
#define SERIALLINK_OBJ_H

#include <QObject>
#include <use_opencv/move.h>
#include <QtSerialPort/QSerialPortInfo>
#include "serialportwriter.h"
class seriallink_obj : public QObject
{
    Q_OBJECT
public:
    explicit seriallink_obj(QObject *parent = 0);

public:
    QSerialPort serialPort;
    SerialPortWriter* serialPortWriter;

public Q_SLOTS:
    void ImageShow(const use_opencv::move& msg);

};

#endif // SERIALLINK_OBJ_H
