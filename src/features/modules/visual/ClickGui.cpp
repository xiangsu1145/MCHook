#include "ClickGui.h"
#include <imgui/imgui.h>
#include "../../FeatureManager.h"
#include "src/hook/WGLHook.h"
#include "include/android/View.h"

inline std::string categoryToString(ModuleCategory cat) {
    switch (cat) {
    case ModuleCategory::Combat:   return "Combat";
    case ModuleCategory::Movement: return "Movement";
    case ModuleCategory::Player:   return "Player";
    case ModuleCategory::Visual:   return "Visual";
    case ModuleCategory::Misc:     return "Misc";
    default:                       return "Unknown";
    }
}

#define M_PI 3.14159265358979323846

#define M_PI_2 1.57079632679489661923


// ─── Palette ──────────────────────────────────────────────────────────────────
namespace C {
    constexpr ImU32 PanelBg = IM_COL32(8, 9, 12, 248);
    constexpr ImU32 HeaderBg = IM_COL32(12, 13, 18, 255);
    constexpr ImU32 ModBgBase = IM_COL32(8, 9, 12, 255);
    constexpr ImU32 ModBgDark = IM_COL32(3, 3, 5, 255);
    constexpr ImU32 SettingBg = IM_COL32(6, 7, 10, 255);
    constexpr ImU32 EnumRowBg = IM_COL32(5, 5, 8, 255);
    constexpr ImU32 Blue = IM_COL32(58, 123, 213, 255);
    constexpr ImU32 BlueDim = IM_COL32(28, 60, 115, 255);
    constexpr ImU32 BlueTrack = IM_COL32(18, 35, 68, 255);
    constexpr ImU32 TextWhite = IM_COL32(230, 233, 242, 255);
    constexpr ImU32 TextGrey = IM_COL32(95, 102, 116, 255);
    constexpr ImU32 TextBlue = IM_COL32(85, 155, 235, 255);
    constexpr ImU32 TextDim = IM_COL32(52, 58, 70, 255);
    constexpr ImU32 Border = IM_COL32(24, 28, 38, 255);
    constexpr ImU32 HeaderLine = IM_COL32(58, 123, 213, 200); // 蓝线，header 和模块之间
    // bubble
    constexpr ImU32 BubbleBg = IM_COL32(4, 4, 6, 235);
    constexpr ImU32 BubbleBdr = IM_COL32(40, 46, 58, 255);
    // switch
    constexpr ImU32 SwitchOff = IM_COL32(28, 32, 44, 255);
    constexpr ImU32 SwitchOn = IM_COL32(58, 123, 213, 255);
}

// ─── Dimensions ───────────────────────────────────────────────────────────────
namespace D {
    constexpr float PanelW = 218.f;
    constexpr float HeaderH = 34.f;
    constexpr float ModH = 32.f;
    constexpr float SetRowH = 46.f;
    constexpr float SwitchH = 28.f;
    constexpr float EnumH = 28.f;
    constexpr float EnumRowH = 22.f;
    constexpr float Round = 5.f;
    constexpr float Pad = 10.f;
    constexpr float SliderH = 3.f;
    constexpr float KnobR = 5.f;
    constexpr float BarW = 2.f;
    // switch widget
    constexpr float SwW = 28.f;
    constexpr float SwH = 14.f;
}

// ─── Animation state ──────────────────────────────────────────────────────────
struct ModAnim {
    float hover = 0.f;
    float enable = 0.f;
    float expand = 0.f;  // open/close 同速
};

struct Panel {
    float x = 0.f, y = 0.f, w = D::PanelW;
    bool  dragging = false;
    float dox = 0.f, doy = 0.f;
    bool  collapsed = false;
    float colAnim = 1.f;
    // +/- 旋转动画：0=+ 45°展开, PI/4=- 展开
    float btnAnim = 1.f;  // 1=expanded(-)  0=collapsed(+)
};

static std::unordered_map<std::string, ModAnim> sModAnim;
static std::unordered_map<std::string, bool>    sExpanded;
static std::unordered_map<std::string, bool>    sEnumOpen;
static std::unordered_map<std::string, Panel>   sPanels;
static bool sInit = false;

// ─── Helpers ──────────────────────────────────────────────────────────────────
static float easeOut(float t) { float i = 1.f - t; return 1.f - i * i * i; }
static float lerpf(float a, float b, float t) { return a + (b - a) * t; }

