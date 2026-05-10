#pragma once


// Qt core
#include <QtCore/Qt>
#include <QtCore/QString>
// Qt GUI
#include <QtGui/QPainter>
// Qt widgets
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QComboBox>
// Qt Multimedia
#include <QtMultimedia/QMediaDevices>
#include <QtMultimedia/QAudioDevice>


/** Device tab that manages info about available input or output devices.*/
class DeviceTab: public QWidget
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Stores info about devices, available for this tab.*/
    QList<QAudioDevice> devices;

public:

    /** Combobox with devices.*/
    QComboBox *combobox_devices = nullptr;
    /** Describes audio device type.*/
    enum DeviceType
    {
        INPUT,
        OUTPUT
    };
    /** Device type.*/
    DeviceType type;

    /** Constructor.
     * 
     *  @param type Device type (input/output).
     *  @param combobox_devices Combobox with devices.
    */
    explicit DeviceTab(DeviceType type, QComboBox *combobox_devices, QWidget *parent = nullptr);
    /** Destructor.*/
    ~DeviceTab();

    /** Redifinition allows usage of QSS.*/
    void paintEvent(QPaintEvent *) override;

    /** Refreshes devices list.*/
    void refreshDevices();
    
    /** Returns info about selected device.
     * 
     *  @return Audio device info.
    */
    QAudioDevice getDevice() const;
};