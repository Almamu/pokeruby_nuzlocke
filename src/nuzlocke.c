#include "global.h"
#include "nuzlocke.h"
#include "main.h"
#include "menu.h"
#include "palette.h"
#include "scanline_effect.h"
#include "sprite.h"
#include "strings2.h"
#include "task.h"
#include "pokemon.h"
#include "random.h"
#include "constants/species.h"
#include "constants/moves.h"
#include "data2.h"

// on global.h:822
/*
    u8 version:4; // version of the settings being used
    u8 pokemonDie:1; // pokemon are dead after their hp gets to 0 (no revives, no pokemon center)
    u8 itemsInBattle:1; // prevents the player from using items in battle
    u8 pokemonCenters:1; // disables the pokemon centers
    u8 hardGameOver:1; // if the player blacks out, it's game over
    u8 mustNickname:1; // the player must assign nicknames to pokemons
    u8 levelBalance:1; // the enemy team's level is balanced to the player's
    u8 noBuying:1; // disables the pokemarts, forbidding the user to buy items
    u8 noDayCare:1; // disables daycare
    u8 noExpShare:1; // disables the effects of exp share
    u8 noHeldItems:1; // disables the use of held items
    u8 randomStarter:1; // random starters for the player based on it's trainer ID, If the last number is 1-3 the player starts with a Grass type, 4-6 is Fire type, 7-9 is Water type, 0 is the player's choice. Alternatively, use the Trainer ID modulo 3 for the same purposes.
    u8 oneCapturePerMap:1; // limits the number of pokemons to be captured to one by map
*/

// Task data
#define tMenuSelection         data[ 0]
#define tInstadead             data[ 1]
#define tItemsInBattle         data[ 2]
#define tPokemonCenters        data[ 3]
#define tHardGameover          data[ 4]
#define tMustNickname          data[ 5]
#define tLevelBalance          data[ 6]
#define tNoBuying              data[ 7]
#define tNoDayCare             data[ 8]
#define tNoExpShare            data[ 9]
#define tNoHeldItems           data[10]
#define tLimitCapture          data[11]
#define tSpecificStarter       data[12]
#define tRandomWildEncounters  data[13]
#define tRandomTrainers        data[14]
#define tRandomWildAttacks     data[15]
#define tRandomTrainersAttacks data[16]
#define tRandomGenders         data[17]
enum
{
    MENUITEM_INSTADEAD,
    MENUITEM_ITEMSINBATTLE,
    MENUITEM_POKEMONCENTERS,
    MENUITEM_HARDGAMEOVER,
    MENUITEM_MUSTNICKNAME,
    MENUITEM_LEVELBALANCE,
    MENUITEM_NOBUYING,
    MENUITEM_NODAYCARE,
    MENUITEM_NOEXPSHARE,
    MENUITEM_NOHELDITEMS,
    MENUITEM_LIMITCAPTURE,
    MENUITEM_SPECIFICSTARTER,
    MENUITEM_RANDOMWILDENCOUNTERS,
    MENUITEM_RANDOMTRAINERS,
    MENUITEM_RANDOMWILDATTACKS,
    MENUITEM_RANDOMTRAINERATTACKS,
    MENUITEM_RANDOMGENDERS,
    MENUITEM_CANCEL
};

enum
{
    SPECIES_TYPE_GRASS,
    SPECIES_TYPE_FIRE,
    SPECIES_TYPE_WATER
};

const u8* nuzlockeMenu[] =
{
    gSystemText_Instadead,
    gSystemText_ItemsInBattle,
    gSystemText_PokemonCenters,
    gSystemText_HardGameover,
    gSystemText_MustNickname,
    gSystemText_LevelBalance,
    gSystemText_NoBuying,
    gSystemText_NoDayCare,
    gSystemText_NoExpShare,
    gSystemText_NoHeldItems,
    gSystemText_OneCapturePerMap,
    gSystemText_SpecificStarter,
    gSystemText_RandomWildEncounters,
    gSystemText_RandomTrainers,
    gSystemText_RandomWildAttacks,
    gSystemText_RandomTrainerAttacks,
    gSystemText_RandomGenders,
    gSystemText_Cancel
};

