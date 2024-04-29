#include "rxtx_devLua.h"
#include "POWERMGNT.h"

#define MATCH_TX_OPT "MatchTX "
char strPowerLevels[] = "10mW;25mW;50mW;100mW;250mW;500mW;1000mW;2000mW;AUX9;AUX10;AUX11;AUX12";
const char STR_EMPTYSPACE[] = { 0 };
const char STR_LUA_PACKETRATES[] =
#if defined(RADIO_SX127X)
    "D50Hz(-112dBm);25Hz(-123dBm);50Hz(-120dBm);100Hz(-117dBm);100Hz Full(-112dBm);200Hz(-112dBm)";
#elif defined(RADIO_LR1121)
    "50Hz 2.4G;100Hz Full 2.4G;150Hz 2.4G;250Hz 2.4G;333Hz Full 2.4G;500Hz 2.4G;" // 2.4G
    "50Hz Low Band;100Hz Low Band;100Hz Full Low Band;200Hz Low Band;200Hz Full Low Band;250Hz Low Band;" // 900M
    "X100Hz Full(-112dBm);X150Hz(-112dBm)"; // Dual
#elif defined(RADIO_SX128X)
    "50Hz(-115dBm);100Hz Full(-112dBm);150Hz(-112dBm);250Hz(-108dBm);333Hz Full(-105dBm);500Hz(-105dBm);"
    "D250(-104dBm);D500(-104dBm);F500(-104dBm);F1000(-104dBm)";
#else
    #error Invalid radio configuration!
#endif

void luadevGeneratePowerOpts(luaItem_selection *luaPower)
{
  // This function modifies the strPowerLevels in place and must not
  // be called more than once!
  char *out = strPowerLevels;
  PowerLevels_e pwr = PWR_10mW;
  // Count the semicolons to move `out` to point to the MINth item
  while (pwr < POWERMGNT::getMinPower())
  {
    while (*out++ != ';') ;
    pwr = (PowerLevels_e)((unsigned int)pwr + 1);
  }
  // There is no min field, compensate by shifting the index when sending/receiving
  // luaPower->min = (uint8_t)MinPower;
  luaPower->options = (const char *)out;


  // Find where max supported power level is ended and then skip to auxes
  char *powerLevelsEnd = NULL;
  while (pwr <= PWR_2000mW)
  {
    // If out still points to a semicolon from the last loop move past it
    if (*out)
      ++out;
    while (*out && *out != ';')
      ++out;
    if (pwr == POWERMGNT::getMaxPower())
    {
      powerLevelsEnd = out;
    }
    pwr = (PowerLevels_e)((unsigned int)pwr + 1);
  }

#if defined(TARGET_RX)
  // The RX has the dynamic option added on to the end
  // the space on the end is to make it display "MatchTX mW"
  // but only if it has more than one power level
  if (POWERMGNT::getMinPower() != POWERMGNT::getMaxPower())
    memmove(powerLevelsEnd, MATCH_TX_OPT, strlen(MATCH_TX_OPT) + 1);
  else
    *powerLevelsEnd = '\0';
#else
  if (powerLevelsEnd != NULL && powerLevelsEnd != out)
  {
    memmove(powerLevelsEnd, out, strlen(out) + 1);
  }
#endif
}
