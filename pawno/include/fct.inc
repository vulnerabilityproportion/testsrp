
#define FCT_ID_NONE 0

#define FCT_NONE 0
#define FCT_BIKE 1

enum FctInfo
{
    Fct_fctName[32],
    Fct_fctType
}
new factions[][FctInfo] = {
    {"None", FCT_NONE},
    {"Hells Angels MC", FCT_BIKE},
    {"Mongols MC", FCT_BIKE},
    {"Pagans MC", FCT_BIKE},
    {"Outlaws MC", FCT_BIKE},
    {"Sons of Silence MC", FCT_BIKE},
    {"Warlocks MC", FCT_BIKE},
    {"Highwaymen MC", FCT_BIKE},
    {"Bandidos MC", FCT_BIKE},
    {"Free Souls MC", FCT_BIKE},
    {"Vagos MC", FCT_BIKE}
};
const MAX_FACTIONS = sizeof(factions);

stock Fct_GetType (fct)
{
    if (fct < 0 || fct >= MAX_FACTIONS)
        return FCT_NONE;
    
    return factions[fct][Fct_fctType];
}

stock Fct_GetName (fct, outName[], size = sizeof(outName))
{
    if (fct < 0 || fct >= MAX_FACTIONS)
        return false;

    format(outName, size, "%s", factions[fct][Fct_fctName]);
    return true;
}

stock Fct_AddMoney (fct, money)
{
    #pragma unused fct
    #pragma unused money
}
stock Fct_AddRating (fct, rating)
{
    #pragma unused fct
    #pragma unused rating
}
stock Fct_AddWare (fct, count)
{
    #pragma unused fct
    #pragma unused count
}