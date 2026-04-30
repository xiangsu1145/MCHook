#include "ClickGui.h"
#include <imgui/imgui.h>
#include "../../FeatureManager.h"
#include "src/hook/WGLHook.h"

#define M_PI 3.14159265358979323846

namespace Theme {
    constexpr ImU32 WinBg = IM_COL32(252, 252, 254, 255);
    constexpr ImU32 SidebarBg = IM_COL32(248, 248, 250, 255);
    constexpr ImU32 CardBg = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 SettBg = IM_COL32(250, 250, 252, 255);
    constexpr ImU32 Border = IM_COL32(215, 218, 225, 255);
    constexpr ImU32 Accent = IM_COL32(85, 135, 220, 255);
    constexpr ImU32 TextPrimary = IM_COL32(45, 48, 55, 255);
    constexpr ImU32 TextSecondary = IM_COL32(120, 125, 140, 255);
    constexpr ImU32 TextWhite = IM_COL32(255, 255, 255, 255);
    constexpr ImU32 SearchBg = IM_COL32(242, 243, 246, 255);
    constexpr ImU32 SwitchOff = IM_COL32(200, 202, 210, 255);
    constexpr ImU32 SwitchOn = IM_COL32(85, 135, 220, 255);
    constexpr ImU32 SliderTrack = IM_COL32(220, 222, 228, 255);
    constexpr ImU32 SliderFill = IM_COL32(85, 135, 220, 255);
}

namespace Dim {
    float WinW = 720.f;
    float WinH = 540.f;
    float SidebarW = 140.f;
    float TitleH = 42.f;
    float SearchH = 36.f;
    float TabH = 36.f;
    float ModH = 40.f;
    float SettH = 36.f;
    float SwitchW = 36.f;
    float SwitchH = 18.f;
    float Round = 8.f;
    float Pad = 14.f;
}

static std::unordered_map<std::string, bool> gExpanded;
static int sSelectedTab = 0;
static char sSearchBuf[64] = { 0 };
static float gWinAlpha = 0.f;
static float gTabSelected[5] = { 0.f };
static bool sSearchActive = false;

void ClickGui::onEnable() {
    gFeatureManager->mDispatcher->listen<RenderEvent, &ClickGui::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<KeyEvent, &ClickGui::onKeyEvent>(this);
    gFeatureManager->mDispatcher->listen<MouseEvent, &ClickGui::onMouseEvent>(this);
    gWinAlpha = 0.f;
}

void ClickGui::onDisable() {
    // Don't deafen - we keep the listener registered and just skip rendering via mEnabled check
}

void ClickGui::onKeyEvent(KeyEvent&) {}
void ClickGui::onMouseEvent(MouseEvent&) {}

static float easeOut(float t) { return 1.f - powf(1.f - t, 3.f); }
static float lerp(float a, float b, float t) { return a + (b - a) * t; }
static void animTo(float& v, float tgt, float spd, float dt) { v += (tgt - v) * (std::min)(spd * dt, 1.f); }

static ImU32 lerpC(ImU32 a, ImU32 b, float t) {
    ImVec4 ca = ImGui::ColorConvertU32ToFloat4(a);
    ImVec4 cb = ImGui::ColorConvertU32ToFloat4(b);
    return ImGui::ColorConvertFloat4ToU32({ lerp(ca.x,cb.x,t), lerp(ca.y,cb.y,t), lerp(ca.z,cb.z,t), lerp(ca.w,cb.w,t) });
}

static bool inRect(float x, float y, float w, float h, ImVec2 p) {
    return p.x >= x && p.x <= x + w && p.y >= y && p.y <= y + h;
}

extern ImFont* gFontNormal;
extern ImFont* gFontBold;

static float txtW(const char* t, float sz = 12.f) { return gFontNormal ? gFontNormal->CalcTextSizeA(sz, FLT_MAX, 0.f, t).x : 10.f; }
static void txt(ImDrawList* dl, float x, float y, ImU32 c, const char* t, float sz = 12.f) { if (gFontNormal) dl->AddText(gFontNormal, sz, { x, y }, c, t); }

static void drawSwitch(ImDrawList* dl, float x, float y, bool on, float& anim, float dt) {
    animTo(anim, on ? 1.f : 0.f, 14.f, dt);
    float r = Dim::SwitchH * 0.5f;
    ImU32 track = lerpC(Theme::SwitchOff, Theme::SwitchOn, anim);
    dl->AddRectFilled({ x, y }, { x + Dim::SwitchW, y + Dim::SwitchH }, track, r);
    float knob = lerp(x + r, x + Dim::SwitchW - r, anim);
    dl->AddCircleFilled({ knob, y + r }, r - 2.f, IM_COL32(255, 255, 255, 255));
}