static ImU32 blendCol(ImU32 a, ImU32 b, float t) {
    ImVec4 fa = ImGui::ColorConvertU32ToFloat4(a);
    ImVec4 fb = ImGui::ColorConvertU32ToFloat4(b);
    return ImGui::ColorConvertFloat4ToU32({
        fa.x + (fb.x - fa.x) * t,fa.y + (fb.y - fa.y) * t,
        fa.z + (fb.z - fa.z) * t,fa.w + (fb.w - fa.w) * t });
}
static ImU32 withA(ImU32 c, float a) {
    ImVec4 v = ImGui::ColorConvertU32ToFloat4(c); v.w = a;
    return ImGui::ColorConvertFloat4ToU32(v);
}
static bool inBox(float x, float y, float w, float h, ImVec2 m) {
    return m.x >= x && m.x <= x + w && m.y >= y && m.y <= y + h;
}
static void animTo(float& v, float tgt, float spd, float dt) {
    v += (tgt - v) * (std::min)(spd * dt, 1.f);
}

static ImFont* gFont = nullptr;

static void T(ImDrawList* dl, float x, float y, ImU32 col, const char* s, float sz = 11.f) {
    dl->AddText(gFont, sz, { x,y }, col, s);
}
static float TW(const char* s, float sz = 11.f) {
    return gFont->CalcTextSizeA(sz, FLT_MAX, 0.f, s).x;
}

// ">" arrow，angle=0 → right，+PI/2 → down
static void Arrow(ImDrawList* dl, float cx, float cy, float angle, ImU32 col, float sz = 5.f) {
    float co = cosf(angle), si = sinf(angle);
    auto R = [&](float lx, float ly)->ImVec2 {
        return { cx + lx * co - ly * si, cy + lx * si + ly * co };
        };
    dl->AddLine(R(-sz * .5f, -sz), R(sz * .5f, 0.f), col, 1.5f);
    dl->AddLine(R(sz * .5f, 0.f), R(-sz * .5f, sz), col, 1.5f);
}

// Switch widget（微信/iOS 风格）
static void Switch(ImDrawList* dl, float cx, float cy, float w, float h, float lerp) {
    float r = h * 0.5f;
    ImU32 track = blendCol(C::SwitchOff, C::SwitchOn, lerp);
    dl->AddRectFilled({ cx,cy }, { cx + w,cy + h }, track, r);
    float knobX = lerpf(cx + r, cx + w - r, lerp);
    dl->AddCircleFilled({ knobX,cy + r }, r - 2.f, C::TextWhite);
}

// ─── Forward declarations ──────────────────────────────────────────────────────
static float settingsHeight(Module* mod);
static void  drawModule(ImDrawList* dl, float x, float& y, float w,
    Module* mod, ImVec2 mouse, float dt,
    bool lclick, bool rclick, bool ldown);
static void  drawSettings(ImDrawList* dl, float x, float& y, float w,
    Module* mod, ImVec2 mouse, bool lclick, bool ldown);

// ─── API ──────────────────────────────────────────────────────────────────────
void ClickGui::onEnable() {
    gFeatureManager->mDispatcher->listen<RenderEvent, &ClickGui::onRenderEvent>(this);
    gFeatureManager->mDispatcher->listen<KeyEvent, &ClickGui::onKeyEvent>(this);
    gFeatureManager->mDispatcher->listen<MouseEvent, &ClickGui::onMouseEvent>(this);
}
void ClickGui::onDisable() {
    gFeatureManager->mDispatcher->deafen<RenderEvent, &ClickGui::onRenderEvent>(this);
    gFeatureManager->mDispatcher->deafen<KeyEvent, &ClickGui::onKeyEvent>(this);
    gFeatureManager->mDispatcher->deafen<MouseEvent, &ClickGui::onMouseEvent>(this);
}
void ClickGui::onKeyEvent(KeyEvent&) {}
void ClickGui::onMouseEvent(MouseEvent&) {}

