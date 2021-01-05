#!/bin/bash
EMLIB_DIR=/c/SiliconLabs/SimplicityStudio/v4/developer/sdks/gecko_sdk_suite/v2.7
cp ${EMLIB_DIR}/util/silicon_labs/silabs_core/graphics/graphics.h .
cp ${EMLIB_DIR}/util/silicon_labs/silabs_core/graphics/graphics.c .
cp ${EMLIB_DIR}/util/silicon_labs/silabs_core/graphics/displayconfigapp.h .
mkdir -p hardware/kit/EFR32BG13_BRD4104A/config/
cp ${EMLIB_DIR}/hardware/kit/EFR32BG13_BRD4104A/config/displayconfig.h hardware/kit/EFR32BG13_BRD4104A/config/
cp ${EMLIB_DIR}/hardware/kit/EFR32BG13_BRD4104A/config/displayls013b7dh03config.h hardware/kit/EFR32BG13_BRD4104A/config/
cp ${EMLIB_DIR}/hardware/kit/EFR32BG13_BRD4104A/config/displaypalconfig.h hardware/kit/EFR32BG13_BRD4104A/config/
mkdir -p hardware/kit/common/drivers
cp ${EMLIB_DIR}/hardware/kit/common/drivers/display.c hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/display.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displaybackend.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayconfigall.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayfont16x20.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayfont6x8.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayfont8x8.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayls013b7dh03.c hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displayls013b7dh03.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displaypal.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/displaypalemlib.c hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/emstatus.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/i2cspm.c hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/udelay.c hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/textdisplay.h hardware/kit/common/drivers/
cp ${EMLIB_DIR}/hardware/kit/common/drivers/retargettextdisplay.h hardware/kit/common/drivers/
mkdir -p platform/emlib/inc/
cp ${EMLIB_DIR}/platform/emlib/inc/em_letimer.h platform/emlib/inc
mkdir -p platform/emlib/src/
cp ${EMLIB_DIR}/platform/emlib/src/em_letimer.c platform/emlib/src
mkdir -p platform/middleware/
cp -r ${EMLIB_DIR}/platform/middleware/glib platform/middleware
rm -rf platform/middleware/glib/dmd/ssd2119
rm -rf platform/middleware/glib/dmd/etm043010edh6