static void drawSlider(ImDrawList* dl, float x, float y, float w, float val, float mn, float mx, float& anim, float dt) {
    float t = std::clamp((val - mn) / (mx - mn), 0.f, 1.f);
    animTo(anim, t, 12.f, dt);
    float fill = w * anim;
    dl->AddRectFilled({ x, y }, { x + w, y + 4.f }, Theme::SliderTrack, 2.f);
    if (fill > 0) dl->AddRectFilled({ x, y }, { x + fill, y + 4.f }, Theme::SliderFill, 2.f);
    dl->AddCircleFilled({ x + fill, y + 2.f }, 5.f, Theme::Accent);
    dl->AddCircleFilled({ x + fill, y + 2.f }, 3.f, IM_COL32(255, 255, 255, 255));
}

static void drawRange(ImDrawList* dl, float x, float y, float w, float valMin, float valMax, float mn, float mx, float& animMin, float& animMax, float dt) {
    float tMin = std::clamp((valMin - mn) / (mx - mn), 0.f, 1.f);
    float tMax = std::clamp((valMax - mn) / (mx - mn), 0.f, 1.f);
    animTo(animMin, tMin, 12.f, dt);
    animTo(animMax, tMax, 12.f, dt);
    float fillMin = w * animMin;
    float fillMax = w * animMax;
    dl->AddRectFilled({ x, y }, { x + w, y + 4.f }, Theme::SliderTrack, 2.f);
    dl->AddRectFilled({ x + fillMin, y }, { x + fillMax, y + 4.f }, Theme::SliderFill, 2.f);
    dl->AddCircleFilled({ x + fillMin, y + 2.f }, 5.f, Theme::Accent);
    dl->AddCircleFilled({ x + fillMin, y + 2.f }, 3.f, IM_COL32(255, 255, 255, 255));
    dl->AddCircleFilled({ x + fillMax, y + 2.f }, 5.f, Theme::Accent);
    dl->AddCircleFilled({ x + fillMax, y + 2.f }, 3.f, IM_COL32(255, 255, 255, 255));
}

static float calcSettH(Module* m) {
    float h = 0;
    for (auto s : m->mSettings) {
        if (!s->mIsVisible()) continue;
        h += Dim::SettH;
        if (s->mType == SettingType::Enum && s->enumExtended) {
            auto e = static_cast<EnumSettingT<int>*>(s);
            h += (Dim::SettH - 2.f) * (float)e->mValues.size();
        }
    }
    return h;
}

