#include "rxtx_devLua.h"
#include "POWERMGNT.h"

static const char *powerStrings[] = {
    "10mW",
    "25mW",
    "50mW",
    "100mW",
    "250mW",
    "500mW",
    "1000mW",
    "2000mW",
};
static const char *powerManagementAuxes = "AUX9;AUX10;AUX11;AUX12";
static const char *matchTxOpt = "MatchTX";
char strPowerLevels[100];
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

void luadevGeneratePowerOpts()
{
  char *out = strPowerLevels;
  PowerLevels_e pwr = POWERMGNT::getMinPower();
  while (pwr <= POWERMGNT::getMaxPower())
  {
    const char *powerStr = powerStrings[pwr];
    strcpy(out, powerStr);
    out += strlen(powerStr);
    strcpy(out, ";");
    out += 1;

    pwr = (PowerLevels_e)((unsigned int)pwr + 1);
  }

#if defined(TARGET_RX)
  if (POWERMGNT::getMinPower() != POWERMGNT::getMaxPower())
  {
    strcpy(out, matchTxOpt);
  }
#else
  strcpy(out, powerManagementAuxes);
#endif
}
