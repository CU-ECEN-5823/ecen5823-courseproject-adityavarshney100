/**
 * HAL configuration settings to support ECEN 5823 application setup
 */
#define HAL_SPIDISPLAY_ENABLE                         (1)
#define HAL_SPIDISPLAY_EXTCOMIN_CALLBACK
#if defined(FEATURE_IOEXPANDER)
#define HAL_SPIDISPLAY_EXTMODE_EXTCOMIN               (0)
#else
#define HAL_SPIDISPLAY_EXTMODE_EXTCOMIN               (1)
#endif
#define HAL_SPIDISPLAY_EXTMODE_SPI                    (0)
#define HAL_SPIDISPLAY_EXTCOMIN_USE_PRS               (0)
#define HAL_SPIDISPLAY_EXTCOMIN_USE_CALLBACK          (0)
#define HAL_SPIDISPLAY_FREQUENCY                      (1000000)