const u8* nuzlockeMenuDesc[] =
{
    gSystemText_InstadeadDesc,
    gSystemText_ItemsInBattleDesc,
    gSystemText_PokemonCentersDesc,
    gSystemText_HardGameoverDesc,
    gSystemText_MustNicknameDesc,
    gSystemText_LevelBalanceDesc,
    gSystemText_NoBuyingDesc,
    gSystemText_NoDayCareDesc,
    gSystemText_NoExpShareDesc,
    gSystemText_NoHeldItemsDesc,
    gSystemText_OneCapturePerMapDesc,
    gSystemText_SpecificStarterDesc,
    gSystemText_RandomWildEncountersDesc,
    gSystemText_RandomTrainersDesc,
    gSystemText_RandomWildAttacksDesc,
    gSystemText_RandomTrainerAttacksDesc,
    gSystemText_RandomGendersDesc,
    gSystemText_CancelDesc
};

const u8* nuzlockeCaptureValues[] =
{
    gSystemText_CaptureNoLimit,
    gSystemText_CaptureFirstEncounter,
    gSystemText_CaptureFirstCapture,
};

EWRAM_DATA static u16 randomizedStarters[] = {SPECIES_NONE, SPECIES_NONE, SPECIES_NONE};
EWRAM_DATA static u32 gNuzlockeRandomSeed = 0;
EWRAM_DATA static u16 data[18] = {0}; // TODO: BETTER STORAGE FOR THE SETTINGS, WE SHOULDN'T BE USING EWRAM WHEN WE HAVE 32 BYTES ALREADY RESERVED AT THE TASK FOR THIS KIND OF DATA

extern const u16 gUnknown_0839F5FC[];
// note: this is only used in the Japanese release
extern const u8 gUnknown_0839F63C[];

static void Task_NuzlockeMenuFadeIn(u8 taskId);
static void Task_NuzlockeMenuSave(u8 taskId);
static void Task_NuzlockeMenuFadeOut(u8 taskId);
static void Task_NuzlockeUpdateMenu(u8 taskId);

static void NuzlockeHighlightMenuItem(u8 index);
static bool8 NuzlockeMenuProcessInput(u8 taskId);
static void NuzlockeDrawOption(u8 index, u8 position, u8 taskId);
static void NuzlockeUpdateDescription(const u8* message);
static void NuzlockeGenerateStarters();
static u32 Nuzlocke_Random();
static void Nuzlocke_SeedRng(u32 seed);

static u32 Nuzlocke_Random(void)
{
    gNuzlockeRandomSeed = 1103515245 * gNuzlockeRandomSeed + 24691;
    return gNuzlockeRandomSeed;
}

void Nuzlocke_SeedRng(u32 seed)
{
    gNuzlockeRandomSeed = seed;
}

static void MainCB(void)
{
    RunTasks();
    AnimateSprites();
    BuildOamBuffer();
    UpdatePaletteFade();
}

static void VBlankCB(void)
{
    LoadOam();
    ProcessSpriteCopyRequests();
    TransferPlttBuffer();
}

