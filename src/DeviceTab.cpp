#include "DeviceTab.hpp"


PaDeviceInfo_ext::PaDeviceInfo_ext(const PaDeviceInfo &other, int index)
{
    this->defaultHighInputLatency = other.defaultHighInputLatency;
    this->defaultHighOutputLatency = other.defaultHighOutputLatency;
    this->defaultLowInputLatency = other.defaultLowInputLatency;
    this->defaultLowOutputLatency = other.defaultLowOutputLatency;
    this->defaultSampleRate = other.defaultSampleRate;
    this->hostApi = other.hostApi;
    this->index = index;
    this->maxInputChannels = other.maxInputChannels;
    this->maxOutputChannels = other.maxOutputChannels;
    this->name = other.name;
    this->structVersion = other.structVersion;
}


DeviceTab::DeviceTab(QRect *screenGeometry, QString checkboxLabel, bool checkboxState, DeviceType type,
                     QComboBox *combobox_devices, QWidget *parent) : QWidget(parent)
{
    // Save device type
    this->type = type;

    // Widget layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    // Layout contents:
    // Combobox
    this->combobox_devices = combobox_devices;
    layout->addWidget(this->combobox_devices);
    // Checkbox
    checkbox = new QCheckBox(checkboxLabel);
    checkbox->setChecked(checkboxState);
    layout->addWidget(checkbox);

    // Load devices
    refreshDevices();

    // Set layout
    setLayout(layout);
}


DeviceTab::~DeviceTab()
{
    // Clear devices list
    devices.clear();
}


void DeviceTab::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


PaDeviceInfo_ext DeviceTab::getDevice() const
{
    // Search in devices list via index of combobox list
    int list_index = 0;
    for (PaDeviceInfo_ext device : devices)
    {
        if (list_index == combobox_devices->currentIndex())
        {
            return device;
        }
        list_index++;
    }

    // If no device was found, app probably ran into some memory issues
    throw runtime_error("Device list and combobox list mismatch");
}


void DeviceTab::refreshDevices()
{
    // Block any signals from combobox since it is now being edited
    combobox_devices->blockSignals(true);

    // Clear combobox list if needed
    if (combobox_devices->count() != 0)
    {
        combobox_devices->clear();
        devices.clear();
    }

    // Iterate over devices
    const PaDeviceInfo* device_info;
    for (int i=0; i < Pa_GetDeviceCount(); i++)
    {
        // Get the device info
        device_info = Pa_GetDeviceInfo(i);
        // Select either input or output device (with hostApi==0 being the most OK host API type)
        if (((type == INPUT)  && (device_info->maxInputChannels  > 0) && (!device_info->hostApi)) ||
            ((type == OUTPUT) && (device_info->maxOutputChannels > 0) && (!device_info->hostApi)))
        {
            combobox_devices->addItem(device_info->name);
            devices.insert(devices.cend(), PaDeviceInfo_ext(*device_info, i));
        }
    }

    // Release combobox
    combobox_devices->blockSignals(false);
}