// ─── Main render ──────────────────────────────────────────────────────────────
//void ClickGui::onRenderEvent(RenderEvent&) {
//    auto  io = ImGui::GetIO();
//    auto* dl = ImGui::GetBackgroundDrawList();
//    gFont = ImGui::GetFont();
//    ImVec2 mouse = io.MousePos;
//    float  dt = io.DeltaTime;
//    bool lclick = io.MouseClicked[0];
//    bool rclick = io.MouseClicked[1];
//    bool ldown = io.MouseDown[0];
//
//    dl->AddRectFilled({ 0,0 }, io.DisplaySize, IM_COL32(0, 0, 0, 115));
//
//    auto& mods = gFeatureManager->mModuleManager->mModules;
//    std::map<ModuleCategory, std::vector<std::shared_ptr<Module>>> byCategory;
//    for (auto& m : mods) byCategory[m->mCategory].push_back(m);
//
//    if (!sInit) {
//        float px = 30.f;
//        for (int ci = 0; ci < 5; ci++) {
//            std::string key = categoryToString(static_cast<ModuleCategory>(ci));
//            if (sPanels.find(key) == sPanels.end()) {
//                sPanels[key] = { px,50.f,D::PanelW };
//                px += D::PanelW + 14.f;
//            }
//        }
//        sInit = true;
//    }
//
//    // ── 修复多卡片拖动：找出当前正在拖的那个 ──
//    // 先判断本帧是否有面板已经在拖
//    bool anyDragging = false;
//    for (auto& [k, p] : sPanels) if (p.dragging) { anyDragging = true; break; }
//
//    if (!ldown)
//        for (auto& [k, p] : sPanels) p.dragging = false;
//
//    for (int ci = 0; ci < 5; ci++) {
//        ModuleCategory mc = static_cast<ModuleCategory>(ci);
//        std::string    cat = categoryToString(mc);
//        auto& panel = sPanels[cat];
//        auto& catMods = byCategory[mc];
//
//        // drag：只有当前帧没有其他面板在拖，才允许新起拖
//        bool onHdr = inBox(panel.x, panel.y, panel.w, D::HeaderH, mouse);
//        if (ldown && onHdr && !anyDragging && !panel.dragging) {
//            panel.dragging = true;
//            panel.dox = mouse.x - panel.x;
//            panel.doy = mouse.y - panel.y;
//            anyDragging = true;
//        }
//        if (panel.dragging && ldown) {
//            panel.x = mouse.x - panel.dox;
//            panel.y = mouse.y - panel.doy;
//        }
//
//        animTo(panel.colAnim, panel.collapsed ? 0.f : 1.f, 7.f, dt);
//        animTo(panel.btnAnim, panel.collapsed ? 0.f : 1.f, 7.f, dt);
//
//        float contentH = 0.f;
//        for (auto& mod : catMods) {
//            contentH += D::ModH;
//            float ea = easeOut(sModAnim[mod->mName].expand);
//            if (ea > 0.001f) contentH += settingsHeight(mod.get()) * ea;
//        }
//        float visH = lerpf(0.f, contentH + 4.f, easeOut(panel.colAnim));
//
//        float px = panel.x, py = panel.y, pw = panel.w;
//        float totalH = D::HeaderH + visH;
//
//        // 面板背景 + 边框
//        dl->AddRectFilled({ px,py }, { px + pw,py + totalH }, C::PanelBg, D::Round);
//        dl->AddRect({ px,py }, { px + pw,py + totalH }, C::Border, D::Round, 0, 1.f);
//
//        // header 背景
//        ImDrawFlags hdrFlags = (panel.colAnim < 0.02f)
//            ? ImDrawFlags_RoundCornersAll : ImDrawFlags_RoundCornersTop;
//        dl->AddRectFilled({ px,py }, { px + pw,py + D::HeaderH }, C::HeaderBg, D::Round, hdrFlags);
//
//        // header 和模块之间的蓝线（展开时才画）
//        if (panel.colAnim > 0.02f)
//            dl->AddLine({ px,py + D::HeaderH }, { px + pw,py + D::HeaderH }, C::HeaderLine, 1.5f);
//
//        // 分类名
//        std::string upper = cat;
//        for (auto& c : upper) c = (char)toupper(c);
//        T(dl, px + D::Pad, py + (D::HeaderH - 11.f) * 0.5f, C::TextWhite, upper.c_str(), 11.f);
//
//        // –/+ 按钮
//        // 逻辑：btnAnim=1 → 展开 → 显示 "-"（水平线）
//        //        btnAnim=0 → 折叠 → 显示 "+"（旋转 90° 补上竖线）
//        // 实现：始终画水平线；竖线的 alpha = 1 - btnAnim（折叠时出现）
//        // 整体再旋转 45°*btnAnim，让切换有转动感
//        {
//            float bx = px + pw - 18.f, by_ = py + D::HeaderH * 0.5f;
//            float hs = 5.f;
//            // 旋转角：展开=0，折叠=PI/4（转45°让+/-切换更生动）
//            float ang = lerpf((float)M_PI * 0.25f, 0.f, easeOut(panel.btnAnim));
//            float co = cosf(ang), si = sinf(ang);
//            auto RV = [&](float lx, float ly)->ImVec2 {
//                return{ bx + lx * co - ly * si, by_ + lx * si + ly * co };
//                };
//            // 展开时亮，收起时稍亮，hover 不额外变色
//            ImU32 btnCol = blendCol(C::TextDim, C::TextGrey, easeOut(panel.btnAnim));
//            // 水平线（始终存在）
//            dl->AddLine(RV(-hs, 0.f), RV(hs, 0.f), btnCol, 1.5f);
//            // 竖线（折叠时出现，展开时消失）
//            float vAlpha = 1.f - easeOut(panel.btnAnim);
//            if (vAlpha > 0.01f)
//                dl->AddLine(RV(0.f, -hs), RV(0.f, hs), withA(btnCol, vAlpha), 1.5f);
//
//            if (lclick && inBox(bx - 8.f, py + 4.f, 16.f, D::HeaderH - 8.f, mouse))
//                panel.collapsed = !panel.collapsed;
//        }
//
//        // 模块列表
//        if (visH > 0.5f) {
//            dl->PushClipRect({ px,py + D::HeaderH }, { px + pw,py + D::HeaderH + visH }, true);
//            float my = py + D::HeaderH + 2.f;
//            for (auto& mod : catMods)
//                drawModule(dl, px, my, pw, mod.get(), mouse, dt, lclick, rclick, ldown);
//            dl->PopClipRect();
//        }
//    }
//}