void CB2_InitNuzlockeMenu(void)
{
    switch (gMain.state)
    {
    default:
    case 0:
        SetVBlankCallback(NULL);
        REG_DISPCNT = 0;
        REG_BG2CNT = 0;
        REG_BG1CNT = 0;
        REG_BG0CNT = 0;
        REG_BG2HOFS = 0;
        REG_BG2VOFS = 0;
        REG_BG1HOFS = 0;
        REG_BG1VOFS = 0;
        REG_BG0HOFS = 0;
        REG_BG0VOFS = 0;
        DmaFill16Large(3, 0, (u8 *)VRAM, 0x18000, 0x1000);
        DmaClear32(3, OAM, OAM_SIZE);
        DmaClear16(3, PLTT, PLTT_SIZE);
        gMain.state++;
        break;
    case 1:
        ResetPaletteFade();
        ScanlineEffect_Stop();
        ResetTasks();
        ResetSpriteData();
        gMain.state++;
        break;
    case 2:
        Text_LoadWindowTemplate(&gWindowTemplate_81E71B4);
        gMain.state++;
        break;
    case 3:
        MultistepInitMenuWindowBegin(&gWindowTemplate_81E71B4);
        gMain.state++;
        break;
    case 4:
        if (!MultistepInitMenuWindowContinue())
            return;
        gMain.state++;
        break;
    case 5:
        LoadPalette(gUnknown_0839F5FC, 0x80, 0x40);
        CpuCopy16(gUnknown_0839F63C, (void *)0x0600BEE0, 0x40);
        gMain.state++;
        break;
    case 6:
        BeginNormalPaletteFade(0xFFFFFFFF, 0, 16, 0, RGB(0, 0, 0));
        gMain.state++;
        break;
    case 7:
    {
        u16 savedIme;

        REG_WIN0H = 0;
        REG_WIN0V = 0;
        REG_WIN1H = 0;
        REG_WIN1V = 0;
        REG_WININ = 0x1111;
        REG_WINOUT = 0x31;
        REG_BLDCNT = 0xE1;
        REG_BLDALPHA = 0;
        REG_BLDY = 7;
        savedIme = REG_IME;
        REG_IME = 0;
        REG_IE |= INTR_FLAG_VBLANK;
        REG_IME = savedIme;
        REG_DISPSTAT |= DISPSTAT_VBLANK_INTR;
        SetVBlankCallback(VBlankCB);
        REG_DISPCNT = DISPCNT_MODE_0 | DISPCNT_OBJ_1D_MAP | DISPCNT_BG0_ON | DISPCNT_OBJ_ON |
          DISPCNT_WIN0_ON | DISPCNT_WIN1_ON;
        gMain.state++;
        break;
    }
    case 8:
    {
        CreateTask(Task_NuzlockeMenuFadeIn, 0);

        // load settings from the savedata
        tMenuSelection         = 255;
        tInstadead             = gSaveBlock2.nuzlockeData.pokemonDie;
        tItemsInBattle         = gSaveBlock2.nuzlockeData.itemsInBattle;
        tPokemonCenters        = gSaveBlock2.nuzlockeData.pokemonCenters;
        tHardGameover          = gSaveBlock2.nuzlockeData.hardGameOver;
        tMustNickname          = gSaveBlock2.nuzlockeData.mustNickname;
        tLevelBalance          = gSaveBlock2.nuzlockeData.levelBalance;
        tNoBuying              = gSaveBlock2.nuzlockeData.noBuying;
        tNoDayCare             = gSaveBlock2.nuzlockeData.noDayCare;
        tNoExpShare            = gSaveBlock2.nuzlockeData.noExpShare;
        tNoHeldItems           = gSaveBlock2.nuzlockeData.noHeldItems;
        tLimitCapture          = gSaveBlock2.nuzlockeData.oneCapturePerMap;
        tSpecificStarter       = gSaveBlock2.nuzlockeData.specificStarter;
        tRandomWildEncounters  = gSaveBlock2.nuzlockeData.wildRandom;
        tRandomTrainers        = gSaveBlock2.nuzlockeData.trainersRandom;
        tRandomWildAttacks     = gSaveBlock2.nuzlockeData.wildAttacksRandom;
        tRandomTrainersAttacks = gSaveBlock2.nuzlockeData.trainerAttacksRandom;
        tRandomGenders         = gSaveBlock2.nuzlockeData.gendersRandom;

        REG_WIN0H = WIN_RANGE(17, 223);
        REG_WIN0V = WIN_RANGE(1, 31);

        gMain.state++;
        break;
    }
    case 9:
        SetMainCallback2(MainCB);
        return;
    }
}

