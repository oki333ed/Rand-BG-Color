using namespace geode::prelude;

#include <matjson.hpp>
std::vector<ccColor3B> parseColors(const std::string& jsonStr) {
    std::vector<ccColor3B> colors;

    auto parsed = matjson::parse(jsonStr);
    if (!parsed) return colors;

    auto& json = parsed.unwrap();
    if (!json.contains("colors")) return colors;

    auto arr = json["colors"].asArray();
    if (!arr) return colors;

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
    static std::random_device rd;
    static std::mt19937 rng(rd());

    if (Mod::get()->getSettingValue<bool>("randomcolors")) {
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

    if (cachedColors.empty()) return {255, 255, 255};

    std::uniform_int_distribution<size_t> dist(0, cachedColors.size() - 1);
    return cachedColors[dist(rng)];
}

bool isDefaultBG(const std::string& path) {
    return path.find("GJ_gradientBG-hd.png") != std::string::npos ||
           path.find("GJ_gradientBG.png") != std::string::npos;
}

bool applyTexture(CCSprite* sprite, const std::filesystem::path& path) {
    if (!sprite || path.empty()) return false;

    std::string pathStr = geode::utils::string::pathToString(path);
    if (pathStr.empty() || isDefaultBG(pathStr)) return false;

    auto newSprite = CCSprite::create(pathStr.c_str());
    if (!newSprite) return false;

    auto tex = newSprite->getTexture();
    if (!tex) return false;

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto texSize = tex->getContentSize();

    if (texSize.width <= 0 || texSize.height <= 0) return false;

    float scaleX = winSize.width / texSize.width;
    float scaleY = winSize.height / texSize.height;

    sprite->setTexture(tex);
    sprite->setTextureRect(CCRect(0, 0, texSize.width, texSize.height));
    sprite->setScaleX(scaleX);
    sprite->setScaleY(scaleY);

    sprite->setAnchorPoint({0.5f, 0.5f});
    sprite->setPosition({winSize.width / 2, winSize.height / 2});

    return true;
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
            (exact_cast<LevelInfoLayer*>(this) && !mod->getSettingValue<bool>("levelinfolayer"))
        )
            return;

        std::string colorsJson = mod->getSettingValue<std::string>("colors");
        ccColor3B randomColor = getRandomColorFromSetting(colorsJson);

        auto texturePath = mod->getSettingValue<std::filesystem::path>("bg-texture");

        auto tryApply = [&](CCSprite* sprite) {
            if (!sprite) return;
            if (applyTexture(sprite, texturePath)) {
                sprite->setColor(randomColor);
            }
        };

        if (mod->getSettingValue<bool>("loadinglayer")) {
            tryApply(typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg-texture")));
        }

        tryApply(typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("background")));

        if (mod->getSettingValue<bool>("cvoltonbetterinfobackground")) {
            tryApply(typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("cvolton.betterinfo/background")));
        }

        tryApply(typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg")));

        auto children = this->getChildren();
        if (children) {
            for (CCNode* node : this->getChildrenExt()) {
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
};