static void drawSettings(ImDrawList* dl, float x, float y, float w, Module* m, ImVec2 mouse, bool lclick, float dt) {
    float ix = x + 10.f;
    float iw = w - 20.f;

    for (auto s : m->mSettings) {
        if (!s->mIsVisible()) continue;

        if (s->mType == SettingType::Bool) {
            auto bs = static_cast<BoolSetting*>(s);
            dl->AddRectFilled({ x, y }, { x + w, y + Dim::SettH }, Theme::SettBg, 0);
            txt(dl, ix + 4.f, y + (Dim::SettH - 11.f) * 0.5f, Theme::TextSecondary, s->mName.c_str(), 11.f);
            drawSwitch(dl, ix + iw - Dim::SwitchW, y + (Dim::SettH - Dim::SwitchH) * 0.5f, bs->mValue, bs->boolScale, dt);
            if (lclick && inRect(x, y, w, Dim::SettH, mouse)) bs->mValue = !bs->mValue;
            y += Dim::SettH;
        }
        else if (s->mType == SettingType::Number) {
            auto ns = static_cast<NumberSetting*>(s);
            dl->AddRectFilled({ x, y }, { x + w, y + Dim::SettH }, Theme::SettBg, 0);
            char buf[32];
            snprintf(buf, sizeof(buf), "%.1f", ns->mValue);
            txt(dl, ix + 4.f, y + 6.f, Theme::TextSecondary, s->mName.c_str(), 10.f);
            txt(dl, ix + iw - txtW(buf, 10.f), y + 6.f, Theme::TextPrimary, buf, 10.f);
            drawSlider(dl, ix + 2.f, y + Dim::SettH - 14.f, iw - 4.f, ns->mValue, ns->mMin, ns->mMax, ns->sliderEase, dt);
            if (ImGui::IsMouseDown(0) && inRect(ix + 2.f - 5.f, y + Dim::SettH - 20.f, iw - 4.f + 10.f, 24.f, mouse)) {
                float nt = std::clamp((mouse.x - ix - 2.f) / (iw - 4.f), 0.f, 1.f);
                ns->setValue(ns->mMin + nt * (ns->mMax - ns->mMin));
            }
            y += Dim::SettH;
        }
        else if (s->mType == SettingType::Range) {
            auto rs = static_cast<RangeSetting*>(s);
            dl->AddRectFilled({ x, y }, { x + w, y + Dim::SettH }, Theme::SettBg, 0);
            char buf[48];
            snprintf(buf, sizeof(buf), "%.1f - %.1f", rs->mValueMin, rs->mValueMax);
            txt(dl, ix + 4.f, y + 6.f, Theme::TextSecondary, s->mName.c_str(), 10.f);
            txt(dl, ix + iw - txtW(buf, 10.f), y + 6.f, Theme::TextPrimary, buf, 10.f);
            drawRange(dl, ix + 2.f, y + Dim::SettH - 14.f, iw - 4.f, rs->mValueMin, rs->mValueMax, rs->mMin, rs->mMax, rs->sliderEase, rs->sliderEaseMax, dt);

            if (lclick && inRect(ix + 2.f - 5.f, y + Dim::SettH - 20.f, iw - 4.f + 10.f, 24.f, mouse)) {
                float nt = std::clamp((mouse.x - ix - 2.f) / (iw - 4.f), 0.f, 1.f);
                float tMin = std::clamp((rs->mValueMin - rs->mMin) / (rs->mMax - rs->mMin), 0.f, 1.f);
                float tMax = std::clamp((rs->mValueMax - rs->mMin) / (rs->mMax - rs->mMin), 0.f, 1.f);
                if (std::abs(nt - tMin) < std::abs(nt - tMax)) {
                    rs->isDraggingMin = true;
                } else {
                    rs->isDraggingMax = true;
                }
            }

            if (ImGui::IsMouseDown(0)) {
                if (rs->isDraggingMin) {
                    float nt = std::clamp((mouse.x - ix - 2.f) / (iw - 4.f), 0.f, 1.f);
                    float nv = rs->mMin + nt * (rs->mMax - rs->mMin);
                    if (nv > rs->mValueMax - 0.1f) nv = rs->mValueMax - 0.1f;
                    rs->setValue(nv, rs->mValueMax);
                } else if (rs->isDraggingMax) {
                    float nt = std::clamp((mouse.x - ix - 2.f) / (iw - 4.f), 0.f, 1.f);
                    float nv = rs->mMin + nt * (rs->mMax - rs->mMin);
                    if (nv < rs->mValueMin + 0.1f) nv = rs->mValueMin + 0.1f;
                    rs->setValue(rs->mValueMin, nv);
                }
            } else {
                rs->isDraggingMin = false;
                rs->isDraggingMax = false;
            }
            y += Dim::SettH;
        }
        else if (s->mType == SettingType::Enum) {
            auto es = static_cast<EnumSettingT<int>*>(s);
            animTo(s->enumSlide, s->enumExtended ? 1.f : 0.f, 10.f, dt);

            dl->AddRectFilled({ x, y }, { x + w, y + Dim::SettH }, Theme::SettBg, 0);
            txt(dl, ix + 4.f, y + (Dim::SettH - 11.f) * 0.5f, Theme::TextSecondary, s->mName.c_str(), 11.f);
            const std::string& cur = es->mValues[(int)es->mValue];
            txt(dl, ix + iw - txtW(cur.c_str(), 11.f) - 14.f, y + (Dim::SettH - 11.f) * 0.5f, Theme::Accent, cur.c_str(), 11.f);

            if (lclick && inRect(x, y, w, Dim::SettH, mouse)) {
                int next = ((int)es->mValue + 1) % (int)es->mValues.size();
                es->setEnumIndex(next);
            }
            y += Dim::SettH;

            float enumH = (Dim::SettH - 2.f) * (float)es->mValues.size() * easeOut(s->enumSlide);
            if (enumH > 0.5f) {
                for (int i = 0; i < (int)es->mValues.size(); i++) {
                    bool sel = (i == (int)es->mValue);
                    float rh = Dim::SettH - 2.f;
                    ImU32 bg = sel ? IM_COL32(240, 245, 252, 255) : Theme::SettBg;
                    dl->AddRectFilled({ x, y }, { x + w, y + rh }, bg, 0);
                    if (sel) dl->AddCircleFilled({ ix + 8.f, y + rh * 0.5f }, 3.f, Theme::Accent);
                    txt(dl, ix + 18.f, y + (rh - 11.f) * 0.5f, sel ? Theme::Accent : Theme::TextSecondary, es->mValues[i].c_str(), 11.f);
                    if (lclick && inRect(x, y, w, rh, mouse)) {
                        es->setEnumIndex(i);
                        s->enumExtended = false;
                    }
                    y += rh;
                }
            }
        }
        else if (s->mType == SettingType::String) {
            auto ss = static_cast<StringSetting*>(s);
            dl->AddRectFilled({ x, y }, { x + w, y + Dim::SettH }, Theme::SettBg, 0);
            txt(dl, ix + 4.f, y + (Dim::SettH - 11.f) * 0.5f, Theme::TextSecondary, s->mName.c_str(), 11.f);
            txt(dl, ix + iw - txtW(ss->mValue.c_str(), 11.f), y + (Dim::SettH - 11.f) * 0.5f, Theme::TextPrimary, ss->mValue.c_str(), 11.f);
            y += Dim::SettH;
        }
    }
}