static void Task_NuzlockeMenuFadeIn(u8 taskId)
{
    if (!gPaletteFade.active)
        gTasks[taskId].func = Task_NuzlockeUpdateMenu;
}

static bool8 NuzlockeMenuProcessInput(u8 taskId)
{
    if (tMenuSelection == 255)
    {
        tMenuSelection = 0;
        return 1;
    }

    if (gMain.newKeys & A_BUTTON)
    {
        if (tMenuSelection == MENUITEM_CANCEL)
            gTasks[taskId].func = Task_NuzlockeMenuSave;
    }
    else if (gMain.newKeys & B_BUTTON)
    {
        gTasks[taskId].func = Task_NuzlockeMenuSave;
    }
    else if (gMain.newAndRepeatedKeys & DPAD_UP)
    {
        if (tMenuSelection > 0)
            tMenuSelection--;
        else
            tMenuSelection = MENUITEM_CANCEL;

        return 1;
    }
    else if (gMain.newAndRepeatedKeys & DPAD_DOWN)
    {
        if (tMenuSelection < MENUITEM_CANCEL)
            tMenuSelection++;
        else
            tMenuSelection = 0;

        return 1;
    }
    else if (gMain.newAndRepeatedKeys & DPAD_LEFT && tMenuSelection != MENUITEM_CANCEL)
    {
        if (tMenuSelection == MENUITEM_SPECIFICSTARTER)
        {
            if (tSpecificStarter == SPECIES_NONE)
            {
                tSpecificStarter = SPECIES_EGG;
            }
            else if (tSpecificStarter == SPECIES_TREECKO)
            {
                tSpecificStarter = SPECIES_CELEBI;
            }
            else
                tSpecificStarter --;
        }
        else if (tMenuSelection == MENUITEM_LIMITCAPTURE)
        {
            if (tLimitCapture == NUZLOCKE_CAPTUREMODE_NOLIMIT)
            {
                tLimitCapture = NUZLOCKE_CAPTUREMODE_FIRSTCAPTURE;
            }
            else
            {
                tLimitCapture --;
            }
        }
        else
        {
            data[1 + tMenuSelection] = 0;
        }
        return 1;
    }
    else if (gMain.newAndRepeatedKeys & DPAD_RIGHT && tMenuSelection != MENUITEM_CANCEL)
    {
        if (tMenuSelection == MENUITEM_SPECIFICSTARTER)
        {
            if (tSpecificStarter == SPECIES_EGG)
            {
                tSpecificStarter = SPECIES_NONE;
            }
            else if (tSpecificStarter == SPECIES_CELEBI)
            {
                tSpecificStarter = SPECIES_TREECKO;
            }
            else
                tSpecificStarter ++;
        }
        else if (tMenuSelection == MENUITEM_LIMITCAPTURE)
        {
            if (tLimitCapture == NUZLOCKE_CAPTUREMODE_FIRSTCAPTURE)
            {
                tLimitCapture = NUZLOCKE_CAPTUREMODE_NOLIMIT;
            }
            else
            {
                tLimitCapture ++;
            }
        }
        else
        {
            data[1 + tMenuSelection] = 1;
        }

        return 1;
    }

    return 0;
}

