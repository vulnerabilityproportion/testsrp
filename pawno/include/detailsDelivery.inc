#if defined _detailsDelivery_included
	#endinput
#endif
#define _detailsDelivery_included

#if defined TEST
    #undef TEST
#endif
#define TEST


#define DSDELIVERY_MIN_COOLDOWN         5400
#define DSDELIVERY_MAX_COOLDOWN         9000

#define DSDELIVERY_MISSION_TIME         1800

#define DSDELIVERY_FCT_MONEY            200000
#define DSDELIVERY_FCT_MATERIALS        30000
#define DSDELIVERY_FCT_RATING           1500

#define DSDELIVERY_PLAYER_MONEY         100000
#define DSDELIVERY_PLAYER_RATING        1500

#define DSDELIVERY_VEHICLE_MODEL        455
#define DSDELIVERY_VEHICLE_MARK_TIME    180

#define DSDELIVERY_MAX_MISSION_MSG_TIME 3
#define DSDELIVERY_MISSION_MSG_TIME_1   1200
#define DSDELIVERY_MISSION_MSG_TIME_2   600
#define DSDELIVERY_MISSION_MSG_TIME_3   300

#define DSDELIVERY_TIME_WHEN_ERROR_START 60


stock static bool:dsDelivery_eventStarted;
stock static dsDelivery_eventTime;

enum dsDelivery_SpawnInfo
{
    Float:dsDelivery_spawnPos[4],
    dsDelivery_spawnName[32]
}
stock static dsDelivery_spawnInfo[][dsDelivery_SpawnInfo] = {
    {{2081.8970,-2006.9373,13.9873,269.4189}, "Willowfield"},
    {{2080.1389,-2019.8335,13.9837,269.6956}, "Willowfield"},
    {{2080.1313,-2033.3491,13.9844,270.4236}, "Willowfield"},
    {{2080.1367,-2046.9878,13.9845,268.8507}, "Willowfield"},
    {{-2449.1304,-120.7151,26.5687,93.3258}, "Hashbury"},
    {{-2447.6011,-125.1134,26.5827,93.3922}, "Hashbury"},
    {{1681.7417,2318.3223,11.2570,270.1104}, "Redsants East"},
    {{1680.8486,2358.8562,11.2536,270.8129}, "Redsants East"}
};
const DSDELIVERY_MAX_SPAWN_POS = sizeof(dsDelivery_spawnInfo);

stock static Float:dsDelivery_deliveryPos[MAX_FACTIONS][3] = {
    {0.0, 0.0, 0.0},
    {701.1597,-446.3981,16.3359},
    {-1295.5288,2710.1636,50.0625},
    {-2120.5686,-2496.7698,30.6250},
    {-300.7595,1319.7745,54.2191},
    {1238.4785,180.2144,20.0276},
    {658.0650,1692.3461,6.9922},
    {34.9405,-2639.2678,40.4264},
    {-1941.2568,2402.6355,49.4922},
    {-203.3469,2602.5283,62.7031},
    {-314.6039,1753.0490,42.7547}
};

stock static dsDelivery_vehicleMarkTime;
stock static dsDelivery_vehicleId = INVALID_VEHICLE_ID;

stock static dsDelivery_missionTime;
stock static dsDelivery_missionMessageTime[DSDELIVERY_MAX_MISSION_MSG_TIME] = {DSDELIVERY_MISSION_MSG_TIME_1, DSDELIVERY_MISSION_MSG_TIME_2, DSDELIVERY_MISSION_MSG_TIME_3};

stock static dsDelivery_playerMark[MAX_PLAYERS] = {INVALID_STREAMER_ID, ...};
stock static dsDelivery_driverid[MAX_VEHICLES] = {INVALID_PLAYER_ID, ...};
stock static dsDelivery_lastDriverId[MAX_VEHICLES] = {INVALID_PLAYER_ID, ...};


stock DSDelivery_Init ()
{
    SetTimer("DSDelivery_Timer1000ms", 1000, true);

    dsDelivery_eventStarted = false;

    #if defined TEST
    dsDelivery_eventTime = 15;
    #else
    dsDelivery_eventTime = exRandom(DSDELIVERY_MIN_COOLDOWN, DSDELIVERY_MAX_COOLDOWN);
    #endif
}


