using namespace geode::prelude;

std::vector<ccColor3B> parseColorsFromJsonString(const std::string& json) {
    std::vector<ccColor3B> colors;

    size_t pos = json.find("\"colors\":[");
    if (pos == std::string::npos)
        return colors;

    pos += 9;

    while (true) {
        pos = json.find('{', pos);
        if (pos == std::string::npos)
            break;

        size_t endPos = json.find('}', pos);
        if (endPos == std::string::npos)
            break;

        std::string obj = json.substr(pos, endPos - pos + 1);

        auto getValue = [&](const std::string& key) -> int {
            size_t kpos = obj.find("\"" + key + "\"");
            if (kpos == std::string::npos)
                return -1;
            size_t colon = obj.find(':', kpos);
            if (colon == std::string::npos)
                return -1;

            size_t start = colon + 1;
            while (start < obj.size() && !isdigit(obj[start]))
                start++;

            size_t end = start;
            while (end < obj.size() && isdigit(obj[end]))
                end++;

            if (start == end)
                return -1;

            return std::stoi(obj.substr(start, end - start));
        };

        int r = getValue("r");
        int g = getValue("g");
        int b = getValue("b");

        if (r >= 0 && g >= 0 && b >= 0 && r <= 255 && g <= 255 && b <= 255) {
            colors.push_back(ccColor3B{static_cast<GLubyte>(r), static_cast<GLubyte>(g), static_cast<GLubyte>(b)});
        }

        pos = endPos + 1;
    }

    return colors;
}

#include <random>
ccColor3B getRandomColorFromSetting(const std::string& colorsJson) {
    static std::random_device rd;
    static std::mt19937 rng(rd());

    static std::vector<ccColor3B> cachedColors;
    static std::string lastJson;

    if (colorsJson != lastJson) {
        cachedColors = parseColorsFromJsonString(colorsJson);
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

        std::string colorsJson = mod->getSettingValue<std::string>("colors");

        ccColor3B randomColor = getRandomColorFromSetting(colorsJson);

        if (mod->getSettingValue<bool>("loadinglayer")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("bg-texture"))) {
                sprite->setColor(randomColor);
            }
        }

        if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("background"))) {
            sprite->setColor(randomColor);
        }

        if (mod->getSettingValue<bool>("cvoltonbetterinfobackground")) {
            if (auto sprite = typeinfo_cast<CCSprite*>(this->getChildByIDRecursive("cvolton.betterinfo/background"))) {
                sprite->setColor(randomColor);
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