static void Task_NuzlockeMenuSave(u8 taskId)
{
    // save the settings
    gSaveBlock2.nuzlockeData.pokemonDie           = tInstadead;
    gSaveBlock2.nuzlockeData.itemsInBattle        = tItemsInBattle;
    gSaveBlock2.nuzlockeData.pokemonCenters       = tPokemonCenters;
    gSaveBlock2.nuzlockeData.hardGameOver         = tHardGameover;
    gSaveBlock2.nuzlockeData.mustNickname         = tMustNickname;
    gSaveBlock2.nuzlockeData.levelBalance         = tLevelBalance;
    gSaveBlock2.nuzlockeData.noBuying             = tNoBuying;
    gSaveBlock2.nuzlockeData.noDayCare            = tNoDayCare;
    gSaveBlock2.nuzlockeData.noExpShare           = tNoExpShare;
    gSaveBlock2.nuzlockeData.noHeldItems          = tNoHeldItems;
    gSaveBlock2.nuzlockeData.oneCapturePerMap     = tLimitCapture;
    gSaveBlock2.nuzlockeData.specificStarter      = tSpecificStarter;
    gSaveBlock2.nuzlockeData.wildRandom           = tRandomWildEncounters;
    gSaveBlock2.nuzlockeData.trainersRandom       = tRandomTrainers;
    gSaveBlock2.nuzlockeData.wildAttacksRandom    = tRandomWildAttacks;
    gSaveBlock2.nuzlockeData.trainerAttacksRandom = tRandomTrainersAttacks;
    gSaveBlock2.nuzlockeData.gendersRandom        = tRandomGenders;

    BeginNormalPaletteFade(0xFFFFFFFF, 0, 0, 16, RGB(0, 0, 0));
    gTasks[taskId].func = Task_NuzlockeMenuFadeOut;
}

static void Task_NuzlockeMenuFadeOut(u8 taskId)
{
    if (!gPaletteFade.active)
    {
        DestroyTask(taskId);
        SetMainCallback2(gMain.savedCallback);
    }
}

static void Task_NuzlockeUpdateMenu(u8 taskId)
{
    u8 position = 0;
    u8 start = 0;

    if (NuzlockeMenuProcessInput(taskId))
    {
        position = tMenuSelection % 4;
        start = tMenuSelection / 4 * 4;

        Menu_DrawStdWindowFrame(2, 0, 27, 3);   // title box
        Menu_DrawStdWindowFrame(2, 4, 27, 13);  // options list box
        Menu_DrawStdWindowFrame(2, 14, 27, 19); // description box

        Menu_PrintText(gSystemText_NuzlockeMenu,  4,  1);

        NuzlockeDrawOption(start,     0, taskId);
        NuzlockeDrawOption(start + 1, 1, taskId);
        NuzlockeDrawOption(start + 2, 2, taskId);
        NuzlockeDrawOption(start + 3, 3, taskId);

        NuzlockeHighlightMenuItem(position);
        NuzlockeUpdateDescription(nuzlockeMenuDesc[tMenuSelection]);
    }
}

static void NuzlockeDrawOption(u8 index, u8 position, u8 taskId)
{
    if (index > MENUITEM_CANCEL)
        return;

    Menu_PrintText(nuzlockeMenu[index], 4,  5 + (position * 2));

    switch(index)
    {
        case MENUITEM_SPECIFICSTARTER:
            switch(tSpecificStarter)
            {
                case SPECIES_NONE:
                    Menu_PrintText(gSystemText_SpecificStarterNone, 15, 5 + (position * 2));
                    break;
                case SPECIES_EGG:
                    Menu_PrintText(gSystemText_SpecificStarterRandom, 15, 5 + (position * 2));
                    break;
                default:
                    Menu_PrintText(gSpeciesNames[tSpecificStarter], 15, 5 + (position * 2));
                    break;
            }
            break;
        case MENUITEM_LIMITCAPTURE:
            Menu_PrintText(nuzlockeCaptureValues[tLimitCapture], 15, 5 + (position * 2));
            break;
        default:
            Menu_PrintText((data[1 + index]) ? gSystemText_On : gSystemText_Off,  24, 5 + (position * 2));
            break;
        case MENUITEM_CANCEL:
            break;
    }
}

//This version uses addition '+' instead of OR '|'.
#define WIN_RANGE_(a, b) (((a) << 8) + (b))

static void NuzlockeHighlightMenuItem(u8 index)
{
    REG_WIN1H = WIN_RANGE(24, 215);
    REG_WIN1V = WIN_RANGE_(index * 16 + 40, index * 16 + 56);
}

