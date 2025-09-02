#include "database.h"

/* .sdata */
extern int databaseInitialized;

INCLUDE_ASM("asm/nonmatchings/prlib/database", __16PrObjectDatabase);

INCLUDE_ASM("asm/nonmatchings/prlib/database", _$_16PrObjectDatabase);

void PrObjectDatabase::Initialize() {
    databaseInitialized = 1;
}

INCLUDE_ASM("asm/nonmatchings/prlib/database", Cleanup__16PrObjectDatabase);

INCLUDE_ASM("asm/nonmatchings/prlib/database", CreateScene__16PrObjectDatabaseP13sceGsDrawEnv1PCcUi);

INCLUDE_ASM("asm/nonmatchings/prlib/database", DeleteScene__16PrObjectDatabaseP13PrSceneObject);

INCLUDE_ASM("asm/nonmatchings/prlib/database", _GLOBAL_$D$prObjectDatabase);

INCLUDE_ASM("asm/nonmatchings/prlib/database", _GLOBAL_$I$prObjectDatabase);

/* objectset.h */
INCLUDE_ASM("asm/nonmatchings/prlib/database", _$_t11PrObjectSet1Z13SpcFileHeader);

INCLUDE_ASM("asm/nonmatchings/prlib/database", _$_t11PrObjectSet1Z13SpaFileHeader);

INCLUDE_ASM("asm/nonmatchings/prlib/database", _$_t11PrObjectSet1Z13PrSceneObject);
