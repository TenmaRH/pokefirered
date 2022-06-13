#include "global.h"
#include "main.h"
#include "multiplayer.h"
#include "link.h"
#include "strings.h"
#include "task.h"
#include "event_data.h"
#include "new_menu_helpers.h"
#include "event_object_movement.h"
#include "script.h"
#include "field_message_box.h"
#include "overworld.h"

static const u8 StringGameCode[] = _("DUMMY");
static const u8 gText_Congrats[] = _("Congrats!!!"); // test

static void Task_CoopLinkupStart(u8 taskId);
static void Task_CoopMasterAwaitSlave(u8 taskId);
static void Task_CoopMasterConfirm(u8 taskId);
static void Task_CoopAwaitPartnerReady(u8 taskId);
static void LinkSuccess();
static void LinkAbort();

// esperamos a que salga todo el mensaje antes de empezar la conexion
void Task_CoopStart(u8 taskId)
{
    if (textbox_any_visible() == 0) // if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
        gTasks[taskId].func = Task_CoopLinkupStart;
}

// iniciamos conexion
static void Task_CoopLinkupStart(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    if (data[0] == 0)
    {
        gLinkType = LINKTYPE_COOP;
        OpenLinkTimed();
        ResetLinkPlayerCount();
        ResetLinkPlayers();
    }
    else if (data[0] > 9)
    {
        gTasks[taskId].func = IsLinkMaster() ? Task_CoopMasterAwaitSlave : Task_CoopAwaitPartnerReady;
    }
    data[0]++;
}

// 1P espera a que 2P ya haya iniciado conexion
static void Task_CoopMasterAwaitSlave(u8 taskId)
{
    if (JOY_NEW(B_BUTTON))
    {         
        LinkAbort();
        DestroyTask(taskId);
    }
    else if (GetLinkPlayerCount_2() == COOP_PLAYERS)
    {
        ShowFieldAutoScrollMessage(gText_StartCoopMultiPlayer);
        gTasks[taskId].func = Task_CoopMasterConfirm;
    }
}

// 2P ya se ha conectado, 1P debe pulsar A para empezar
static void Task_CoopMasterConfirm(u8 taskId)
{
    if (textbox_any_visible() == 0 && JOY_NEW(A_BUTTON)) // if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN && JOY_NEW(A_BUTTON))
    {
        CheckShouldAdvanceLinkState();
        gTasks[taskId].func = Task_CoopAwaitPartnerReady;
    }
}

// 2P espera a que 1P este listo, 1P siempre llega el ultimo
static void Task_CoopAwaitPartnerReady(u8 taskId)
{
    if (GetLinkPlayerDataExchangeStatusTimed(COOP_PLAYERS, COOP_PLAYERS) == EXCHANGE_COMPLETE) // LINKUP_SUCCESS
    {
        LinkSuccess();
        DestroyTask(taskId);
    }
    else if (JOY_NEW(B_BUTTON))
    {
        LinkAbort();
        DestroyTask(taskId);
    }
}

// TODO
static void LinkSuccess()
{
    gFieldLinkPlayerCount = GetLinkPlayerCount_2();
    gLocalLinkPlayerId = GetMultiplayerId();
    sub_800A900(gFieldLinkPlayerCount); // SaveLinkPlayers(gFieldLinkPlayerCount);

    //TransferInitialData(0xBABA);

    //ClearDialogWindowAndFrame(0, TRUE);
    //UnfreezeObjectEvents();
    //ScriptContext2_Disable();

    //SetMainCallback2(CB2_ReturnToFieldCoopMultiplayer);

    ShowFieldAutoScrollMessage(gText_Congrats); // test
}

static void LinkAbort()
{
    gLinkType = 0;
    ClearDialogWindowAndFrame(0, TRUE);
    UnfreezeObjectEvents();
    ScriptContext2_Disable();
    CloseLink();
}