static void NuzlockeUpdateDescription(const u8* message)
{
    Menu_PrintText(message, 4, 15);
}

void Nuzlocke_DefaultData(void)
{
    memset(&gSaveBlock2.nuzlockeData.capturedZones[0], 0x00, NUZLOCKE_SITUATION_DATA);
    // the seed for the random implementation of wild encounters
    gSaveBlock2.nuzlockeData.randomSeed = Random();
}

bool16 Nuzlocke_CanCaptureOn(struct MapHeader *mapHeader)
{
    u16 bit = 0;
    u16 byteToRead = 0;
    u8 byte = 0;
    u8 bitRead = 0;
    u16 bitCount = NUZLOCKE_SITUATION_DATA * 8;

    if (Nuzlocke_CaptureMode() == NUZLOCKE_CAPTUREMODE_NOLIMIT)
    {
        return TRUE;
    }

    // restrict capture to known maps only
    if (mapHeader->regionMapSectionId > bitCount)
    {
        return FALSE;
    }

    bit         = mapHeader->regionMapSectionId;
    byteToRead  = bit / 8;
    byte        = gSaveBlock2.nuzlockeData.capturedZones [byteToRead];
    bitRead     = byte >> (bit - (byteToRead * 8));

    return (bitRead & 0x01) == 0x00;
}

void Nuzlocke_CapturedOn(struct MapHeader *mapHeader)
{
    u16 bit = 0;
    u16 byteToRead = 0;
    u16 bitCount = NUZLOCKE_SITUATION_DATA * 8;

    // restrict capture to known maps only
    if (mapHeader->regionMapSectionId > bitCount)
    {
        return;
    }

    bit         = mapHeader->regionMapSectionId;
    byteToRead  = bit / 8;
    
    gSaveBlock2.nuzlockeData.capturedZones [byteToRead] |= 1 << (bit - (byteToRead * 8));
}

bool16 Nuzlocke_IsPokemonInstadead()
{
    return gSaveBlock2.nuzlockeData.pokemonDie == 1;
}

bool16 Nuzlocke_AreItemsAllowedMidBattle()
{
    return gSaveBlock2.nuzlockeData.itemsInBattle == 0;
}

bool16 Nuzlocke_ArePokemonCentersAllowed()
{
    return gSaveBlock2.nuzlockeData.pokemonCenters == 0;
}

bool16 Nuzlocke_IsBlackoutAllowed()
{
    return gSaveBlock2.nuzlockeData.hardGameOver == 0;
}

bool16 Nuzlocke_IsForcedToNickname()
{
    return gSaveBlock2.nuzlockeData.mustNickname == 1;
}

bool16 Nuzlocke_ShouldBalanceLevels()
{
    return gSaveBlock2.nuzlockeData.levelBalance == 1;
}

bool16 Nuzlocke_IsBuyingItemsAllowed()
{
    return gSaveBlock2.nuzlockeData.noBuying == 0;
}

bool16 Nuzlocke_IsDaycareAllowed()
{
    return gSaveBlock2.nuzlockeData.noDayCare == 0;
}

bool16 Nuzlocke_IsExpShareAllowed()
{
    return gSaveBlock2.nuzlockeData.noExpShare == 0;
}

bool16 Nuzlocke_AreHeldItemsAllowed()
{
    return gSaveBlock2.nuzlockeData.noHeldItems == 0;
}

u8 Nuzlocke_CaptureMode()
{
    return gSaveBlock2.nuzlockeData.oneCapturePerMap;
}

bool16 Nuzlocke_SpecificStarter()
{
    return gSaveBlock2.nuzlockeData.specificStarter > 0;
}

bool16 Nuzlocke_AreWildMonsRandomized()
{
    return gSaveBlock2.nuzlockeData.wildRandom == 1;
}

bool16 Nuzlocke_AreTrainersRandomized()
{
    return gSaveBlock2.nuzlockeData.trainersRandom == 1;
}

