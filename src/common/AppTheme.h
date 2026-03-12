#pragma once

// ─────────────────────────────────────────────────────────────────────────────
//  AppTheme — master include.
//
//  Widget code only needs:  #include "common/AppTheme.h"
//
//  For runtime theme switching:
//    setTheme(AppPalettes::light());
//    qApp->setStyleSheet(AppTheme::globalStyle());
//    // then re-apply widget stylesheets
// ─────────────────────────────────────────────────────────────────────────────

#include "AppColors.h"
#include "theme/ThemePalette.h"
#include "theme/Tokens.h"
#include "theme/Builders.h"
#include "theme/Global.h"
#include "theme/Labels.h"
#include "theme/Buttons.h"
#include "theme/Containers.h"
#include "theme/Icons.h"
