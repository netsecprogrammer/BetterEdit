# BetterEdit v5 Migration Plan

## Goal
Port BetterEdit to support **Geometry Dash 2.2081** and **Geode v5.0.0-alpha.1**.

**Fork:** https://github.com/netsecprogrammer/BetterEdit
**Branch:** `v5-compat`
**Local path:** `C:\Users\steven\Documents\geode\BetterEdit`
**Date:** February 5, 2026

---

## Phase 1: Build Configuration (Must Do First)

### 1.1 Update CMakeLists.txt
- [ ] Change `CMAKE_CXX_STANDARD` from `20` to `23` (line 2)

### 1.2 Update mod.json
- [ ] Change `"geode"` from `"4.6.2"` to `"5.0.0-alpha.1"` (line 2)
- [ ] Change all `"gd"` entries from `"2.2074"` to `"2.2081"` (lines 4-8)
- [ ] Update `geode.node-ids` version to `"1.22.0-beta.1"` (line 20)
- [ ] Update `cvolton.level-id-api` version to `"2.0.0-beta.1"` (line 31)
- [ ] Handle unavailable dependencies (see Phase 6)

---

## Phase 2: Popup De-templating (13 classes)

Geode v5 `Popup` is no longer templated. The `setup(Args...)` virtual method is gone.
Instead, subclasses override `init()` directly and call `Popup::init(width, height, bg, bgRect)`.

**New pattern (from MDPopup in Geode v5):**
```cpp
// OLD v4:
class MyPopup : public Popup<EditorUI*> {
    bool setup(EditorUI* ui) override { ... }
    static MyPopup* create(EditorUI* ui) {
        auto ret = new MyPopup();
        if (ret->initAnchored(380, 290, ui)) { ... }
    }
};

// NEW v5:
class MyPopup : public Popup {
    bool init(EditorUI* ui) {
        if (!Popup::init(380, 290)) return false;
        // setup content here (what was in setup())
        return true;
    }
    static MyPopup* create(EditorUI* ui) {
        auto ret = new MyPopup();
        if (ret->init(ui)) { ret->autorelease(); return ret; }
        delete ret; return nullptr;
    }
};
```

### 2.1 Remove PopupWithCorners utility
- [ ] **`src/utils/PopupWithCorners.hpp`** — `PopupWithCorners<Args...>` wraps `Popup<Args...>`
  - De-template this class, make it inherit from `Popup` directly
  - Move corner sprite creation into the init method

### 2.2 Migrate Popup<> subclasses (no template args)
These are simpler — just remove `<>` and rename `setup()` to use `init()`:

- [ ] `src/features/supporters/ActivateLicensePopup.hpp:9` — `ActivateLicensePopup`
- [ ] `src/features/supporters/MySupportPopup.cpp:15` — `ActivateNewDevicePopup`
- [ ] `src/features/supporters/SupportersPopup.hpp:11` — `SupportersPopup`
- [ ] `src/features/scripting/ScriptingUI.hpp:39` — `RunScriptPopup`

### 2.3 Migrate Popup<Args...> subclasses (with template args)
These need the args moved from template to init() parameters:

- [ ] `src/features/supporters/ActivateLicensePopup.cpp:11` — `ActivatingLicensePopup : Popup<ActivateLicensePopup*, std::string const&>`
- [ ] `src/features/GroupSummaryPopup.hpp:38` — `ObjectsListPopup : Popup<std::string const&, GroupSummaryPopup*, CCArray*, bool>`
- [ ] `src/features/GroupSummaryPopup.hpp:51` — `GroupSummaryPopup : Popup<EditorUI*>`

### 2.4 Migrate PopupWithCorners<> subclasses
- [ ] `src/features/about/AboutBEPopup.hpp:8` — `AboutBEPopup : PopupWithCorners<>`
- [ ] `src/features/about/AboutBEPopup.hpp:27` — `SpecialThanksPopup : PopupWithCorners<>`
- [ ] `src/features/about/ChangelogPopup.hpp:8` — `ChangelogPopup : PopupWithCorners<std::optional<VersionInfo> const&>`
- [ ] `src/features/backups/BackupListPopup.hpp:10` — `BackupListPopup : PopupWithCorners<GJGameLevel*>`
- [ ] `src/features/supporters/MySupportPopup.hpp:10` — `MySupportPopup : PopupWithCorners<>`

### 2.5 Update all initAnchored calls
Replace `initAnchored(w, h, args..., bg)` with `init(args...)` where `init()` calls `Popup::init(w, h, bg)`:

- [ ] `about/ChangelogPopup.cpp:77`
- [ ] `about/AboutBEPopup.cpp:302, 343`
- [ ] `backups/BackupListPopup.cpp:91`
- [ ] `GroupSummaryPopup.cpp:99, 492`
- [ ] `scripting/ScriptingUI.cpp:279`
- [ ] `supporters/ActivateLicensePopup.cpp:79, 255`
- [ ] `supporters/MySupportPopup.cpp:108, 494`
- [ ] `supporters/SupportersPopup.cpp:204`