// ─── Settings height ──────────────────────────────────────────────────────────
static float settingsHeight(Module* mod) {
    float h = 0.f;
    bool any = false;
    for (auto* s : mod->mSettings) {
        if (!s->mIsVisible()) continue;
        any = true;
        if (s->mType == SettingType::Bool)     h += D::SwitchH;
        else if (s->mType == SettingType::Enum) {
            h += D::EnumH;
            std::string key = mod->mName + "::" + s->mName;
            if (sEnumOpen.count(key) && sEnumOpen[key]) {
                auto* es = static_cast<EnumSettingT<int>*>(s);
                h += D::EnumRowH * (float)es->mValues.size();
            }
        }
        else h += D::SetRowH;
    }
    if (any) h += 6.f;
    return h;
}

// ─── Module row ───────────────────────────────────────────────────────────────
static void drawModule(ImDrawList* dl, float x, float& y, float w,
    Module* mod, ImVec2 mouse, float dt,
    bool lclick, bool rclick, bool ldown) {
    float h = D::ModH;
    auto& anim = sModAnim[mod->mName];
    bool& expanded = sExpanded[mod->mName];

    bool hov = inBox(x, y, w, h, mouse);
    animTo(anim.hover, hov ? 1.f : 0.f, 12.f, dt);
    animTo(anim.enable, mod->mEnabled ? 1.f : 0.f, 8.f, dt);
    // 展开/关闭同速 = 7
    animTo(anim.expand, expanded ? 1.f : 0.f, 7.f, dt);

    // 背景：hover 变更黑
    ImU32 bg = blendCol(C::ModBgBase, C::ModBgDark, easeOut(anim.hover));
    dl->AddRectFilled({ x,y }, { x + w,y + h }, bg);
    // 不画模块间分隔线

    // 模块名：灰 → 蓝（enabled），hover 时 → 白
    ImU32 nameCol = blendCol(
        blendCol(C::TextGrey, C::TextWhite, easeOut(anim.hover)),
        C::TextBlue, easeOut(anim.enable));
    T(dl, x + D::Pad, y + (h - 11.f) * 0.5f, nameCol, mod->mName.c_str(), 11.f);

    // ">" 箭头，右边，旋转 90° on expand，不可点
    float arrowAngle = lerpf(0.f, (float)M_PI * 0.5f, easeOut(anim.expand));
    ImU32 arrowCol = blendCol(C::TextDim, C::TextWhite, easeOut(anim.hover));
    float arrowX = x + w - 13.f;
    Arrow(dl, arrowX, y + h * 0.5f, arrowAngle, arrowCol, 4.5f);

    // 描述 bubble：在面板右侧弹出（不遮住面板内容）
    // 微信消息气泡风格：黑底白字，左下有个小尖角
    if (anim.hover > 0.02f && !mod->mDescription.empty()) {
        float alpha = easeOut(anim.hover);
        const char* desc = mod->mDescription.c_str();
        float bw = TW(desc, 10.f) + 14.f;
        float bh = 22.f;
        float bx = x + w + 6.f;             // 面板右侧
        float bby = y + (h - bh) * 0.5f;
        float br = 4.f;

        // 气泡主体
        dl->AddRectFilled({ bx,bby }, { bx + bw,bby + bh }, withA(C::BubbleBg, alpha), br);
        dl->AddRect({ bx,bby }, { bx + bw,bby + bh }, withA(C::BubbleBdr, alpha), br, 0, .8f);
        // 左侧小尖角（三角形，指向面板方向）
        float ty = bby + bh * 0.5f;
        dl->AddTriangleFilled(
            { bx - 5.f,ty },
            { bx,ty - 4.f },
            { bx,ty + 4.f },
            withA(C::BubbleBg, alpha));
        // 边框三角
        dl->AddLine({ bx - 5.f,ty }, { bx,ty - 4.f }, withA(C::BubbleBdr, alpha), .8f);
        dl->AddLine({ bx - 5.f,ty }, { bx,ty + 4.f }, withA(C::BubbleBdr, alpha), .8f);

        T(dl, bx + 7.f, bby + (bh - 10.f) * 0.5f, withA(C::TextWhite, alpha), desc, 10.f);
    }

    // 左键：toggle 启用
    if (lclick && hov) {
        // 如果 Module 有 toggle()：mod->toggle();
        // 如果只有 setEnabled：
        mod->setEnabled(!mod->mEnabled);
    }
    // 右键：展开/折叠（整行都响应）
    if (rclick && hov) expanded = !expanded;

    y += h;

    // Settings 区域（高度动画）
    float ea = easeOut(anim.expand);
    if (ea > 0.001f) {
        float fullH = settingsHeight(mod);
        float clipH = fullH * ea;

        // 蓝色左竖线：贯穿整个 settings 块
        dl->AddRectFilled({ x,y }, { x + D::BarW,y + clipH }, C::Blue);

        dl->PushClipRect({ x,y }, { x + w,y + clipH }, true);
        float sy = y;
        drawSettings(dl, x, sy, w, mod, mouse, lclick, ldown);
        dl->PopClipRect();

        y += clipH;
    }
}