forward DSDelivery_Timer1000ms ();
public DSDelivery_Timer1000ms ()
{
    if (!dsDelivery_eventStarted)
    {
        if (dsDelivery_eventTime <= 0)
            return;

        if (--dsDelivery_eventTime == 0)
            DSDelivery_StartEvent();
    }
    else
    {
        if (dsDelivery_missionTime <= 0)
            return;
        
        if (dsDelivery_vehicleMarkTime)
        {
            if (--dsDelivery_vehicleMarkTime == 0)
                DSDelivery_UpdateMark();
        }

        --dsDelivery_missionTime;

        if (IsValidVehicle(dsDelivery_vehicleId) && dsDelivery_driverid[dsDelivery_vehicleId] != INVALID_PLAYER_ID)
        {
            for (new i = 0; i < DSDELIVERY_MAX_MISSION_MSG_TIME; i++)
            {
                if (dsDelivery_missionMessageTime[i] == dsDelivery_missionTime)
                {
                    DSDelivery_SendMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Действует событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE"");
                    DSDelivery_SendFmtMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Заберите и доставьте груз в течение "STR_LIGHTBLUE"%d"STR_WHITE" минут. Установлена метка на карте",
                        dsDelivery_missionTime / 60);
                }
            }
        }

        if (dsDelivery_missionTime == 0)
        {
            if (DSDelivery_StopEvent())
                DSDelivery_SendMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Завершено событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE". Flatbed не был доставлен");
        }
    }
}


stock DSDelivery_StartEvent ()
{
    if (dsDelivery_eventStarted)
        return;
    
    if (dsDelivery_vehicleId != INVALID_VEHICLE_ID)
    {
        dsDelivery_driverid[dsDelivery_vehicleId] = INVALID_PLAYER_ID;
        dsDelivery_lastDriverId[dsDelivery_vehicleId] = INVALID_PLAYER_ID;

        DestroyVehicle(dsDelivery_vehicleId);
        dsDelivery_vehicleId = INVALID_VEHICLE_ID;
    }

    new randomPos = random(DSDELIVERY_MAX_SPAWN_POS);

    dsDelivery_vehicleId = CreateVehicle(DSDELIVERY_VEHICLE_MODEL, 
        dsDelivery_spawnInfo[randomPos][dsDelivery_spawnPos][0], dsDelivery_spawnInfo[randomPos][dsDelivery_spawnPos][1],
        dsDelivery_spawnInfo[randomPos][dsDelivery_spawnPos][2], dsDelivery_spawnInfo[randomPos][dsDelivery_spawnPos][3],
        -1, -1, 86400);
    
    if (!IsValidVehicle(dsDelivery_vehicleId))
    {
        dsDelivery_eventTime = DSDELIVERY_TIME_WHEN_ERROR_START;
        return;
    }
    
    dsDelivery_eventStarted = true;
    dsDelivery_missionTime = DSDELIVERY_MISSION_TIME;

    dsDelivery_vehicleMarkTime = DSDELIVERY_VEHICLE_MARK_TIME;
    
    DSDelivery_SendFmtMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Начато событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE". Flatbed припаркован в районе "STR_LIGHTBLUE"%s",
        dsDelivery_spawnInfo[randomPos][dsDelivery_spawnName]);
    
    DSDelivery_SendFmtMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Заберите и доставьте груз в течение "STR_LIGHTBLUE"%d"STR_WHITE" минут. Установлена метка на карте",
        DSDELIVERY_MISSION_TIME / 60);
}

stock DSDelivery_StopEvent ()
{
    if (!dsDelivery_eventStarted)
        return false;
    
    dsDelivery_eventStarted = false; 
    dsDelivery_missionTime = 0;
    dsDelivery_vehicleMarkTime = 0;

    #if defined TEST
    dsDelivery_eventTime = 15;
    #else
    dsDelivery_eventTime = exRandom(DSDELIVERY_MIN_COOLDOWN, DSDELIVERY_MAX_COOLDOWN);
    #endif

    if (dsDelivery_vehicleId != INVALID_VEHICLE_ID)
    {
        dsDelivery_driverid[dsDelivery_vehicleId] = INVALID_PLAYER_ID;
        dsDelivery_lastDriverId[dsDelivery_vehicleId] = INVALID_PLAYER_ID;

        DestroyVehicle(dsDelivery_vehicleId);
        dsDelivery_vehicleId = INVALID_VEHICLE_ID;
    }

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (Fct_GetType(PlayerInfo[i][pFct]) != FCT_BIKE)
            continue;

        if (dsDelivery_playerMark[i] != INVALID_STREAMER_ID)
        {
            DestroyDynamicMapIcon(dsDelivery_playerMark[i]);
            dsDelivery_playerMark[i] = INVALID_STREAMER_ID;
        }
    }

    return true;
}