static void drawModule(ImDrawList* dl, float x, float y, float w, Module* mod, ImVec2 mouse, bool lclick, bool rclick, float dt, float& outH) {
    if (rclick && inRect(x, y, w, Dim::ModH, mouse)) {
        gExpanded[mod->mName] = !gExpanded[mod->mName];
    }

    bool expanded = gExpanded[mod->mName];

    animTo(mod->mExpandAnim, expanded ? 1.f : 0.f, 8.f, dt);
    float settH = calcSettH(mod) * easeOut(mod->mExpandAnim);
    float totalH = Dim::ModH + settH;
    if (totalH < Dim::ModH) totalH = Dim::ModH;

    dl->AddRectFilled({ x, y }, { x + w, y + totalH }, Theme::CardBg, Dim::Round);

    if (mod->mEnabled) {
        dl->AddRectFilled({ x, y }, { x + 3.f, y + totalH }, Theme::Accent, 0);
    }

    float animEnabled = mod->mEnabled ? 1.f : 0.f;
    ImU32 dot = lerpC(IM_COL32(180, 185, 200, 255), Theme::Accent, animEnabled);
    dl->AddCircleFilled({ x + 16.f, y + Dim::ModH * 0.5f }, 4.f, dot);

    ImU32 nameC = lerpC(Theme::TextSecondary, Theme::TextPrimary, animEnabled);
    txt(dl, x + 28.f, y + (Dim::ModH - 12.f) * 0.5f, nameC, mod->mName.c_str(), 12.f);

    char kb[8];
    snprintf(kb, sizeof(kb), "[%c]", (char)mod->mKey);
    txt(dl, x + w - txtW(kb, 10.f) - 16.f, y + (Dim::ModH - 10.f) * 0.5f, Theme::TextSecondary, kb, 10.f);

    if (lclick && inRect(x, y, w, Dim::ModH, mouse)) {
        mod->setEnabled(!mod->mEnabled);
    }

    outH = totalH;

    if (settH > 0.5f) {
        dl->PushClipRect({ x, y + Dim::ModH }, { x + w, y + totalH }, true);
        drawSettings(dl, x, y + Dim::ModH, w, mod, mouse, lclick, dt);
        dl->PopClipRect();
    }
}

