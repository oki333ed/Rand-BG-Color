using namespace geode::prelude;

std::vector<ccColor3B> getRandomColors() {
    return {
        {0x00, 0x66, 0xFF},
        {0xFF, 0x4C, 0x00},
        {0xFF, 0xAB, 0x00},
        {0xFF, 0xEC, 0x00},
        {0xA7, 0xFF, 0x00},
        {0x00, 0xEA, 0xFF},
        {0x00, 0x27, 0xFF},
        {0x7E, 0x00, 0xFF},
        {0xE8, 0x00, 0xFF},
        {0xDF, 0xDF, 0xDF},
        {0x94, 0x94, 0x94},
        {0x45, 0x45, 0x45},
        {0x4C, 0x4C, 0x4C},
        {0xA4, 0x00, 0xFF},
        {0x28, 0x28, 0x28},
        {0x25, 0x32, 0xA7},
        {0x4B, 0x0A, 0x00},
        {0x00, 0x3C, 0xB4}
    };
}

#include <random>
ccColor3B getRandomColor() {
    static std::random_device rd;
    static std::mt19937 rng(rd());
    static auto colors = getRandomColors();
    std::uniform_int_distribution<size_t> dist(0, colors.size() - 1);
    return colors[dist(rng)];
}

#include <Geode/modify/CCLayer.hpp>
class $modify(CCLayer) {
    void onEnter() override {
        CCLayer::onEnter();

        auto mod = Mod::get();

        std::string typeName = typeid(*this).name();

        if (
            (exact_cast<GauntletLayer*>(this) && !mod->getSettingValue<bool>("gauntletlayer")) ||
            exact_cast<GauntletSelectLayer*>(this) ||
            exact_cast<SecretRewardsLayer*>(this) ||
            exact_cast<LevelSelectLayer*>(this) ||
            (typeName.find("SogLayer") != std::string::npos) ||
            (typeName.find("MeltdownSelectLayer") != std::string::npos) ||
            (typeName.find("SubZeroSelectLayer") != std::string::npos) ||
            exact_cast<LevelAreaInnerLayer*>(this) ||
            (exact_cast<GJGarageLayer*>(this) && !mod->getSettingValue<bool>("gjgaragelayer")) ||
            (exact_cast<CreatorLayer*>(this) && !mod->getSettingValue<bool>("creatorlayer")) ||
            (exact_cast<LoadingLayer*>(this) && !mod->getSettingValue<bool>("loadinglayer")) ||
            (typeName.find("GlobedServersLayer") != std::string::npos && !mod->getSettingValue<bool>("globedlayers")) ||
            (typeName.find("GlobedMenuLayer") != std::string::npos && !mod->getSettingValue<bool>("globedlayers")) ||
            (exact_cast<SecretLayer4*>(this) && !mod->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<SecretLayer2*>(this) && !mod->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<SecretLayer*>(this) && !mod->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<LevelAreaLayer*>(this) && !mod->getSettingValue<bool>("levelarealayer"))
        )
            return;

        if (mod->getSettingValue<bool>("loadinglayer")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg-texture"))) {
                sprite->setColor(getRandomColor());
            }
        }

        if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("background"))) {
            sprite->setColor(getRandomColor());
        }

        if (mod->getSettingValue<bool>("cvoltonbetterinfobackground")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("cvolton.betterinfo/background"))) {
                sprite->setColor(getRandomColor());
            }
        }

        if (mod->getSettingValue<bool>("fix")) {
            auto children = this->getChildren();
            if (children) {
                CCObject* obj;
                CCARRAY_FOREACH(children, obj) {
                    auto node = static_cast<CCNode*>(obj);
                    if (auto scale9 = typeinfo_cast<CCScale9Sprite*>(node)) {
                        const ccColor3B& color = scale9->getColor();
                        GLubyte opacity = scale9->getOpacity();

                        if (opacity == 255 &&
                            (
                                (color.r == 0 && color.g == 39 && color.b == 98) ||
                                (color.r == 0 && color.g == 56 && color.b == 141) ||
                                (color.r == 0 && color.g == 36 && color.b == 96) ||
                                (color.r == 0 && color.g == 36 && color.b == 91)
                            )
                        ) {
                            scale9->setColor({0, 0, 0});
                            scale9->setOpacity(130);
                        }
                        else if (opacity == 255 &&
                            (
                                (color.r == 0 && color.g == 46 && color.b == 117) ||
                                (color.r == 0 && color.g == 31 && color.b == 79)
                            )
                        ) {
                            scale9->setColor({0, 0, 0});
                            scale9->setOpacity(140);
                        }
                    }
                }
            }
        }
    }

};
