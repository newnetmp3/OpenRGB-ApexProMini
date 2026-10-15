#pragma once
#include "apex_pro_mini_catalog.h"
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QString>

namespace apex_mini
{
struct LightingProfile
{
    int effect = 0;
    int speed = 100;
    int brightness = 100;
    bool reverse = false;
    Color base{18,30,65};
    Color accent{0,230,255};
    std::vector<KeyColor> saved_colors;
};

inline QString profile_directory()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation)
        + QStringLiteral("/apex-pro-mini/profiles");
}
inline bool valid_profile_name(const QString& name)
{
    if(name.isEmpty() || name.size() > 48 || name == QStringLiteral(".") ||
       name == QStringLiteral("..")) return false;
    for(const auto character : name)
        if(!(character.isLetterOrNumber() || character == QLatin1Char('-') ||
             character == QLatin1Char('_') || character == QLatin1Char(' ')))
            return false;
    return name.trimmed() == name;
}
inline bool valid_effect(int effect)
{
    return effect >= 0 && effect <= last_basic_effect ||
           effect >= factory_effect_base &&
           effect < factory_effect_base + static_cast<int>(factory_presets.size());
}
inline QJsonObject encode_profile(const LightingProfile& p)
{
    QJsonObject obj;
    obj.insert(QStringLiteral("version"),1);
    obj.insert(QStringLiteral("effect"),p.effect);
    obj.insert(QStringLiteral("speed"),p.speed);
    obj.insert(QStringLiteral("brightness"),p.brightness);
    obj.insert(QStringLiteral("reverse"),p.reverse);
    obj.insert(QStringLiteral("base"),static_cast<int>(p.base.r)*65536+
                                      static_cast<int>(p.base.g)*256+p.base.b);
    obj.insert(QStringLiteral("accent"),static_cast<int>(p.accent.r)*65536+
                                        static_cast<int>(p.accent.g)*256+p.accent.b);
    QJsonArray colors;
    for(const auto& entry : p.saved_colors)
    {
        QJsonObject item;
        item.insert(QStringLiteral("hid"),static_cast<int>(entry.hid));
        item.insert(QStringLiteral("rgb"),static_cast<int>(entry.color.r)*65536+
                                          static_cast<int>(entry.color.g)*256+entry.color.b);
        colors.append(item);
    }
    obj.insert(QStringLiteral("colors"),colors);
    return obj;
}
inline bool decode_profile(const QJsonObject& obj, LightingProfile& p)
{
    if(obj.value(QStringLiteral("version")).toInt(-1) != 1) return false;
    const int effect=obj.value(QStringLiteral("effect")).toInt(-1);
    const int speed=obj.value(QStringLiteral("speed")).toInt(-1);
    const int brightness=obj.value(QStringLiteral("brightness")).toInt(-1);
    const auto base=obj.value(QStringLiteral("base"));
    const auto accent=obj.value(QStringLiteral("accent"));
    const auto reverse=obj.value(QStringLiteral("reverse"));
    if(!valid_effect(effect) || speed<25 || speed>300 ||
       brightness<0 || brightness>100 ||
       !base.isDouble() || !accent.isDouble() || !reverse.isBool()) return false;
    const int b=base.toInt(-1), a=accent.toInt(-1);
    if(b<0 || b>0xffffff || a<0 || a>0xffffff) return false;
    LightingProfile candidate;
    candidate.effect=effect;
    candidate.speed=speed;
    candidate.brightness=brightness;
    candidate.reverse=reverse.toBool();
    candidate.base=from_hex(static_cast<std::uint32_t>(b));
    candidate.accent=from_hex(static_cast<std::uint32_t>(a));
    const auto colors=obj.value(QStringLiteral("colors"));
    if(!colors.isArray() || colors.toArray().size() != static_cast<int>(key_count)) return false;
    const auto array=colors.toArray();
    for(std::size_t i=0;i<key_count;++i)
    {
        if(!array[static_cast<int>(i)].isObject()) return false;
        const auto item=array[static_cast<int>(i)].toObject();
        const int hid=item.value(QStringLiteral("hid")).toInt(-1);
        const int rgb=item.value(QStringLiteral("rgb")).toInt(-1);
        if(hid != hid_codes[i] || rgb<0 || rgb>0xffffff) return false;
        candidate.saved_colors.push_back({static_cast<std::uint8_t>(hid),
                                 from_hex(static_cast<std::uint32_t>(rgb))});
    }
    p=std::move(candidate);
    return true;
}
inline bool save_profile(const QString& name,const LightingProfile& p,QString& error)
{
    if(!valid_profile_name(name))
    {
        error=QStringLiteral("Profile name must be 1–48 letters, digits, spaces, - or _.");
        return false;
    }
    QDir directory;
    if(!directory.mkpath(profile_directory()))
    {
        error=QStringLiteral("Could not create profile directory.");
        return false;
    }
    QSaveFile file(profile_directory()+QLatin1Char('/')+name+QStringLiteral(".json"));
    if(!file.open(QIODevice::WriteOnly))
    {
        error=file.errorString();
        return false;
    }
    const QByteArray bytes=QJsonDocument(encode_profile(p)).toJson(QJsonDocument::Indented);
    if(file.write(bytes)!=bytes.size() || !file.commit())
    {
        error=file.errorString();
        return false;
    }
    return true;
}
inline bool load_profile(const QString& name,LightingProfile& p,QString& error)
{
    if(!valid_profile_name(name)) { error=QStringLiteral("Invalid profile name"); return false; }
    QFile file(profile_directory()+QLatin1Char('/')+name+QStringLiteral(".json"));
    if(!file.open(QIODevice::ReadOnly)) { error=file.errorString(); return false; }
    if(file.size()>65536) { error=QStringLiteral("Profile is too large"); return false; }
    QJsonParseError parse{};
    const auto json=QJsonDocument::fromJson(file.readAll(),&parse);
    if(parse.error!=QJsonParseError::NoError || !json.isObject() ||
       !decode_profile(json.object(),p))
    {
        error=QStringLiteral("Invalid or unsupported profile format");
        return false;
    }
    return true;
}
} // namespace apex_mini
