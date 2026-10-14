#include <QCoreApplication>
#include <QJsonObject>
#include <QPluginLoader>
#include <QString>
#include <iostream>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    if(argc != 2)
    {
        std::cerr << "Usage: apex_plugin_metadata_check /absolute/path/to/plugin.so\n";
        return 2;
    }

    QPluginLoader loader(QString::fromLocal8Bit(argv[1]));
    const QJsonObject outer = loader.metaData();
    const QJsonObject data = outer.value("MetaData").toObject();

    std::cout << "Plugin file: " << argv[1] << "\n";
    std::cout << "Qt IID: " << outer.value("IID").toString().toStdString() << "\n";
    std::cout << "Plugin Id: " << data.value("Id").toString().toStdString() << "\n";
    std::cout << "OpenRGB API version: " << data.value("OpenRGBPluginAPIVersion").toInt(-1) << "\n";

    if(outer.isEmpty())
    {
        std::cerr << "Qt cannot read plugin metadata: " << loader.errorString().toStdString() << "\n";
        return 1;
    }
    if(data.value("Id").toString() != QStringLiteral("org.openrgb.apexpromini")
       || data.value("OpenRGBPluginAPIVersion").toInt(-1) != 5)
    {
        std::cerr << "Invalid or missing OpenRGB MetaData fields.\n";
        return 1;
    }
    return 0;
}