---

## Phase 3: Removed APIs

### 3.1 `Mod::getMetadataRef()` → `Mod::getMetadata()`
- [ ] `src/features/about/ChangelogPopup.cpp:6` — change `getMetadataRef()` to `getMetadata()`

---

## Phase 4: `std::function` → `geode::Function` / `geode::FunctionRef`

Geode v5 introduces:
- `geode::Function<Sig>` = `std::move_only_function<Sig>` (move-only, preferred)
- `geode::CopyableFunction<Sig>` = copyable version (replaces `std::function`)
- `geode::FunctionRef<Sig>` = non-owning reference

**Rule of thumb:** Replace `std::function` with `geode::CopyableFunction` for stored callbacks, or `geode::Function` if move-only is acceptable.

### Files to update (35+ occurrences):
- [ ] `src/utils/BEMenuItemToggler.hpp:7-8` — member fields
- [ ] `src/utils/EditableBMLabelProxy.hpp:14-15, 25-26` — members + constructor params
- [ ] `src/utils/EditableBMLabelProxy.cpp:18-19` — function params
- [ ] `src/utils/Editor.hpp:24, 101` — `createViewOnlyEditor`, `UIShowFilter::handle`
- [ ] `src/utils/Editor.cpp:72, 75, 87, 242` — `ViewOnlyModeData`, filter handle
- [ ] `src/features/HSVPreview.cpp:35, 51` — `srcChannel` member
- [ ] `src/features/ImprovedScaleAndRotate.cpp:36, 38, 71` — callbacks
- [ ] `src/features/Keybinds.cpp:187` — defineKeybind callback
- [ ] `src/features/ViewTab/ViewTab.cpp:93, 106` — view toggle callbacks
- [ ] `src/features/scripting/QJS.hpp:49, 107, 161, 590` — scripting wrappers
- [ ] `src/features/scripting/QJS.cpp:8-9, 19, 36, 94, 242, 458` — scripting internals
- [ ] `src/features/scripting/Scripting.hpp:76` — `JsScriptLoggedFilter::handle`
- [ ] `src/features/scripting/Scripting.cpp:445` — filter handle

---

## Phase 5: Event System (V1 → V2)

Geode v5 redesigns events with a new "Ports" system. The `Event` base class still exists but the template signature has changed. Custom events need to be updated.

**New Event pattern:**
```cpp
// OLD v4:
class MyEvent : public Event {
    // ...
};
class MyFilter : public EventFilter<MyEvent> {
    ListenerResult handle(std::function<Callback> fn, MyEvent* event) override;
};

// NEW v5:
class MyEvent final : public ::geode::Event<MyEvent, ReturnType(ArgTypes...), FilterArgs...> {
    using Event::Event;
};
```

### Events to migrate:
- [ ] `src/utils/Editor.hpp:82` — `EditorExitEvent`
- [ ] `src/utils/Editor.hpp:85-91` — `UIShowEvent` + `UIShowFilter`
- [ ] `src/utils/Editor.hpp:103` — `using OnUIHide = EventListener<UIShowFilter>`
- [ ] `src/features/backups/BackupItem.hpp:8` — `UpdateBackupListEvent`
- [ ] `src/features/backups/BackupListPopup.hpp:15` — `EventListener<EventFilter<UpdateBackupListEvent>>`
- [ ] `src/features/scripting/Scripting.hpp:61-76` — `JsScriptLoggedEvent` + `JsScriptLoggedFilter`

**Note:** Need to study Geode v5 event examples more closely to determine exact migration pattern. Check `loader/include/Geode/loader/Event.hpp` for the new API.

---

## Phase 6: Dependency Management

### Available for v5 — update versions:
- [ ] `geode.node-ids` → `"1.22.0-beta.1"`
- [ ] `cvolton.level-id-api` → `"2.0.0-beta.1"` (major version bump — check for API changes)

### NOT available for v5 — make optional or stub:
- [ ] `geode.custom-keybinds` — **Required, NOT ported**
  - Used in: `src/features/Keybinds.cpp`
  - Strategy: Wrap keybind code in `#ifdef` guards or make dependency optional in mod.json
- [ ] `hjfod.gmd-api` — **Required, NOT ported**
  - Used in: backup features
  - Strategy: Make optional, disable backup import/export features
- [ ] `hjfod.trashcan` — Recommended, not ported → make suggested or remove
- [ ] `hjfod.backups` — Suggested, not ported → remove temporarily
- [ ] `alphalaneous.awesome_modifier_icons` — Suggested, not ported → remove
- [ ] `alphalaneous.improvedlink` — Suggested, not ported → remove
- [ ] `alphalaneous.improved_group_view` — Suggested, not ported → remove

---

## Phase 7: WebRequest API Rewrite