bool16 Nuzlocke_AreWildAttacksRandomized()
{
    return gSaveBlock2.nuzlockeData.wildAttacksRandom == 1;
}

bool16 Nuzlocke_AreTrainerAttacksRandomized()
{
    return gSaveBlock2.nuzlockeData.trainerAttacksRandom == 1;
}

bool16 Nuzlocke_AreGendersRandomized()
{
    return gSaveBlock2.nuzlockeData.gendersRandom == 1;
}


u16 Nuzlocke_RandomAttack(u16 baseSpecies, u8 monIndex, u8 extra)
{
    u16 move = MOVE_NONE;
    // seed the random with a specific, predictable seed
    // we do not want to affect the game's rng
    // so our own, copied implementation should be enough
    Nuzlocke_SeedRng((gSaveBlock2.nuzlockeData.randomSeed | (((monIndex << 8) | extra) << 16)) + baseSpecies);
    // generate a valid move
    do
    {
        move = Nuzlocke_Random() % NUM_MOVES;
    }
    while(move == MOVE_NONE);

    return move;
}

u16 Nuzlocke_RandomSpecies(u16 baseSpecies, u8 wildMonIndex, struct MapHeader *mapHeader)
{
    u16 species = SPECIES_NONE;
    // seed the random with a specific, predectible seed
    // we do not want to affect the game's rng
    // so our own, copied implementation should be enough
    Nuzlocke_SeedRng((gSaveBlock2.nuzlockeData.randomSeed | (((wildMonIndex << 8) | mapHeader->regionMapSectionId) << 16)) + baseSpecies);
    // generate a valid random species and return it
    do
    {
        species = Nuzlocke_Random() % SPECIES_EGG;
    }
    while(species == SPECIES_NONE || (species > SPECIES_CELEBI && species < SPECIES_TREECKO));

    return species;
}

u8 Nuzlocke_RandomGender(u16 species, u16 personality)
{
    if (gBaseStats [species].genderRatio == MON_GENDERLESS)
    {
        return MON_GENDERLESS;
    }

    Nuzlocke_SeedRng((species << 16) | (personality * gBaseStats [species].genderRatio));

    switch(Nuzlocke_Random() % 2)
    {
        case 0:
            return MON_MALE;
            break;

        default:
        case 1:
            return MON_FEMALE;
            break;

    }

    return MON_FEMALE;
}

u16 Nuzlocke_GetStarterMon(u16 n)
{
    if (gSaveBlock2.nuzlockeData.specificStarter > 0 && gSaveBlock2.nuzlockeData.specificStarter != SPECIES_EGG)
        return gSaveBlock2.nuzlockeData.specificStarter;

    if (randomizedStarters[0] == SPECIES_NONE)
        NuzlockeGenerateStarters();

    if (n > 2)
        n = 0;

    return randomizedStarters [n];
}

#define ByteRead16(ptr) ((ptr)[0] | ((ptr)[1] << 8))

void NuzlockeGenerateStarters()
{
    u16 trainerId = ByteRead16(gSaveBlock2.playerTrainerId);
    u8 type = trainerId % 3;
    u8 typeToSearch = TYPE_NORMAL;
    u8 found = 0;

    switch (type)
    {
        default:
        case SPECIES_TYPE_WATER:
            typeToSearch = TYPE_WATER;
            break;
        case SPECIES_TYPE_FIRE:
            typeToSearch = TYPE_FIRE;
            break;
        case SPECIES_TYPE_GRASS:
            typeToSearch = TYPE_GRASS;
            break;
    }

    do
    {
        u16 value = (Random () << 8 | Random ()) % NUM_SPECIES;
        // do not try with species_none or species_egg
        if (value == SPECIES_NONE || value == SPECIES_EGG) continue;
        // ignore pokemons that do not have the given type
        if (gBaseStats [value].type1 != typeToSearch && gBaseStats [value].type2 != typeToSearch) continue;
        randomizedStarters [found++] = value;
    }
    while (found < 3);
}