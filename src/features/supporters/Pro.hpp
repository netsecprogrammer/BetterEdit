#pragma once

#include <Geode/DefaultInclude.hpp>

using namespace geode::prelude;

class AboutBEPopup;

namespace pro {
    // Supporter verification is disabled in v5 alpha - server/OpenSSL not yet ported
    inline size_t verifyPro() {
        return 0;
    }

    // Interfaces for free

    void addAboutPopupStuff(AboutBEPopup* popup);

    ccColor3B getSupporterColor(int supportedAmount);

    void showProOnlyFeaturePopup(std::string_view featureName);
}

#define HAS_PRO() (pro::verifyPro())
