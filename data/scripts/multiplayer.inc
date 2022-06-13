Coop_EventScript_SelectAction:: @ Menu when coop is active
	message Coop_SelectAction_Text
	waitmessage
	multichoice 0, 0, MULTICHOICE_TRADE_COLOSSEUM_2, FALSE
	switch VAR_RESULT
	case 0, Coop_EventScript_InitTrade @ Trade, should ahve code to send trade request to player 2
	case 1, Coop_EventScript_InitBattle @ Battle, should have code to send battle request to player 2
	case 2, Coop_EventScript_Teleport @ TODO routine to teleport to other player
	case 3, CableClub_EventScript_RecordCorner @ just an end command
	end

Coop_EventScript_InitTrade::
    end

Coop_EventScript_InitBattle::
    end

Coop_EventScript_Teleport::
    end


Coop_SelectAction_Text::
	.string "Select an action.$"