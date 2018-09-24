#include "daide.h"
#include "daideplugin.h"

#include <QtPlugin>

DAidePlugin::DAidePlugin(QObject *parent)
    : QObject(parent)
{
    m_initialized = false;
}

void DAidePlugin::initialize(QDesignerFormEditorInterface * /* core */)
{
    if (m_initialized)
        return;

    // Add extension registrations, etc. here

    m_initialized = true;
}

bool DAidePlugin::isInitialized() const
{
    return m_initialized;
}

QWidget *DAidePlugin::createWidget(QWidget *parent)
{
    return new DAide(parent);
}

QString DAidePlugin::name() const
{
    return QLatin1String("DAide");
}

QString DAidePlugin::group() const
{
    return QLatin1String("");
}

QIcon DAidePlugin::icon() const
{
    return QIcon();
}

QString DAidePlugin::toolTip() const
{
    return QLatin1String("");
}

QString DAidePlugin::whatsThis() const
{
    return QLatin1String("");
}

bool DAidePlugin::isContainer() const
{
    return false;
}

QString DAidePlugin::domXml() const
{
    return QLatin1String("<widget class=\"DAide\" name=\"dAide\">\n</widget>\n");
}

QString DAidePlugin::includeFile() const
{
    return QLatin1String("daide.h");
}
#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(daideplugin, DAidePlugin)
#endif // QT_VERSION < 0x050000
