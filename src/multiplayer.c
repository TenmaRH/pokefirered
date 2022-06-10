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

static void Task_CoopLinkupAwaitConnection(u8 taskId);
static void Task_CoopMasterAwaitSlave(u8 taskId);
static void Task_CoopMasterConfirm(u8 taskId);
static void Task_CoopMasterSuccess(u8 taskId);
static void Task_CoopSlaveAwaitMaster(u8 taskId);
static void LinkSuccess();
static void LinkAbort();

void Task_CoopLinkupStart(u8 taskId)
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
        gTasks[taskId].func = Task_CoopLinkupAwaitConnection;
    }
    data[0]++;
}

static void Task_CoopLinkupAwaitConnection(u8 taskId)
{
    ShowFieldAutoScrollMessage(gText_WaitCoopMultiPlayer);
    if (IsLinkMaster())
        gTasks[taskId].func = Task_CoopMasterAwaitSlave;
    else
        gTasks[taskId].func = Task_CoopSlaveAwaitMaster;
}

static void Task_CoopMasterAwaitSlave(u8 taskId)
{
    s16 *data = gTasks[taskId].data;
    s32 linkPlayerCount = GetLinkPlayerCount_2();

    if (textbox_any_visible() == 0) // if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
    {
        if (gMain.heldKeys & B_BUTTON)
        {         
            LinkAbort();
            DestroyTask(taskId);
        }
        if (linkPlayerCount != COOP_PLAYERS)
        {
            return;
        }
        ShowFieldAutoScrollMessage(gText_StartCoopMultiPlayer);
        gTasks[taskId].func = Task_CoopMasterConfirm;
    }
}

static void Task_CoopMasterConfirm(u8 taskId)
{
    if (textbox_any_visible() == 0) // if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN)
    {
        if (gMain.heldKeys & A_BUTTON)
        {
            CheckShouldAdvanceLinkState();
            gTasks[taskId].func = Task_CoopMasterSuccess;
        }
    }
}

static void Task_CoopMasterSuccess(u8 taskId)
{
    gSpecialVar_Result = GetLinkPlayerDataExchangeStatusTimed(COOP_PLAYERS, COOP_PLAYERS);
    if (gMain.heldKeys & B_BUTTON) // ???
    {
        LinkAbort();
        DestroyTask(taskId);
    }
    else if (gSpecialVar_Result == 1) // LINKUP_SUCCESS
    {
        LinkSuccess();
        DestroyTask(taskId);
    }
}

static void Task_CoopSlaveAwaitMaster(u8 taskId)
{
    if (textbox_any_visible() == 0) // if (GetFieldMessageBoxMode() == FIELD_MESSAGE_BOX_HIDDEN) // esto no se si esta bien...
    {
        if (gMain.heldKeys & B_BUTTON)
        {
            LinkAbort();
            DestroyTask(taskId);
        }
        else 
        {
            gSpecialVar_Result = GetLinkPlayerDataExchangeStatusTimed(COOP_PLAYERS, COOP_PLAYERS);
            if (gSpecialVar_Result == 0) // LINKUP_ONGOING
            {
                return;
            }
            else if (gSpecialVar_Result == 1) // LINKUP_SUCCESS
            {
                LinkSuccess();
                DestroyTask(taskId);
            }
        }
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