stock DSDelivery_UpdateMark ()
{
    if (!dsDelivery_eventStarted)
        return;
    
    if (dsDelivery_vehicleId == INVALID_VEHICLE_ID)
        return;

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (Fct_GetType(PlayerInfo[i][pFct]) != FCT_BIKE)
            continue;

        if (dsDelivery_playerMark[i] != INVALID_STREAMER_ID)
            DestroyDynamicMapIcon(dsDelivery_playerMark[i]);
        
        if (dsDelivery_driverid[dsDelivery_vehicleId] == i)
            continue;
        
        new Float:pos[3];
        GetVehiclePos(dsDelivery_vehicleId, pos[0], pos[1], pos[2]);
        dsDelivery_playerMark[i] = CreateDynamicMapIcon(pos[0], pos[1], pos[2], 51, 0, 0, 0, i, 50000.0, MAPICON_GLOBAL);
    }

    dsDelivery_vehicleMarkTime = DSDELIVERY_VEHICLE_MARK_TIME;
}

stock DSDelivery_OnVehicleSpawn (vehicleid)
{
    if (dsDelivery_vehicleId == vehicleid)
    {
        if (DSDelivery_StopEvent())
            DSDelivery_SendMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Завершено событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE". Flatbed был уничтожен");
    }
}

stock DSDelivery_OnPlayerStateChange (playerid, newstate, oldstate)
{
    if (!dsDelivery_eventStarted)
        return;

    new vehicleid = GetPlayerVehicleID(playerid);
    if (newstate == PLAYER_STATE_DRIVER && vehicleid == dsDelivery_vehicleId)
    {
        new fct = PlayerInfo[playerid][pFct];

        if (Fct_GetType(fct) != FCT_BIKE)
        {
            RemovePlayerFromVehicle(playerid);
            return;
        }

        if (dsDelivery_lastDriverId[vehicleid] != playerid)
        {
            new fctName[32];
            Fct_GetName(fct, fctName);

            DSDelivery_SendMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Действует событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE"");
            DSDelivery_SendFmtMessage(fct, COLOR_LIGHTBLUE, " [Событие] "STR_LIGHTBLUE"%s "STR_WHITE"находится за рулем Flatbed с деталями. Помогите в доставке деталей на склад фракции", 
                PlayerInfo[playerid][pName]);

            for (new i = 0; i < MAX_FACTIONS; i++) {
                if (factions[i][Fct_fctType] != FCT_BIKE) continue;
                if (fct == i) continue;
                DSDelivery_SendFmtMessage(i, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Flatbed перехвачен клубом "STR_LIGHTBLUE"%s"STR_WHITE". Перехватите или уничтожьте транспорт", fctName);
            }
        }

        DisablePlayerRaceCheckpoint(playerid);
        SetPlayerRaceCheckpoint(playerid, 1, dsDelivery_deliveryPos[fct][0], dsDelivery_deliveryPos[fct][1], dsDelivery_deliveryPos[fct][2], 0.0, 0.0, 0.0, 5.0);

        dsDelivery_driverid[vehicleid] = playerid;
    }
    else if (newstate == PLAYER_STATE_ONFOOT && oldstate == PLAYER_STATE_DRIVER && vehicleid == dsDelivery_vehicleId)
    {
        dsDelivery_lastDriverId[vehicleid] = playerid;
        dsDelivery_driverid[vehicleid] = INVALID_PLAYER_ID;
        DisablePlayerRaceCheckpoint(playerid);
    }
}

stock DSDelivery_OnPlayerDisconnect (playerid)
{
    if (dsDelivery_playerMark[playerid] != INVALID_STREAMER_ID)
    {
        DestroyDynamicMapIcon(dsDelivery_playerMark[playerid]);
        dsDelivery_playerMark[playerid] = INVALID_STREAMER_ID;
    }
}

stock DSDelivery_OnPlayerEnterRaceCP (playerid)
{
    if (!IsPlayerInAnyVehicle(playerid))
        return;

    if (!dsDelivery_eventStarted)
        return;

    new fct = PlayerInfo[playerid][pFct];

    if (Fct_GetType(fct) != FCT_BIKE)
        return;
    
    if (!IsPlayerInRangeOfPoint(playerid, 10.0, dsDelivery_deliveryPos[fct][0], dsDelivery_deliveryPos[fct][1], dsDelivery_deliveryPos[fct][2]))
        return;
    
    new vehicleid = GetPlayerVehicleID(playerid);
    if (vehicleid != dsDelivery_vehicleId || GetPlayerState(playerid) != PLAYER_STATE_DRIVER)
        return;
    
    if (DSDelivery_StopEvent())
    {
        new fctName[32];
        Fct_GetName(fct, fctName);

        DSDelivery_SendFmtMessageType(FCT_BIKE, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Завершено событие: "STR_LIGHTBLUE"\"Доставка деталей\""STR_WHITE". Груз был доставлен клубом %s", fctName);
        DSDelivery_GiveReward(playerid, fct);

        DisablePlayerRaceCheckpoint(playerid);
    }
}

