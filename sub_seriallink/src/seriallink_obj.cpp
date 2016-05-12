#include "seriallink_obj.h"
#include <QtSerialPort/QSerialPortInfo>
#include <QDebug>
#include "serialportwriter.h"

int bytesToInt(QByteArray bytes)
{
    int addr = bytes[0] & 0x000000FF;
    addr |= ((bytes[1] << 8) & 0x0000FF00);
    addr |= ((bytes[2] << 16) & 0x00FF0000);
    addr |= ((bytes[3] << 24) & 0xFF000000);
    return addr;
}

QByteArray intToByte(int number)
{
    QByteArray abyte0;
    abyte0.resize(4);
    abyte0[0] = (uchar)  (0x000000ff & number);
    abyte0[1] = (uchar) ((0x0000ff00 & number) >> 8);
    abyte0[2] = (uchar) ((0x00ff0000 & number) >> 16);
    abyte0[3] = (uchar) ((0xff000000 & number) >> 24);
    return abyte0;
}

int bytesToShort(QByteArray bytes)
{
    int addr = bytes[0] & 0x00FF;
    addr |= ((bytes[1] << 8) & 0xFF00);
    return addr;
}

QByteArray shortToByte(int number)
{
    QByteArray abyte0;
    abyte0.resize(2);
    abyte0[0] = (uchar)  (0x00ff & number);
    abyte0[1] = (uchar) ((0xff00 & number) >> 8);
    return abyte0;
}

const short wCRC_Talbe[] =
{
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401,
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400
};

short CRC16(char* pyData, int wDataLen)
{
    short wCRC = 0xFFFF;
    short i;
    char yChar;

    for (i = 0; i < wDataLen; i++)
    {
        yChar = *pyData++;
        wCRC = wCRC_Talbe[(yChar ^ wCRC) & 15] ^ (wCRC >> 4);
        wCRC = wCRC_Talbe[((yChar >> 4) ^ wCRC) & 15] ^ (wCRC >> 4);
    }

    return wCRC;
}

//char cLength != 0;
QByteArray Txn_Msg_Request_Buf_Common(char cType, char cLength, QByteArray byte)
{
    QByteArray request;

    request[0]=char(0xEB);
    request[1]=char(0x90);
    request[2]=char(0xFF);
    request[3]=char(0xFF);
    request[4]=char(0xFF);
    request[5]=char(0xFF);
    request[6]=cType;
    request[7]=cLength;

    request += byte;

    return request;
}

QByteArray Txn_Msg_Request_Buf_Common_Total(char cType, char cLength, QByteArray byte)
{
    QByteArray byteTmp = Txn_Msg_Request_Buf_Common(cType,cLength, byte);
    int iCRC = CRC16((char*)byteTmp.data(), byteTmp.size());
    return byteTmp + shortToByte(iCRC);
}



seriallink_obj::seriallink_obj(QObject *parent) :
    QObject(parent)
  , serialPortWriter(0)
{
    Q_FOREACH (QSerialPortInfo info, QSerialPortInfo::availablePorts())
    {
        qDebug() << "Name : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();
        qDebug() << "Serial Number: " << info.serialNumber();
        qDebug() << "System Location: " << info.systemLocation();

        QString strPortName = info.portName();
        if(!strPortName.isEmpty()){

            serialPort.setPortName(strPortName);
            serialPort.setBaudRate(QSerialPort::Baud115200);
            serialPort.setDataBits(QSerialPort::Data8);
            serialPort.setFlowControl(QSerialPort::NoFlowControl);
            serialPort.setParity(QSerialPort::NoParity);
            serialPort.setStopBits(QSerialPort::OneStop);
            serialPort.open(QIODevice::WriteOnly);

            serialPortWriter = new SerialPortWriter(&serialPort);
        }
    }
}

void seriallink_obj::ImageShow(const use_opencv::move& msg)
{
    int iX = msg.iX;
    int iY = msg.iY;

    QByteArray writeData = Txn_Msg_Request_Buf_Common_Total(0x90, 4, shortToByte(iX)+ shortToByte(iY));

    if(serialPortWriter){
        serialPortWriter->write(writeData);
    }
}



