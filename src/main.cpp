using namespace geode::prelude;

#include <matjson.hpp>
std::vector<ccColor3B> parseColors(const std::string& jsonStr) {
    std::vector<ccColor3B> colors;

    auto parsed = matjson::parse(jsonStr);
    if (!parsed) {
        return colors;
    }

    auto& json = parsed.unwrap();
    if (!json.contains("colors")) {
        return colors;
    }

    auto arr = json["colors"].asArray();
    if (!arr) {
        return colors;
    }

    for (auto const& c : arr.unwrap()) {
        int r = c.contains("r") ? c["r"].as<int>().unwrapOr(255) : 255;
        int g = c.contains("g") ? c["g"].as<int>().unwrapOr(255) : 255;
        int b = c.contains("b") ? c["b"].as<int>().unwrapOr(255) : 255;

        colors.push_back({
            static_cast<GLubyte>(r),
            static_cast<GLubyte>(g),
            static_cast<GLubyte>(b)
        });
    }

    return colors;
}

#include <random>
ccColor3B getRandomColorFromSetting(const std::string& colorsJson) {
    static std::mt19937 rng(std::random_device{}());

    bool realRand = Mod::get()->getSettingValue<bool>("realrandcolors");

    if (realRand) {
        std::uniform_int_distribution<int> dist(0, 255);
        return {
            static_cast<GLubyte>(dist(rng)),
            static_cast<GLubyte>(dist(rng)),
            static_cast<GLubyte>(dist(rng))
        };
    }

    static std::vector<ccColor3B> cachedColors;
    static std::string lastJson;

    if (colorsJson != lastJson) {
        cachedColors = parseColors(colorsJson);
        lastJson = colorsJson;
    }

    if (cachedColors.empty()) {
        return {255, 255, 255};
    }

    std::uniform_int_distribution<size_t> dist(0, cachedColors.size() - 1);
    return cachedColors[dist(rng)];
}

#include <Geode/modify/CCLayer.hpp>
class $modify(CCLayer) {
 /* static void onModify(auto& self) {
        self.setHookPriorityBeforePost(
            "CCLayer::init",
            "alphalaneous.happy_textures"
        );
    } */

    void onEnter() override {
        CCLayer::onEnter();

        std::string typeName = typeid(*this).name();

        if (
            (exact_cast<GauntletLayer*>(this) && !Mod::get()->getSettingValue<bool>("gauntletlayer")) ||
            exact_cast<GauntletSelectLayer*>(this) ||
            exact_cast<SecretRewardsLayer*>(this) ||
            exact_cast<LevelSelectLayer*>(this) ||
            (typeName.find("SogLayer") != std::string::npos) ||
         /* (typeName.find("MeltdownSelectLayer") != std::string::npos) ||
            (typeName.find("SubZeroSelectLayer") != std::string::npos) || */
            exact_cast<LevelAreaInnerLayer*>(this) ||
            (exact_cast<GJGarageLayer*>(this) && !Mod::get()->getSettingValue<bool>("gjgaragelayer")) ||
            (exact_cast<CreatorLayer*>(this) && !Mod::get()->getSettingValue<bool>("creatorlayer")) ||
            (exact_cast<LoadingLayer*>(this) && !Mod::get()->getSettingValue<bool>("loadinglayer")) ||
            (typeName.find("GlobedServersLayer") != std::string::npos && !Mod::get()->getSettingValue<bool>("globedlayers")) ||
            (typeName.find("GlobedMenuLayer") != std::string::npos && !Mod::get()->getSettingValue<bool>("globedlayers")) ||
            (exact_cast<SecretLayer4*>(this) && !Mod::get()->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<SecretLayer2*>(this) && !Mod::get()->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<SecretLayer*>(this) && !Mod::get()->getSettingValue<bool>("secretlayers")) ||
            (exact_cast<LevelAreaLayer*>(this) && !Mod::get()->getSettingValue<bool>("levelarealayer")) ||
            (exact_cast<LevelInfoLayer*>(this) && !Mod::get()->getSettingValue<bool>("levelinfolayer"))
        )
            return;

        std::string colorsJson = Mod::get()->getSettingValue<std::string>("colors");
        ccColor3B randomColor = getRandomColorFromSetting(colorsJson);

        if (Mod::get()->getSettingValue<bool>("loadinglayer")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg-texture"))) {
                sprite->setColor(randomColor);
            }
        }

        if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("background"))) {
            sprite->setColor(randomColor);
        }

        if (Mod::get()->getSettingValue<bool>("cvoltonbetterinfobackground")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("cvolton.betterinfo/background"))) {
                sprite->setColor(randomColor);
            }
        }

        if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg"))) {
            sprite->setColor(randomColor);
        }

        auto children = this->getChildren();
        if (children) {
            for (CCNode* node : this->getChildrenExt()) {
                if (auto scaleSprite = typeinfo_cast<CCScale9Sprite*>(node)) {
                    const ccColor3B& color = scaleSprite->getColor();
                    GLubyte opacity = scaleSprite->getOpacity();

                    if (opacity == 255 &&
                        (
                            (color.r == 0 && color.g == 39 && color.b == 98) ||
                            (color.r == 0 && color.g == 56 && color.b == 141) ||
                            (color.r == 0 && color.g == 36 && color.b == 96) ||
                            (color.r == 0 && color.g == 36 && color.b == 91)
                        )
                    ) {
                        scaleSprite->setColor({0, 0, 0});
                        scaleSprite->setOpacity(130);
                    }
                    else if (opacity == 255 &&
                        (
                            (color.r == 0 && color.g == 46 && color.b == 117) ||
                            (color.r == 0 && color.g == 31 && color.b == 79)
                        )
                    ) {
                        scaleSprite->setColor({0, 0, 0});
                        scaleSprite->setOpacity(140);
                    }
                }
            }
        }
    }
};