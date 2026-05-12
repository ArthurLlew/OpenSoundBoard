#include <DeviceTab.hpp>


// Exceptions
#include <stdexcept>


DeviceTab::DeviceTab(DevicesList::DeviceType device_type, QComboBox *combobox_devices, QWidget *parent) : QWidget(parent)
{
    // Init devices list
    devices = new DevicesList(device_type);

    // Widget layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setAlignment(Qt::AlignTop);
    // Combobox
    this->combobox_devices = combobox_devices;
    layout->addWidget(this->combobox_devices);

    // Load devices
    refreshDevices();

    // Set layout
    setLayout(layout);
}


DeviceTab::~DeviceTab()
{
    // Clear devices list
    delete devices;
}


void DeviceTab::paintEvent(QPaintEvent *)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


SDL_AudioDeviceID DeviceTab::getDevice() const
{
    // Throw exception if there are no devices
    if (devices->count() < 1)
        throw std::runtime_error("Audio devices: nothing is avaliable");

    // Return audio device ID corresponding to selected device in combobox
    return devices->get(combobox_devices->currentIndex());
}


void DeviceTab::refreshDevices()
{
    // Block any signals from combobox (editing combobox will not cause any updates)
    combobox_devices->blockSignals(true);

    // Clear combobox
    if (combobox_devices->count() != 0)
    {
        combobox_devices->clear();
    }
    // Refresh devices list
    devices->refresh();
    // Fill combobox
    for (int i=0; i < devices->count(); i++)
    {
        combobox_devices->addItem(SDL_GetAudioDeviceName(devices->get(i)));
    }

    // Release combobox
    combobox_devices->blockSignals(false);
}