// ─── Settings ─────────────────────────────────────────────────────────────────
static void drawSettings(ImDrawList* dl, float x, float& y, float w,
    Module* mod, ImVec2 mouse, bool lclick, bool ldown) {
    float ix = x + D::BarW + 6.f;
    float iw = w - D::BarW - 6.f;

    for (auto* s : mod->mSettings) {
        if (!s->mIsVisible()) continue;

        // ── Switch (Bool) ────────────────────────────────────────────────
        if (s->mType == SettingType::Bool) {
            auto* bs = static_cast<BoolSetting*>(s);
            float h = D::SwitchH;
            dl->AddRectFilled({ x,y }, { x + w,y + h }, C::SettingBg);

            // switch 在右侧
            float swX = ix + iw - D::Pad - D::SwW;
            float swY = y + (h - D::SwH) * 0.5f;
            float lerp = bs->mValue ? 1.f : 0.f;  // 如需动画可存 per-setting float
            Switch(dl, swX, swY, D::SwW, D::SwH, lerp);

            T(dl, ix + D::Pad, y + (h - 10.f) * 0.5f, C::TextGrey, s->mName.c_str(), 10.f);

            if (lclick && inBox(x, y, w, h, mouse)) bs->mValue = !bs->mValue;
            y += h;
        }

        // ── Number ───────────────────────────────────────────────────────
        else if (s->mType == SettingType::Number) {
            auto* ns = static_cast<NumberSetting*>(s);
            float h = D::SetRowH;
            dl->AddRectFilled({ x,y }, { x + w,y + h }, C::SettingBg);

            // 上行：名字（左）+ 数值（右，白色）
            char vb[16]; snprintf(vb, sizeof(vb), "%.2f", ns->mValue);
            float nameY = y + 8.f;
            T(dl, ix + D::Pad, nameY, C::TextGrey, s->mName.c_str(), 10.f);
            T(dl, ix + iw - D::Pad - TW(vb, 10.f), nameY, C::TextWhite, vb, 10.f);  // 白色

            // 下行：拖动条
            float trX = ix + D::Pad;
            float trW = iw - D::Pad * 2.f;
            float trY = y + h - 15.f;

            dl->AddRectFilled({ trX,trY }, { trX + trW,trY + D::SliderH }, C::BlueTrack, D::SliderH);
            float t = std::clamp((ns->mValue - ns->mMin) / (ns->mMax - ns->mMin), 0.f, 1.f);
            float fw = trW * t;
            if (fw > 0.f)
                dl->AddRectFilled({ trX,trY }, { trX + fw,trY + D::SliderH }, C::Blue, D::SliderH);
            dl->AddCircleFilled({ trX + fw,trY + D::SliderH * .5f }, D::KnobR, C::Blue);
            dl->AddCircleFilled({ trX + fw,trY + D::SliderH * .5f }, D::KnobR - 2.f, C::TextWhite);

            if (ldown && inBox(trX - 5.f, trY - 10.f, trW + 10.f, D::SliderH + 20.f, mouse)) {
                float nt = std::clamp((mouse.x - trX) / trW, 0.f, 1.f);
                ns->setValue(ns->mMin + nt * (ns->mMax - ns->mMin));
            }
            y += h;
        }

        // ── Range ────────────────────────────────────────────────────────
        else if (s->mType == SettingType::Range) {
            auto* rs = static_cast<RangeSetting*>(s);
            float h = D::SetRowH;
            dl->AddRectFilled({ x,y }, { x + w,y + h }, C::SettingBg);

            char vb[32]; snprintf(vb, sizeof(vb), "%.1f – %.1f", rs->mValueMin, rs->mValueMax);
            float nameY = y + 8.f;
            T(dl, ix + D::Pad, nameY, C::TextGrey, s->mName.c_str(), 10.f);
            T(dl, ix + iw - D::Pad - TW(vb, 10.f), nameY, C::TextWhite, vb, 10.f);  // 白色

            float trX = ix + D::Pad, trW = iw - D::Pad * 2.f, trY = y + h - 15.f;
            dl->AddRectFilled({ trX,trY }, { trX + trW,trY + D::SliderH }, C::BlueTrack, D::SliderH);
            float tMin = std::clamp((rs->mValueMin - rs->mMin) / (rs->mMax - rs->mMin), 0.f, 1.f);
            float tMax = std::clamp((rs->mValueMax - rs->mMin) / (rs->mMax - rs->mMin), 0.f, 1.f);
            dl->AddRectFilled({ trX + trW * tMin,trY }, { trX + trW * tMax,trY + D::SliderH }, C::Blue);
            dl->AddCircleFilled({ trX + trW * tMin,trY + D::SliderH * .5f }, D::KnobR, C::Blue);
            dl->AddCircleFilled({ trX + trW * tMin,trY + D::SliderH * .5f }, D::KnobR - 2.f, C::TextWhite);
            dl->AddCircleFilled({ trX + trW * tMax,trY + D::SliderH * .5f }, D::KnobR, C::BlueDim);
            dl->AddCircleFilled({ trX + trW * tMax,trY + D::SliderH * .5f }, D::KnobR - 2.f, C::TextWhite);

            if (ldown && inBox(trX - 5.f, trY - 10.f, trW + 10.f, D::SliderH + 20.f, mouse)) {
                float nt = std::clamp((mouse.x - trX) / trW, 0.f, 1.f);
                float nv = rs->mMin + nt * (rs->mMax - rs->mMin);
                float mid = (rs->mValueMin + rs->mValueMax) * 0.5f;
                if (nv < mid) rs->setValue(nv, rs->mValueMax);
                else        rs->setValue(rs->mValueMin, nv);
            }
            y += h;
        }

        // ── Enum ─────────────────────────────────────────────────────────
        else if (s->mType == SettingType::Enum) {
            auto* es = static_cast<EnumSettingT<int>*>(s);
            std::string key = mod->mName + "::" + s->mName;
            bool& open = sEnumOpen[key];
            float h = D::EnumH;
            bool hov = inBox(x, y, w, h, mouse);

            dl->AddRectFilled({ x,y }, { x + w,y + h }, C::SettingBg);
            T(dl, ix + D::Pad, y + (h - 10.f) * 0.5f, C::TextGrey, s->mName.c_str(), 10.f);

            const std::string& cur = es->mValues[(int)es->mValue];
            float cw = TW(cur.c_str(), 10.f);
            T(dl, ix + iw - D::Pad - cw - 14.f, y + (h - 10.f) * 0.5f, C::TextBlue, cur.c_str(), 10.f);
            Arrow(dl, ix + iw - D::Pad - 5.f, y + h * .5f,
                open ? (float)M_PI * .5f : 0.f, C::TextDim, 3.5f);

            if (lclick && hov) open = !open;
            y += h;

            if (open) {
                for (int i = 0; i < (int)es->mValues.size(); i++) {
                    bool isCur = (i == (int)es->mValue);
                    bool rHov = inBox(x, y, w, D::EnumRowH, mouse);
                    dl->AddRectFilled({ x,y }, { x + w,y + D::EnumRowH },
                        rHov ? C::ModBgDark : C::EnumRowBg);
                    if (isCur)
                        dl->AddCircleFilled({ ix + D::Pad + 4.f,y + D::EnumRowH * .5f }, 3.f, C::Blue);
                    T(dl, ix + D::Pad + 12.f, y + (D::EnumRowH - 10.f) * 0.5f,
                        isCur ? C::TextBlue : C::TextDim,
                        es->mValues[i].c_str(), 10.f);
                    if (lclick && rHov) {
                        es->mValue = (decltype(es->mValue))i;
                        open = false;
                    }
                    y += D::EnumRowH;
                }
            }
        }
    }
    y += 4.f;
}


