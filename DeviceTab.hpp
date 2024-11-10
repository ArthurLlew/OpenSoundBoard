#pragma once

// Qt5
#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSizePolicy>
// PortAudio
#include <portaudio.h>

using namespace std;


enum DeviceType
{
    INPUT,
    OUTPUT
};


typedef struct PaDeviceInfo_ext : PaDeviceInfo
{
    // PortAudio device index
    int index;

    // Copy constructor
    PaDeviceInfo_ext(const PaDeviceInfo &other, int index);
} PaDeviceInfo_ext;


class DeviceTab: public QWidget
{
    // Stores info about devices, available for this tab
    list<PaDeviceInfo_ext> devices;

    public:

    // Combobox with devices
    QComboBox *combobox_devices;
    // Checkbox that tells if the device should be used by the audio player
    QCheckBox *checkbox;
    // Tells if device is an input/output (true/false)
    DeviceType device_type;

    // Constructor
    DeviceTab(QRect *screen_geometry, QString checkbox_label, bool checkbox_state, DeviceType device_type, QComboBox *combobox_devices,
              QWidget *parent = nullptr);
    // Destructor
    ~DeviceTab();

    // Refreshes devices list
    void refresh_devices();
    // Returns selected device info
    PaDeviceInfo_ext get_selected_device() const;
};