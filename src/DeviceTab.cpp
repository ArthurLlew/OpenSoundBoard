#include <DeviceTab.hpp>


DeviceTab::DeviceTab(QString checkboxLabel, bool checkboxState, DeviceType type, QComboBox *combobox_devices,
                     QWidget *parent) : QWidget(parent)
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


QAudioDevice DeviceTab::getDevice() const
{
    // Return audio device info corresponding to selected device in combobox (empty device if there are no devices)
    return combobox_devices->count() != 0 ? devices[combobox_devices->currentIndex()] : QAudioDevice();
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

    // Create new list and fill in combobox
    switch (type)
    {
        case INPUT:
        {
            devices = QMediaDevices::audioInputs();
            break;
        }
        case OUTPUT:
        {
            devices = QMediaDevices::audioOutputs();
            break;
        }
    }
    for (int i=0; i < devices.count(); i++)
    {
        combobox_devices->addItem(devices[i].description());
    }

    // Release combobox
    combobox_devices->blockSignals(false);
}