The web request API has been completely rewritten in v5. The old `web::WebRequest` / `web::WebResponse` / `web::WebProgress` pattern with `.send().map()` is gone.

### Files affected:
- [ ] `src/server/Server.cpp` — **Entire file** (10+ request constructions, ~150 lines of web code)
- [ ] `src/server/Server.hpp` — `ServerRequest<T>` type alias using old `Task<Result<T>, uint8_t>`
- [ ] `src/features/about/AboutBEPopup.cpp` — `#include <Geode/utils/web.hpp>`, `web::openLinkInBrowser` calls
- [ ] `src/features/supporters/Pro.cpp` — `#include <Geode/utils/web.hpp>`

**Strategy:** Study Geode v5's new web API (likely uses `arc` async runtime), then rewrite Server.cpp. The `web::openLinkInBrowser` calls may still work.

---

## Phase 8: GD 2.2081 Hook Verification

All 24+ `$modify` hooks must be verified against 2.2081 bindings. Method signatures, vtable order, and member offsets may have changed.

### Priority classes to verify (most hooks):
- [ ] `EditorUI` — hooked in 17+ files
- [ ] `EditorPauseLayer` — hooked in 7 files
- [ ] `LevelEditorLayer` — hooked in 5 files
- [ ] `DrawGridLayer` — hooked in 3 files
- [ ] `SetGroupIDLayer` — hooked in 2 files
- [ ] `CustomizeObjectLayer` — hooked in 2 files
- [ ] `ConfigureHSVWidget` — hooked in 1 file
- [ ] `ColorSelectPopup` — hooked in 1 file
- [ ] `GJTransformControl` — hooked in 1 file
- [ ] `GJScaleControl` — hooked in 1 file
- [ ] `GJRotationControl` — hooked in 1 file
- [ ] `SetupTriggerPopup` — hooked in 1 file
- [ ] `SelectFontLayer` — hooked in 1 file
- [ ] `EditorOptionsLayer` — hooked in 1 file
- [ ] `EditButtonBar` — hooked in 2 files
- [ ] `ObjectToolbox` — hooked in 1 file
- [ ] `GameObject` — hooked in 2 files
- [ ] `CreateParticlePopup` — hooked in 1 file
- [ ] `GManager` — hooked in 1 file
- [ ] `MenuLayer` — hooked in 2 files
- [ ] `EditLevelLayer` — hooked in 1 file
- [ ] `GameManager` — hooked in 1 file
- [ ] `GJSpecialColorSelect` — hooked in 1 file
- [ ] `HSVWidgetPopup` — hooked in 1 file

**Strategy:** Compare hooked method signatures against `bindings/2.2081/GeometryDash.bro` to find changes. Many hooks may work without changes if the function signatures didn't change between 2.2074 and 2.2081.

---

## Phase 9: Medium Priority Changes

### 9.1 `matjson::makeObject` (9 occurrences)
- [ ] Check if `matjson::makeObject` still exists in v5
- [ ] Files: `src/server/Server.cpp` (8x), `src/features/backups/Backup.cpp` (1x)

### 9.2 `checkJson()` / JsonValidation (9 occurrences)
- [ ] Check if `checkJson()` API changed
- [ ] Files: `src/server/Server.cpp` (7x), `src/features/backups/Backup.cpp` (2x)

### 9.3 `Geode/utils/ranges.hpp` (5 files)
- [ ] Check if this utility header still exists or if `std::ranges` should be used
- [ ] Files: Server.cpp, BetterColorSelect.cpp, GroupSummaryPopup.cpp, TriggerIndicators.cpp, ScriptingUI.hpp

### 9.4 `matjson::Serialize<T>` specialization
- [ ] Check serialization pattern in `src/features/backups/Backup.cpp:21-38`

---

## Execution Order

1. **Phase 1** (config) — Get mod.json and CMakeLists.txt updated
2. **Phase 6** (dependencies) — Remove/stub unavailable deps so it can load
3. **Phase 2** (Popup) — Largest code change, get all 13 classes compiling
4. **Phase 3** (removed APIs) — Quick fix for getMetadataRef
5. **Phase 4** (std::function) — Systematic replacement
6. **Phase 5** (events) — Requires understanding new Event V2 API
7. **Phase 7** (web requests) — Rewrite server layer
8. **Phase 8** (hook verification) — Compare against 2.2081 bindings
9. **Phase 9** (medium priority) — Cleanup and remaining API changes

---

## Build & Test Setup

- **Geode SDK:** `C:\Users\steven\Documents\geode\geode-sdk` (at `v5.0.0-alpha.1`)
- **Build script:** `geode-sdk/build_geode.bat`
- **Compiler:** clang-cl 21.1.8, Ninja, VS 2022
- Set `GEODE_SDK` env var to point to the geode-sdk directory
- Build BetterEdit: `cmake -B build -G Ninja && cmake --build build`

---

*Created: February 5, 2026*
