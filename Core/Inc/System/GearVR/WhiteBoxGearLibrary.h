#ifdef __GEAR_VR

#ifdef __cplusplus
#define GEAR_EXTERN_C extern "C"
#else
#define GEAR_EXTERN_C
#endif

GEAR_EXTERN_C int GetVal();

GEAR_EXTERN_C void WBGL_Init();
GEAR_EXTERN_C void WBGL_Update();

#endif