stock DSDelivery_GiveReward (playerid, fctid)
{
    DSDelivery_SendFmtMessage(fctid, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Ваша фракция получила: %d$ на банк клуба / %d рейтинга / %d материалов", 
        DSDELIVERY_FCT_MONEY, DSDELIVERY_FCT_RATING, DSDELIVERY_FCT_MATERIALS);
    DSDelivery_SendFmtMessage(fctid, COLOR_LIGHTBLUE, " [Событие] "STR_WHITE"Игрок %s получил за доставку груза: %d$ / %d рейтинга", 
        PlayerInfo[playerid][pName], DSDELIVERY_PLAYER_MONEY, DSDELIVERY_PLAYER_RATING);
    
    Fct_AddMoney(fctid, DSDELIVERY_FCT_MONEY);
    Fct_AddRating(fctid, DSDELIVERY_FCT_RATING);
    Fct_AddWare(fctid, DSDELIVERY_FCT_MATERIALS);

    GivePlayerMoney(playerid, DSDELIVERY_PLAYER_MONEY);
    // AddPlayerRating(playerid, DSDELIVERY_PLAYER_RATING);
}

stock DSDelivery_TP (playerid)
{
    #if defined TEST
    new Float:pos[3];
    GetVehiclePos(dsDelivery_vehicleId, pos[0], pos[1], pos[2]);
    SetPlayerPos(playerid, pos[0], pos[1] + 2.0, pos[2]);
    #endif
}


stock DSDelivery_SendMessage (fct, color, const message[])
{
    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (PlayerInfo[i][pFct] != fct)
            continue;

        SendClientMessage(i, color, message);
    }
    return true;
}

stock DSDelivery_SendMessageType (fctType, color, const message[])
{
    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (Fct_GetType(PlayerInfo[i][pFct]) != fctType)
            continue;

        SendClientMessage(i, color, message);
    }
    return true;
}

stock DSDelivery_SendFmtMessage (fct, color, const text[], {Float, _}:...)
{
    static args, buffer[144];
    buffer[0] = EOS;

    if ((args = numargs()) == 3)
    {
        for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
        {
            if (!IsPlayerConnected(i))
                continue;
        
            if (PlayerInfo[i][pFct] != fct)
                continue;

            SendClientMessage(i, color, text);
        }
        return 1;
    }

    while (--args >= 3)
    {
        #emit LCTRL 5
        #emit LOAD.alt args
        #emit SHL.C.alt 2
        #emit ADD.C 12
        #emit ADD
        #emit LOAD.I
        #emit PUSH.pri
    }

    #emit PUSH.S text
    #emit PUSH.C 144
    #emit PUSH.C buffer
    #emit PUSH.S 8
    #emit SYSREQ.C format
    #emit LCTRL 5
    #emit SCTRL 4

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (PlayerInfo[i][pFct] != fct)
            continue;

        SendClientMessage(i, color, buffer);
    }

    #emit RETN
    return true;
}

stock DSDelivery_SendFmtMessageType (fctType, color, const text[], {Float, _}:...)
{
    static args, buffer[144];
    buffer[0] = EOS;

    if ((args = numargs()) == 3)
    {
        for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
        {
            if (!IsPlayerConnected(i))
                continue;
        
            if (Fct_GetType(PlayerInfo[i][pFct]) != fctType)
                continue;

            SendClientMessage(i, color, text);
        }
        return 1;
    }

    while (--args >= 3)
    {
        #emit LCTRL 5
        #emit LOAD.alt args
        #emit SHL.C.alt 2
        #emit ADD.C 12
        #emit ADD
        #emit LOAD.I
        #emit PUSH.pri
    }

    #emit PUSH.S text
    #emit PUSH.C 144
    #emit PUSH.C buffer
    #emit PUSH.S 8
    #emit SYSREQ.C format
    #emit LCTRL 5
    #emit SCTRL 4

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!IsPlayerConnected(i))
            continue;
        
        if (Fct_GetType(PlayerInfo[i][pFct]) != fctType)
            continue;

        SendClientMessage(i, color, buffer);
    }

    #emit RETN
    return 1;
}