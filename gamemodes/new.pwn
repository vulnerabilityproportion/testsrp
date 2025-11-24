
#pragma option -d2

#include <crashdetect>
#include <a_samp>
#include <streamer>

#define exRandom(%0,%1) random(%1-%0)+%0
const BYTES_PER_CELL = (cellbits / 8);

main() {}

enum pInfo
{
    pName[MAX_PLAYER_NAME + 1],
    pFct
}
new PlayerInfo[MAX_PLAYERS][pInfo];

#include <colors>
#include <fct>
#include <detailsDelivery>

public OnGameModeInit ()
{
    DSDelivery_Init();
    return 1;
}

public OnVehicleSpawn(vehicleid)
{
    DSDelivery_OnVehicleSpawn(vehicleid);
    return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
    DSDelivery_OnPlayerStateChange (playerid, newstate);
    return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
    DSDelivery_OnPlayerDisconnect (playerid);
    return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
    DSDelivery_OnPlayerEnterRaceCP (playerid);
    return 1;
}

public OnPlayerConnect (playerid)
{
    GetPlayerName(playerid, PlayerInfo[playerid][pName], MAX_PLAYER_NAME);
    PlayerInfo[playerid][pFct] = FCT_NONE;
    return 1;
}

public OnPlayerCommandText (playerid, const cmdtext[])
{
    if (strcmp(cmdtext, "/randomfct") == 0)
    {
        new count;

        while (PlayerInfo[playerid][pFct] == FCT_NONE)
        {
            if (++count >= MAX_FACTIONS * 2)
                assert(printf("Infinity while from RandomFct: %d : %d", MAX_FACTIONS, count));

            PlayerInfo[playerid][pFct] = random(MAX_FACTIONS);
        }

        SendClientMessage(playerid, -1, " Выполнено");
        return 1;
    }

    if (strcmp(cmdtext, "/tpcar") == 0)
    {
        DSDelivery_TP(playerid);
        return 1;
    }
    return 1;
}

stock IsVehicleOccupied (vehicleid)
{
    if (vehicleid == INVALID_VEHICLE_ID)
        return false;

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (GetPlayerVehicleID(i) == vehicleid && GetPlayerState(i) == PLAYER_STATE_DRIVER)
            return true;
    }
    return false;
}