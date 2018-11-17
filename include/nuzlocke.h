#ifndef GUARD_NUZLOCKE_H
#define GUARD_NUZLOCKE_H

#include "global.fieldmap.h"

#define NUZLOCKE_CAPTUREMODE_NOLIMIT 0
#define NUZLOCKE_CAPTUREMODE_FIRSTENCOUNTER 1
#define NUZLOCKE_CAPTUREMODE_FIRSTCAPTURE 2

extern const u8 gSystemText_NuzlockeMenu[];
extern const u8 gSystemText_Instadead[];
extern const u8 gSystemText_InstadeadDesc[];
extern const u8 gSystemText_ItemsInBattle[];
extern const u8 gSystemText_ItemsInBattleDesc[];
extern const u8 gSystemText_PokemonCenters[];
extern const u8 gSystemText_PokemonCentersDesc[];
extern const u8 gSystemText_HardGameover[];
extern const u8 gSystemText_HardGameoverDesc[];
extern const u8 gSystemText_MustNickname[];
extern const u8 gSystemText_MustNicknameDesc[];
extern const u8 gSystemText_LevelBalance[];
extern const u8 gSystemText_LevelBalanceDesc[];
extern const u8 gSystemText_NoBuying[];
extern const u8 gSystemText_NoBuyingDesc[];
extern const u8 gSystemText_NoDayCare[];
extern const u8 gSystemText_NoDayCareDesc[];
extern const u8 gSystemText_NoExpShare[];
extern const u8 gSystemText_NoExpShareDesc[];
extern const u8 gSystemText_NoHeldItems[];
extern const u8 gSystemText_NoHeldItemsDesc[];
extern const u8 gSystemText_OneCapturePerMap[];
extern const u8 gSystemText_OneCapturePerMapDesc[];
extern const u8 gSystemText_SpecificStarter[];
extern const u8 gSystemText_SpecificStarterDesc[];
extern const u8 gSystemText_SpecificStarterNone[];
extern const u8 gSystemText_SpecificStarterRandom[];
extern const u8 gSystemText_CaptureNoLimit[];
extern const u8 gSystemText_CaptureFirstEncounter[];
extern const u8 gSystemText_CaptureFirstCapture[];
extern const u8 gSystemText_RandomWildEncounters[];
extern const u8 gSystemText_RandomWildEncountersDesc[];
extern const u8 gSystemText_RandomTrainers[];
extern const u8 gSystemText_RandomTrainersDesc[];
extern const u8 gSystemText_RandomWildAttacks[];
extern const u8 gSystemText_RandomWildAttacksDesc[];
extern const u8 gSystemText_RandomTrainerAttacks[];
extern const u8 gSystemText_RandomTrainerAttacksDesc[];
extern const u8 gSystemText_RandomGenders[];
extern const u8 gSystemText_RandomGendersDesc[];
extern const u8 gSystemText_CancelDesc[];
extern const u8 SystemText_Nuzlocke[];
extern const u8 gSystemText_NuzlockeRuleBlocks[];

void Nuzlocke_DefaultData();
bool16 Nuzlocke_CanCaptureOn(struct MapHeader *mapHeader);
void Nuzlocke_CapturedOn(struct MapHeader *mapHeader);
bool16 Nuzlocke_IsPokemonInstadead();
bool16 Nuzlocke_AreItemsAllowedMidBattle();
bool16 Nuzlocke_ArePokemonCentersAllowed();
bool16 Nuzlocke_IsBlackoutAllowed();
bool16 Nuzlocke_IsForcedToNickname();
bool16 Nuzlocke_ShouldBalanceLevels();
bool16 Nuzlocke_IsBuyingItemsAllowed();
bool16 Nuzlocke_IsDaycareAllowed();
bool16 Nuzlocke_IsExpShareAllowed();
bool16 Nuzlocke_AreHeldItemsAllowed();
bool16 Nuzlocke_IsStarterRandomized();
bool16 Nuzlocke_AreWildMonsRandomized();
bool16 Nuzlocke_AreTrainersRandomized();
bool16 Nuzlocke_AreWildAttacksRandomized();
bool16 Nuzlocke_AreTrainerAttacksRandomized();
bool16 Nuzlocke_AreGendersRandomized();
u16 Nuzlocke_RandomSpecies(u16 baseSpecies, u8 wildMonIndex, struct MapHeader *mapHeader);
u16 Nuzlocke_RandomAttack(u16 baseSpecies, u8 monIndex, u8 extra);
u8 Nuzlocke_RandomGender(u16 species, u16 personality);
u8 Nuzlocke_CaptureMode();
bool16 Nuzlocke_SpecificStarter();
u16 Nuzlocke_GetStarterMon(u16);

void CB2_InitNuzlockeMenu(void);

#endif // GUARD_NUZLOCKE_H