void ClickGui::onRenderEvent(RenderEvent&) {
    if (!mEnabled) return;

    ImGuiIO& io = ImGui::GetIO();
    int currentFrame = ImGui::GetFrameCount();
    if (currentFrame == mLastFrame) return;
    mLastFrame = currentFrame;
    auto* dl = ImGui::GetBackgroundDrawList();
    ImVec2 mouse = io.MousePos;
    float dt = io.DeltaTime;
    bool lclick = ImGui::IsMouseReleased(0);
    bool rclick = ImGui::IsMouseReleased(1);

    animTo(gWinAlpha, 1.f, 8.f, dt);
    float a = easeOut(gWinAlpha);

    ImVec2 center = { (io.DisplaySize.x - Dim::WinW) * 0.5f, (io.DisplaySize.y - Dim::WinH) * 0.5f };
    float wx = center.x, wy = center.y;

    dl->AddRectFilled({ wx + 5, wy + 7 }, { wx + Dim::WinW + 5, wy + Dim::WinH + 7 }, IM_COL32(0, 0, 0, 15 * a), Dim::Round);
    dl->AddRectFilled({ wx, wy }, { wx + Dim::WinW, wy + Dim::WinH }, Theme::WinBg, Dim::Round);
    dl->AddRect({ wx, wy }, { wx + Dim::WinW, wy + Dim::WinH }, Theme::Border, Dim::Round, 0, 1.5f);

    dl->AddRectFilled({ wx, wy }, { wx + Dim::WinW, wy + Dim::TitleH }, Theme::WinBg, Dim::Round, ImDrawFlags_RoundCornersTop);

    if (gFontBold) dl->AddText(gFontBold, 15.f, { wx + Dim::Pad, wy + (Dim::TitleH - 15.f) * 0.5f }, Theme::TextPrimary, "MCHook");

    float sX = wx + 6.f;
    float sY = wy + Dim::TitleH + 10.f;
    float sW = Dim::SidebarW;
    float sH = Dim::WinH - Dim::TitleH - 16.f;

    dl->AddRectFilled({ sX, sY }, { sX + sW, sY + sH }, Theme::SidebarBg, 6.f);

    const char* tabs[] = { "Combat", "Movement", "Player", "Visual", "Misc" };
    float tY = sY + 8.f;

    for (int i = 0; i < 5; i++) {
        float tH = Dim::TabH;
        bool hover = inRect(sX + 4.f, tY, sW - 8.f, tH, mouse);
        bool active = (sSelectedTab == i);

        animTo(gTabSelected[i], active ? 1.f : 0.f, 10.f, dt);

        float bgLerp = gTabSelected[i];
        if (hover && !active) bgLerp = 0.3f;

        ImU32 tBg = lerpC(Theme::SidebarBg, Theme::Accent, bgLerp);
        dl->AddRectFilled({ sX + 4.f, tY }, { sX + sW - 4.f, tY + tH }, tBg, 6.f);

        ImU32 tTxt = active ? Theme::TextWhite : Theme::TextSecondary;
        txt(dl, sX + 10.f, tY + (tH - 11.f) * 0.5f, tTxt, tabs[i], 11.f);

        if (lclick && hover) {
            sSelectedTab = i;
            sSearchActive = false;
            memset(sSearchBuf, 0, sizeof(sSearchBuf));
        }

        tY += tH + 6.f;
    }

    float cX = sX + sW + 10.f;
    float cW = Dim::WinW - sW - Dim::Pad * 2 - 10.f;
    float cY = sY;
    float cH = sH;

    float srY = cY;
    float srH = Dim::SearchH;
    dl->AddRectFilled({ cX, srY }, { cX + cW, srY + srH }, Theme::SearchBg, 6.f);
    dl->AddCircleFilled({ cX + 14.f, srY + srH * 0.5f }, 4.f, Theme::TextSecondary);
    dl->AddLine({ cX + 17.f, srY + srH * 0.5f + 3.f }, { cX + 20.f, srY + srH * 0.5f + 6.f }, Theme::TextSecondary, 1.5f);

    ImGui::PushFont(gFontNormal);
    ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    ImGui::BeginChild("##S", ImVec2(cW - 28.f, srH), false, ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos({ 26.f, (srH - 18.f) * 0.5f });
    ImGui::PushItemWidth(cW - 36.f);

    ImGui::InputText("##search", sSearchBuf, sizeof(sSearchBuf), ImGuiInputTextFlags_None);
    sSearchActive = (sSearchBuf[0] != '\0');

    ImGui::PopItemWidth();
    ImGui::EndChild();
    ImGui::PopFont();

    if (lclick && inRect(cX, srY, cW, srH, mouse) && !ImGui::IsItemActive()) {
        ImGui::SetKeyboardFocusHere(-1);
    }

    float lY = srY + srH + 10.f;
    float lH = cY + cH - lY - 6.f;

    dl->AddRectFilled({ cX, lY }, { cX + cW, lY + lH }, Theme::CardBg, 6.f);
    dl->AddRect({ cX, lY }, { cX + cW, lY + lH }, Theme::Border, 6.f, 0, 1.5f);

    dl->PushClipRect({ cX + 4.f, lY + 4.f }, { cX + cW - 4.f, lY + lH - 4.f }, true);

    auto& mods = gFeatureManager->mModuleManager->mModules;
    float mY = lY + 6.f;
    float mX = cX + 6.f;
    float mW = cW - 12.f;

    for (auto& mod : mods) {
        bool match = true;
        if (sSearchActive) {
            std::string search = sSearchBuf;
            std::string name = mod->mName;
            std::transform(search.begin(), search.end(), search.begin(), ::tolower);
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            match = (name.find(search) != std::string::npos);
        } else {
            match = ((int)mod->mCategory == sSelectedTab);
        }

        if (!match) continue;

        float endY = 0;
        drawModule(dl, mX, mY, mW, mod.get(), mouse, lclick, rclick, dt, endY);
        if (endY <= 0) endY = Dim::ModH;
        mY += endY + 4.f;
    }

    dl->PopClipRect();
}
