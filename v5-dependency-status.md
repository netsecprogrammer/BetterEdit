# BetterEdit v5 Dependency Compatibility Status

**Checked against:** GD 2.2081 / Geode 5.0.0-alpha.1
**API endpoint:** `https://api.geode-sdk.org/v1/mods?gd=2.2081&geode=5.0.0-alpha.1`
**Date:** February 5, 2026

---

## Summary

| Dependency | Required By | Importance | v5 Available? | Latest Version | Notes |
|-----------|------------|------------|---------------|----------------|-------|
| `geode.node-ids` | BetterEdit | required | **YES** | 1.22.0-beta.1 | Available for 2.2081/v5 |
| `geode.custom-keybinds` | BetterEdit | required (win/mac) | **NO** | 1.11.0 (Geode 4.8.0) | Not ported to v5 |
| `hjfod.gmd-api` | BetterEdit | required | **NO** | 1.4.3 (Geode 4.5.0) | Not ported to v5 |
| `cvolton.level-id-api` | BetterEdit | required | **YES** | 2.0.0-beta.1 | Available for 2.2081/v5 |
| `hjfod.trashcan` | BetterEdit | recommended | **NO** | 1.1.0 (Geode 4.0.0) | Not ported to v5 |
| `hjfod.backups` | BetterEdit | suggested | **NO** | 2.0.2 (Geode 4.5.0) | Not ported to v5 |
| `alphalaneous.awesome_modifier_icons` | BetterEdit | suggested | **NO** | 1.0.11 (Geode 4.10.0) | Not ported to v5 |
| `alphalaneous.improvedlink` | BetterEdit | suggested | **NO** | 1.0.15 (Geode 4.8.0) | Not ported to v5 |
| `alphalaneous.improved_group_view` | BetterEdit | suggested | **NO** | 1.0.17 (Geode 4.10.0) | Not ported to v5 |

---

## Critical Blockers

### Required dependencies NOT available for v5:

1. **`geode.custom-keybinds` (v1.11.0)** — Required on Windows and macOS
   - BetterEdit uses this for editor keybinding registration (`src/features/Keybinds.cpp`)
   - Without this, the keybinds feature cannot function
   - **Workaround:** Make dependency optional/conditional, or disable keybinds until ported

2. **`hjfod.gmd-api` (v1.4.3)** — Required
   - Used for level backup/export functionality
   - **Workaround:** Make dependency optional, or stub the API calls

### Required dependencies available for v5:

1. **`geode.node-ids` (v1.22.0-beta.1)** — Available
   - BetterEdit requires v1.21.0, the v5-compatible version is v1.22.0-beta.1
   - Update version requirement in mod.json

2. **`cvolton.level-id-api` (v2.0.0-beta.1)** — Available
   - BetterEdit requires v1.0.0, the v5-compatible version is v2.0.0-beta.1
   - Major version bump — check for API changes

---

## Recommended Approach

For initial v5 porting, consider:
1. Make `geode.custom-keybinds` optional (guard with `#ifdef` or runtime check)
2. Make `hjfod.gmd-api` optional (guard backup features)
3. Remove or make optional all `suggested` dependencies (they are non-critical)
4. Update `geode.node-ids` requirement to `1.22.0-beta.1`
5. Update `cvolton.level-id-api` requirement to `2.0.0-beta.1`

---

*Generated: February 5, 2026*