void ClickGui::onRenderEvent(RenderEvent&) {
    // 设置ImGui样式 - 白色配色 + 大圆角
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
    style.ChildRounding = 8.0f;

    // 白色/浅色主题
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.95f, 0.95f, 0.97f, 0.98f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.85f, 0.85f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.75f, 0.75f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.80f, 0.80f, 0.85f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.70f, 0.70f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.65f, 0.65f, 0.70f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.85f, 0.85f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.70f, 0.70f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.95f, 0.95f, 1.00f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.90f, 0.90f, 0.95f, 1.0f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.30f, 0.60f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.30f, 0.60f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.20f, 0.50f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_Text] = ImVec4(0.10f, 0.10f, 0.15f, 1.0f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.0f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.85f, 0.85f, 0.90f, 1.0f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.75f, 0.75f, 0.80f, 1.0f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.95f, 0.95f, 1.00f, 1.0f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.70f, 0.70f, 0.75f, 1.0f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.20f, 0.20f, 0.25f, 0.95f);

    ImGui::Begin("ClickGui", nullptr, ImGuiWindowFlags_NoCollapse);

    if (!gFeatureManager || !gFeatureManager->mModuleManager) {
        ImGui::Text("FeatureManager not initialized");
        ImGui::End();
        return;
    }

    auto& modules = gFeatureManager->mModuleManager->mModules;
    std::map<ModuleCategory, std::vector<std::shared_ptr<Module>>> categorized;
    for (const auto& mod : modules) {
        categorized[mod->mCategory].push_back(mod);
    }

    struct TabInfo {
        ModuleCategory category;
        const char* name;
    };
    std::vector<TabInfo> tabs = {
        {ModuleCategory::Combat,   "Combat"},
        {ModuleCategory::Movement, "Movement"},
        {ModuleCategory::Player,   "Player"},
        {ModuleCategory::Visual,   "Visual"},
        {ModuleCategory::Misc,     "Misc"}
    };

    static int currentTab = 0;

    ImGui::BeginGroup();
    for (int i = 0; i < tabs.size(); i++) {
        if (i > 0) ImGui::SameLine();

        if (currentTab == i) {
            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.30f, 0.60f, 0.90f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.60f, 0.90f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.30f, 0.60f, 0.90f, 1.0f));
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            if (ImGui::Button(tabs[i].name, ImVec2(80, 30))) {
                currentTab = i;
            }
            ImGui::PopStyleColor(4);
        }
        else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.10f, 0.10f, 0.15f, 1.0f));
            if (ImGui::Button(tabs[i].name, ImVec2(80, 30))) {
                currentTab = i;
            }
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndGroup();
    ImGui::Separator();
    ImGui::Spacing();

    ModuleCategory currentCategory = tabs[currentTab].category;
    auto it = categorized.find(currentCategory);
    if (it != categorized.end()) {
        auto& modList = it->second;

        ImGui::BeginChild("ModuleList", ImVec2(0, 0), true);

        for (auto& mod : modList) {
            ImGui::PushID(mod.get());

            bool enabled = mod->mEnabled;
            if (ImGui::Checkbox(("##enabled_" + mod->mName).c_str(), &enabled)) {
                mod->setEnabled(enabled);
            }
            ImGui::SameLine();

            bool open = ImGui::CollapsingHeader(mod->mName.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

            ImGui::SameLine();
            ImGui::TextDisabled("(Key: %d)", mod->mKey);

            if (open) {
                ImGui::Indent(20.0f);

                if (!mod->mDescription.empty()) {
                    ImGui::TextWrapped("%s", mod->mDescription.c_str());
                    ImGui::Spacing();
                }

                for (Setting* setting : mod->mSettings) {
                    if (!setting || !setting->mIsVisible())
                        continue;

                    ImGui::PushID(setting);
                    std::string label = setting->mName;

                    switch (setting->mType) {
                    case SettingType::Bool: {
                        auto* boolSet = static_cast<BoolSetting*>(setting);
                        ImGui::Checkbox(label.c_str(), &boolSet->mValue);
                        break;
                    }
                    case SettingType::Number: {
                        auto* numSet = static_cast<NumberSetting*>(setting);
                        float value = numSet->mValue;
                        if (ImGui::SliderFloat(label.c_str(), &value, numSet->mMin, numSet->mMax, "%.3f")) {
                            value = std::round(value / numSet->mStep) * numSet->mStep;
                            numSet->setValue(value);
                        }
                        break;
                    }
                    case SettingType::Range: {
                        auto* rangeSet = static_cast<RangeSetting*>(setting);

                        ImGui::Text("%s", label.c_str());
                        ImGui::Indent(10.0f);

                        float minVal = rangeSet->mValueMin;
                        ImGui::PushID("min");
                        ImGui::Text("Min:");
                        ImGui::SameLine(50.0f);
                        if (ImGui::SliderFloat("##min_slider", &minVal, rangeSet->mMin, rangeSet->mMax, "%.3f")) {
                            minVal = std::round(minVal / rangeSet->mStep) * rangeSet->mStep;
                            if (minVal > rangeSet->mValueMax) {
                                minVal = rangeSet->mValueMax;
                            }
                            rangeSet->mValueMin = minVal;
                        }
                        ImGui::PopID();

                        float maxVal = rangeSet->mValueMax;
                        ImGui::PushID("max");
                        ImGui::Text("Max:");
                        ImGui::SameLine(50.0f);
                        if (ImGui::SliderFloat("##max_slider", &maxVal, rangeSet->mMin, rangeSet->mMax, "%.3f")) {
                            maxVal = std::round(maxVal / rangeSet->mStep) * rangeSet->mStep;
                            if (maxVal < rangeSet->mValueMin) {
                                maxVal = rangeSet->mValueMin;
                            }
                            rangeSet->mValueMax = maxVal;
                        }
                        ImGui::PopID();

                        ImGui::Unindent(10.0f);
                        break;
                    }
                    case SettingType::Enum: {
                        int currentIndex = setting->getEnumIndex();
                        const auto* values = setting->getEnumValues();

                        if (values && !values->empty() && currentIndex >= 0) {
                            if (ImGui::BeginCombo(label.c_str(), (*values)[currentIndex].c_str())) {
                                for (int i = 0; i < values->size(); i++) {
                                    if (ImGui::Selectable((*values)[i].c_str(), currentIndex == i)) {
                                        setting->setEnumIndex(i);
                                    }
                                }
                                ImGui::EndCombo();
                            }
                        }
                        break;
                    }
                    case SettingType::String: {
                        // 跳过
                        break;
                    }
                    }

                    // 鼠标悬停时显示描述信息
                    if (!setting->mDescription.empty() && ImGui::IsItemHovered()) {
                        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.15f, 0.15f, 0.20f, 0.95f));
                        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                        ImGui::SetTooltip("%s", setting->mDescription.c_str());
                        ImGui::PopStyleColor(2);
                    }
                    ImGui::PopID();
                    ImGui::Spacing();
                }

                ImGui::Unindent(20.0f);
            }
            ImGui::PopID();
            ImGui::Separator();
        }

        ImGui::EndChild();
    }

    ImGui::End();
}