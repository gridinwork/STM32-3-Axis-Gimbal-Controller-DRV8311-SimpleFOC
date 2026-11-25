p=r'C:\Users\Oleg\Documents\Arduino\libraries\Simple_FOC\src\drivers\hardware_specific\stm32\stm32_mcu.cpp'
s=open(p,encoding='utf-8').read()
old="  enableTimerClock(handle);\n  HAL_TIM_Base_Init(handle);"
new="""#if !defined(__MBED__) && defined(STM32_CORE_VERSION) && (STM32_CORE_VERSION >= 0x03000000)
  enableTimerClock(handle->Instance);   // GEC patch: STM32duino 3.x expects TIM_TypeDef*
#else
  enableTimerClock(handle);
#endif
  HAL_TIM_Base_Init(handle);"""
assert old in s, "pattern not found"
open(p,'w',encoding='utf-8').write(s.replace(old,new))
print("patched")
