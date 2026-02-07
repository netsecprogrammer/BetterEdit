// Stub implementations for pro:: functions that were in excluded supporter CPP files
// These are needed because the headers declare them and non-supporter code calls them
// TODO: Re-enable when server/web API is ported to v5

#include <features/supporters/Pro.hpp>
#include <features/about/AboutBEPopup.hpp>
#include <Geode/binding/FLAlertLayer.hpp>

void pro::addAboutPopupStuff(AboutBEPopup*) {
    // Stub: supporter features disabled in v5 alpha
}

ccColor3B pro::getSupporterColor(int) {
    return ccWHITE;
}

void pro::showProOnlyFeaturePopup(std::string_view) {
    FLAlertLayer::create(
        "Supporter Feature",
        "Supporter features are not yet available in this version.",
        "OK"
    )->show();
}
