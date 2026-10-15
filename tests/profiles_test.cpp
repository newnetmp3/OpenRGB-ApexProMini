#include "ApexMiniProfiles.h"
#include <QCoreApplication>
#include <QTemporaryDir>
#include <cassert>

int main(int argc,char** argv)
{
    QCoreApplication app(argc,argv);
    QTemporaryDir temp;
    assert(temp.isValid());
    qputenv("XDG_CONFIG_HOME",temp.path().toUtf8());
    using namespace apex_mini;
    assert(valid_profile_name(QStringLiteral("My Key Colors")));
    assert(!valid_profile_name(QStringLiteral("../invalid")));
    assert(!valid_profile_name(QStringLiteral("escape/name")));
    LightingProfile p;
    p.effect=factory_effect_base+29;
    p.speed=175;
    p.brightness=67;
    p.reverse=true;
    p.base={2,40,95};
    p.accent={250,9,17};
    for(const auto hid : hid_codes) p.saved_colors.push_back({hid,{hid,2,3}});
    const auto raw=encode_profile(p);
    LightingProfile restored;
    assert(decode_profile(raw,restored));
    assert(restored.effect==p.effect && restored.speed==175);
    assert(restored.brightness==67 && restored.reverse);
    assert(restored.base.g==40 && restored.accent.r==250);
    assert(restored.saved_colors.size()==key_count);
    assert(restored.saved_colors[20].hid==hid_codes[20]);
    auto damaged=raw;
    damaged.insert(QStringLiteral("version"),3);
    assert(!decode_profile(damaged,restored));
    damaged=raw;
    damaged.insert(QStringLiteral("effect"),999);
    assert(!decode_profile(damaged,restored));
    damaged=raw;
    damaged.insert(QStringLiteral("brightness"),101);
    assert(!decode_profile(damaged,restored));
    damaged=raw;
    QJsonArray wrong=damaged.value(QStringLiteral("colors")).toArray();
    wrong[0]=QJsonObject{{QStringLiteral("hid"),99},{QStringLiteral("rgb"),0}};
    damaged.insert(QStringLiteral("colors"),wrong);
    assert(!decode_profile(damaged,restored));
    QString error;
    assert(save_profile(QStringLiteral("QA profile"),p,error));
    LightingProfile from_disk;
    assert(load_profile(QStringLiteral("QA profile"),from_disk,error));
    assert(from_disk.saved_colors.size()==key_count);
    assert(from_disk.effect==p.effect && from_disk.base.b==95);
}
