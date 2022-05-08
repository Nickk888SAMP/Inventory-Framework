#include <a_samp>
#define NI_DEBUG
#define MAX_PLAYER_ITEMS 15
#include "ninventory.inc"

// Textdraws
new Text:PublicTD[5];
new Text:TD_InvExitBtn;
new Text:TD_InvUseBtn;
new Text:TD_InvDropBtn;
new PlayerText:TD_InvItemPreview[MAX_PLAYERS];
new PlayerText:TD_InvItemDesc[MAX_PLAYERS];
new PlayerText:TD_InvItemName[MAX_PLAYERS][MAX_PLAYER_ITEMS];
new PlayerText:TD_InvItemAmount[MAX_PLAYERS][MAX_PLAYER_ITEMS];

// Hold the selected Item
new PlayerItem:SelectedInventoryItem[MAX_PLAYERS] = PlayerItem:-1;

// Enumerators
enum
{
	ITEM_DATA_CATEGORY = 1,
	ITEM_DATA_DEMANDGAIN,
	ITEM_DATA_SUB_CATEGORY,
	ITEM_DATA_WEAPONID
}

enum
{
	ITEM_TYPE_FOOD = 1,
	ITEM_TYPE_DRINK,
	ITEM_TYPE_ALCOHOL,
	ITEM_TYPE_WEAPON,
	ITEM_TYPE_AMMO
}

// Init
main() { AddPlayerClass(0, 0.0, 0.0, 3.0, 0.0, 0, 0, 0, 0, 0, 0); }

public OnGameModeInit()
{
	CreateInventoryTextDraws();
	CreateItems();
	return 1;
}

public OnGameModeExit()
{
	DestroyInventoryTextDraws();
	return 1;
}

public OnPlayerConnect(playerid)
{
	GivePlayerItem(playerid, GetItemFromName("Burger"), (random(5) + 1));
	GivePlayerItem(playerid, GetItemFromName("Pizza"), (random(5) + 1));
	GivePlayerItem(playerid, GetItemFromName("Orange juice"), (random(5) + 1));
	GivePlayerItem(playerid, GetItemFromName("Apple juice"), (random(5) + 1));
	GivePlayerItem(playerid, GetItemFromName("Wine bottle"), (random(5) + 1));
	GivePlayerItem(playerid, GetItemFromName("Desert Eagle"), 1);
	GivePlayerItem(playerid, GetItemFromName("Grenade"), 8);
	GivePlayerItem(playerid, GetItemFromName("Minigun"), 1);
	GivePlayerItem(playerid, GetItemFromName("Ammunition (Desert Eagle)"), 285);
	GivePlayerItem(playerid, GetItemFromName("Ammunition (Minigun)"), 330);

	CreateInventoryPlayerTextDraws(playerid);
	return 1;
}

public OnPlayerClickTextDraw(playerid, Text:clickedid)
{
	if(clickedid == INVALID_TEXT_DRAW )
	{
		ShowPlayerInventory(playerid, false);
		return 1;
	}
	else if(clickedid == TD_InvUseBtn)
	{
		UseItem(playerid, SelectedInventoryItem[playerid]);
		return 1;
	}
	return 0;
}

public OnPlayerDisconnect(playerid, reason)
{
	DestroyInventoryPlayerTextDraws(playerid);
	return 1;
}

public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
	for(new i; i < MAX_PLAYER_ITEMS; i++)
	{
		if(playertextid == TD_InvItemName[playerid][i])
		{
			SelectInventoryItem(playerid, i);
			return 1;
		}
	}
	return 0;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp("/eq", cmdtext, true))
	{
		ShowPlayerInventory(playerid, true);
		return 1;
	}
	return 0;
}

CreateItems()
{
	new Item:item;

	/* Food */
	item = CreateItem("Burger", "A delicious burger from Cluckin' Bell.", 4, ITEM_TYPE_FOOD, 2768);
	SetItemFloatData(item, ITEM_DATA_DEMANDGAIN, 24.554);

	item = CreateItem("Pizza", "A medium sized pizza from 'The Well Stacked Pizza'.", 2, ITEM_TYPE_FOOD, 2814);
	SetItemFloatData(item, ITEM_DATA_DEMANDGAIN, 42.0015);


	/* Drink */
	item = CreateItem("Orange juice", "A small carton with artificialy flavoured orange juice.", 5, ITEM_TYPE_DRINK, 19563);
	SetItemFloatData(item, ITEM_DATA_DEMANDGAIN, 28.0154);

	item = CreateItem("Apple juice", "A small carton with artificialy flavoured apple juice.", 5, ITEM_TYPE_DRINK, 19564);
	SetItemFloatData(item, ITEM_DATA_DEMANDGAIN, 28.0154);

	item = CreateItem("Wine bottle", "A bottle of cheap wine with 75\% of alcohol.", 2, ITEM_TYPE_DRINK, 1509);
	SetItemIntData(item, ITEM_DATA_SUB_CATEGORY, ITEM_TYPE_ALCOHOL);
	SetItemFloatData(item, ITEM_DATA_DEMANDGAIN, 19.7618);


	/* Weapons */
	item = CreateItem("Desert Eagle", "A heavy pistol like weapon. It hurts a lot!", 1, ITEM_TYPE_WEAPON, 348);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_DEAGLE);

	item = CreateItem("Grenade", "Makes hole in everything, even in your dreams.", 12, ITEM_TYPE_WEAPON, 342);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_GRENADE);

	item = CreateItem("Minigun", "Converts enemies to gouda cheese.", 1, ITEM_TYPE_WEAPON, 362);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_MINIGUN);


	/* Ammunition */
	item = CreateItem("Ammunition (Desert Eagle)", "Converts enemies to gouda cheese.", 500, ITEM_TYPE_AMMO, 2040);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_MINIGUN);

	item = CreateItem("Ammunition (Minigun)", "Converts enemies to gouda cheese.", 500, ITEM_TYPE_AMMO, 2040);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_MINIGUN);
	return 1;
}

UseItem(playerid, PlayerItem:pitem)
{
	switch(GetPlayerItemCategory(playerid, pitem))
	{
		case ITEM_TYPE_FOOD:
		{
			SendClientMessage(playerid, -1, "You are eating something.");
			TakePlayerItem(playerid, pitem, 1);
		}
		case ITEM_TYPE_DRINK:
		{
			if(GetPlayerItemIntData(playerid, pitem, ITEM_DATA_SUB_CATEGORY) == ITEM_TYPE_ALCOHOL)
			{
				SendClientMessage(playerid, -1, "You are drinking alcohol, you are now drunk.");
				SetPlayerDrunkLevel(playerid, 8000);
			}
			else
			{
				SendClientMessage(playerid, -1, "You are drinking something.");
			}
			TakePlayerItem(playerid, pitem, 1);
		}
		case ITEM_TYPE_WEAPON:
		{
			SendClientMessage(playerid, -1, "This is a weapon.");
		}
		case ITEM_TYPE_AMMO:
		{
			SendClientMessage(playerid, -1, "This is ammunition.");
		}
	}
	ShowPlayerInventory(playerid, true);
	return 1;
}

/* Inventory GUI Manager */

ShowPlayerInventory(playerid, bool:show)
{
	if(show)
	{
		TextDrawShowForPlayer(playerid, PublicTD[0]);
		TextDrawShowForPlayer(playerid, PublicTD[1]);
		TextDrawShowForPlayer(playerid, PublicTD[2]);
		TextDrawShowForPlayer(playerid, PublicTD[3]);
		TextDrawShowForPlayer(playerid, PublicTD[4]);
		TextDrawShowForPlayer(playerid, TD_InvExitBtn);
		SelectTextDraw(playerid, 0xFF0000FF);
		SelectInventoryItem(playerid, -1);
	}
	else
	{
		TextDrawHideForPlayer(playerid, PublicTD[0]);
		TextDrawHideForPlayer(playerid, PublicTD[1]);
		TextDrawHideForPlayer(playerid, PublicTD[2]);
		TextDrawHideForPlayer(playerid, PublicTD[3]);
		TextDrawHideForPlayer(playerid, PublicTD[4]);
		TextDrawHideForPlayer(playerid, TD_InvExitBtn);
		TextDrawHideForPlayer(playerid, TD_InvUseBtn);
		TextDrawHideForPlayer(playerid, TD_InvDropBtn);
		PlayerTextDrawHide(playerid, TD_InvItemPreview[playerid]);
		PlayerTextDrawHide(playerid, TD_InvItemDesc[playerid]);
		HideInventoryItems(playerid);
	}
	return 1;
}

SelectInventoryItem(playerid, index)
{
	new PlayerItem:pitem = GetPlayerItemFromIndex(playerid, index);
	if(IsValidPlayerItem(playerid, pitem))
	{
		if(SelectedInventoryItem[playerid] == pitem)
			return 0;
		SelectedInventoryItem[playerid] = pitem;
		PlayerTextDrawSetPreviewModel(playerid, TD_InvItemPreview[playerid], GetPlayerItemModelID(playerid, pitem));
		PlayerTextDrawSetString(playerid, TD_InvItemDesc[playerid], GetPlayerItemDescription(playerid, pitem));
		//
		PlayerTextDrawShow(playerid, TD_InvItemPreview[playerid]);
		PlayerTextDrawShow(playerid, TD_InvItemDesc[playerid]);
		TextDrawShowForPlayer(playerid, TD_InvUseBtn);
		TextDrawShowForPlayer(playerid, TD_InvDropBtn);
		//
		ShowInventoryItems(playerid);
		printf("Index: %i | Item Name: %s", _:SelectedInventoryItem[playerid], GetPlayerItemName(playerid, pitem));
	}
	else
	{
		SelectedInventoryItem[playerid] = PlayerItem:-1;
		TextDrawHideForPlayer(playerid, TD_InvUseBtn);
		TextDrawHideForPlayer(playerid, TD_InvDropBtn);
		PlayerTextDrawHide(playerid, TD_InvItemPreview[playerid]);
		PlayerTextDrawHide(playerid, TD_InvItemDesc[playerid]);
		ShowInventoryItems(playerid);
	}
	return 1;
}

ShowInventoryItems(playerid)
{
	new count;
	new itemString[64];
	HideInventoryItems(playerid);
	LOOP_PLAYER_ITEMS(playerid, i)
	{
		PlayerTextDrawSetString(playerid, TD_InvItemName[playerid][count], GetPlayerItemName(playerid, i));
		format(itemString, sizeof itemString, "%i/%i", GetPlayerItemAmount(playerid, i), GetPlayerItemStackSize(playerid, i));
		PlayerTextDrawSetString(playerid, TD_InvItemAmount[playerid][count], itemString);
		//
		if(SelectedInventoryItem[playerid] == i)
		{
			PlayerTextDrawColor(playerid, TD_InvItemName[playerid][count], 0xFF0000FF);
		}
		else
		{
			PlayerTextDrawColor(playerid, TD_InvItemName[playerid][count], 0xFFFFFFFF);
		}
		//
		PlayerTextDrawShow(playerid, TD_InvItemName[playerid][count]);
		PlayerTextDrawShow(playerid, TD_InvItemAmount[playerid][count]);
		count++;
	}
	return 1;
}

HideInventoryItems(playerid)
{
	for(new i; i < MAX_PLAYER_ITEMS; i++)
	{
		PlayerTextDrawHide(playerid, TD_InvItemName[playerid][i]);
		PlayerTextDrawHide(playerid, TD_InvItemAmount[playerid][i]);
	}
	return 1;
}

/* TextDraws */

CreateInventoryTextDraws()
{
	PublicTD[0] = TextDrawCreate(152.000000, 157.000000, "_");
	TextDrawFont(PublicTD[0], 1);
	TextDrawLetterSize(PublicTD[0], 0.600000, 26.099977);
	TextDrawTextSize(PublicTD[0], 494.000000, 17.000000);
	TextDrawSetOutline(PublicTD[0], 1);
	TextDrawSetShadow(PublicTD[0], 0);
	TextDrawAlignment(PublicTD[0], 1);
	TextDrawColor(PublicTD[0], -1);
	TextDrawBackgroundColor(PublicTD[0], 255);
	TextDrawBoxColor(PublicTD[0], 229);
	TextDrawUseBox(PublicTD[0], 1);
	TextDrawSetProportional(PublicTD[0], 1);
	TextDrawSetSelectable(PublicTD[0], 0);

	PublicTD[1] = TextDrawCreate(152.000000, 157.000000, "_");
	TextDrawFont(PublicTD[1], 1);
	TextDrawLetterSize(PublicTD[1], 0.600000, 0.725000);
	TextDrawTextSize(PublicTD[1], 494.000000, 17.000000);
	TextDrawSetOutline(PublicTD[1], 1);
	TextDrawSetShadow(PublicTD[1], 0);
	TextDrawAlignment(PublicTD[1], 1);
	TextDrawColor(PublicTD[1], -1);
	TextDrawBackgroundColor(PublicTD[1], 255);
	TextDrawBoxColor(PublicTD[1], 1687547331);
	TextDrawUseBox(PublicTD[1], 1);
	TextDrawSetProportional(PublicTD[1], 1);
	TextDrawSetSelectable(PublicTD[1], 0);

	PublicTD[2] = TextDrawCreate(277.000000, 168.000000, "_");
	TextDrawFont(PublicTD[2], 1);
	TextDrawLetterSize(PublicTD[2], 0.600000, 24.899955);
	TextDrawTextSize(PublicTD[2], 274.500000, 17.000000);
	TextDrawSetOutline(PublicTD[2], 1);
	TextDrawSetShadow(PublicTD[2], 0);
	TextDrawAlignment(PublicTD[2], 1);
	TextDrawColor(PublicTD[2], -1);
	TextDrawBackgroundColor(PublicTD[2], 255);
	TextDrawBoxColor(PublicTD[2], 1687547331);
	TextDrawUseBox(PublicTD[2], 1);
	TextDrawSetProportional(PublicTD[2], 1);
	TextDrawSetSelectable(PublicTD[2], 0);

	PublicTD[3] = TextDrawCreate(152.000000, 291.000000, "_");
	TextDrawFont(PublicTD[3], 1);
	TextDrawLetterSize(PublicTD[3], 0.600000, -0.324999);
	TextDrawTextSize(PublicTD[3], 273.000000, 17.000000);
	TextDrawSetOutline(PublicTD[3], 1);
	TextDrawSetShadow(PublicTD[3], 0);
	TextDrawAlignment(PublicTD[3], 1);
	TextDrawColor(PublicTD[3], -1);
	TextDrawBackgroundColor(PublicTD[3], 255);
	TextDrawBoxColor(PublicTD[3], 1687547331);
	TextDrawUseBox(PublicTD[3], 1);
	TextDrawSetProportional(PublicTD[3], 1);
	TextDrawSetSelectable(PublicTD[3], 0);

	PublicTD[4] = TextDrawCreate(153.000000, 156.000000, "INVENTORY");
	TextDrawFont(PublicTD[4], 1);
	TextDrawLetterSize(PublicTD[4], 0.229166, 0.949998);
	TextDrawTextSize(PublicTD[4], 400.000000, 17.000000);
	TextDrawSetOutline(PublicTD[4], 1);
	TextDrawSetShadow(PublicTD[4], 0);
	TextDrawAlignment(PublicTD[4], 1);
	TextDrawColor(PublicTD[4], -1);
	TextDrawBackgroundColor(PublicTD[4], 255);
	TextDrawBoxColor(PublicTD[4], 50);
	TextDrawUseBox(PublicTD[4], 0);
	TextDrawSetProportional(PublicTD[4], 1);
	TextDrawSetSelectable(PublicTD[4], 0);

	TD_InvExitBtn = TextDrawCreate(488.000000, 156.000000, "X");
	TextDrawFont(TD_InvExitBtn, 1);
	TextDrawLetterSize(TD_InvExitBtn, 0.312498, 0.850000);
	TextDrawTextSize(TD_InvExitBtn, 7.500000, 10.500000);
	TextDrawSetOutline(TD_InvExitBtn, 0);
	TextDrawSetShadow(TD_InvExitBtn, 0);
	TextDrawAlignment(TD_InvExitBtn, 2);
	TextDrawColor(TD_InvExitBtn, -16776961);
	TextDrawBackgroundColor(TD_InvExitBtn, 255);
	TextDrawBoxColor(TD_InvExitBtn, 200);
	TextDrawUseBox(TD_InvExitBtn, 0);
	TextDrawSetProportional(TD_InvExitBtn, 1);
	TextDrawSetSelectable(TD_InvExitBtn, 1);

	TD_InvUseBtn = TextDrawCreate(213.000000, 365.200012, "Use");
	TextDrawFont(TD_InvUseBtn, 2);
	TextDrawLetterSize(TD_InvUseBtn, 0.204163, 1.250000);
	TextDrawTextSize(TD_InvUseBtn, 12.500000, 121.500000);
	TextDrawSetOutline(TD_InvUseBtn, 1);
	TextDrawSetShadow(TD_InvUseBtn, 0);
	TextDrawAlignment(TD_InvUseBtn, 2);
	TextDrawColor(TD_InvUseBtn, -1);
	TextDrawBackgroundColor(TD_InvUseBtn, 255);
	TextDrawBoxColor(TD_InvUseBtn, 1687547336);
	TextDrawUseBox(TD_InvUseBtn, 1);
	TextDrawSetProportional(TD_InvUseBtn, 1);
	TextDrawSetSelectable(TD_InvUseBtn, 1);

	TD_InvDropBtn = TextDrawCreate(213.000000, 381.000000, "Drop");
	TextDrawFont(TD_InvDropBtn, 2);
	TextDrawLetterSize(TD_InvDropBtn, 0.204163, 1.250000);
	TextDrawTextSize(TD_InvDropBtn, 12.500000, 121.500000);
	TextDrawSetOutline(TD_InvDropBtn, 1);
	TextDrawSetShadow(TD_InvDropBtn, 0);
	TextDrawAlignment(TD_InvDropBtn, 2);
	TextDrawColor(TD_InvDropBtn, -1);
	TextDrawBackgroundColor(TD_InvDropBtn, 255);
	TextDrawBoxColor(TD_InvDropBtn, 1687547336);
	TextDrawUseBox(TD_InvDropBtn, 1);
	TextDrawSetProportional(TD_InvDropBtn, 1);
	TextDrawSetSelectable(TD_InvDropBtn, 1);
	return 1;
}

CreateInventoryPlayerTextDraws(playerid)
{
	TD_InvItemPreview[playerid] = CreatePlayerTextDraw(playerid, 150.000000, 166.000000, "Preview_Model");
	PlayerTextDrawFont(playerid, TD_InvItemPreview[playerid], 5);
	PlayerTextDrawLetterSize(playerid, TD_InvItemPreview[playerid], 0.600000, 2.000000);
	PlayerTextDrawTextSize(playerid, TD_InvItemPreview[playerid], 125.000000, 125.000000);
	PlayerTextDrawSetOutline(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawSetShadow(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawAlignment(playerid, TD_InvItemPreview[playerid], 1);
	PlayerTextDrawColor(playerid, TD_InvItemPreview[playerid], -1);
	PlayerTextDrawBackgroundColor(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawBoxColor(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawUseBox(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawSetProportional(playerid, TD_InvItemPreview[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, TD_InvItemPreview[playerid], 0);
	PlayerTextDrawSetPreviewModel(playerid, TD_InvItemPreview[playerid], 2768);
	PlayerTextDrawSetPreviewRot(playerid, TD_InvItemPreview[playerid], -25.000000, 0.000000, -157.000000, 1.000000);
	PlayerTextDrawSetPreviewVehCol(playerid, TD_InvItemPreview[playerid], 1, 1);

	TD_InvItemDesc[playerid] = CreatePlayerTextDraw(playerid, 212.000000, 294.000000, "A small carton with artificialy flavoured orange juice.");
	PlayerTextDrawFont(playerid, TD_InvItemDesc[playerid], 1);
	PlayerTextDrawLetterSize(playerid, TD_InvItemDesc[playerid], 0.216664, 1.250000);
	PlayerTextDrawTextSize(playerid, TD_InvItemDesc[playerid], 270.000000, 116.000000);
	PlayerTextDrawSetOutline(playerid, TD_InvItemDesc[playerid], 1);
	PlayerTextDrawSetShadow(playerid, TD_InvItemDesc[playerid], 0);
	PlayerTextDrawAlignment(playerid, TD_InvItemDesc[playerid], 2);
	PlayerTextDrawColor(playerid, TD_InvItemDesc[playerid], -1);
	PlayerTextDrawBackgroundColor(playerid, TD_InvItemDesc[playerid], 255);
	PlayerTextDrawBoxColor(playerid, TD_InvItemDesc[playerid], -206);
	PlayerTextDrawUseBox(playerid, TD_InvItemDesc[playerid], 0);
	PlayerTextDrawSetProportional(playerid, TD_InvItemDesc[playerid], 1);
	PlayerTextDrawSetSelectable(playerid, TD_InvItemDesc[playerid], 0);

	for(new i; i < MAX_PLAYER_ITEMS; i++)
	{
		TD_InvItemName[playerid][i] = CreatePlayerTextDraw(playerid, 280.000000, 170.000000 + (i * 15), "Ammunition (Desert Eagle)");
		PlayerTextDrawFont(playerid, TD_InvItemName[playerid][i], 1);
		PlayerTextDrawLetterSize(playerid, TD_InvItemName[playerid][i], 0.204163, 0.999997);
		PlayerTextDrawTextSize(playerid, TD_InvItemName[playerid][i], 443.000000, 12.500000);
		PlayerTextDrawSetOutline(playerid, TD_InvItemName[playerid][i], 1);
		PlayerTextDrawSetShadow(playerid, TD_InvItemName[playerid][i], 0);
		PlayerTextDrawAlignment(playerid, TD_InvItemName[playerid][i], 1);
		PlayerTextDrawColor(playerid, TD_InvItemName[playerid][i], -1);
		PlayerTextDrawBackgroundColor(playerid, TD_InvItemName[playerid][i], 255);
		PlayerTextDrawBoxColor(playerid, TD_InvItemName[playerid][i], -1061109560);
		PlayerTextDrawUseBox(playerid, TD_InvItemName[playerid][i], 1);
		PlayerTextDrawSetProportional(playerid, TD_InvItemName[playerid][i], 1);
		PlayerTextDrawSetSelectable(playerid, TD_InvItemName[playerid][i], 1);

		TD_InvItemAmount[playerid][i] = CreatePlayerTextDraw(playerid, 447.000000, 170.000000 + (i * 15), "100/100");
		PlayerTextDrawFont(playerid, TD_InvItemAmount[playerid][i], 1);
		PlayerTextDrawLetterSize(playerid, TD_InvItemAmount[playerid][i], 0.204163, 0.999997);
		PlayerTextDrawTextSize(playerid, TD_InvItemAmount[playerid][i], 492.500000, 12.500000);
		PlayerTextDrawSetOutline(playerid, TD_InvItemAmount[playerid][i], 1);
		PlayerTextDrawSetShadow(playerid, TD_InvItemAmount[playerid][i], 0);
		PlayerTextDrawAlignment(playerid, TD_InvItemAmount[playerid][i], 1);
		PlayerTextDrawColor(playerid, TD_InvItemAmount[playerid][i], -1);
		PlayerTextDrawBackgroundColor(playerid, TD_InvItemAmount[playerid][i], 255);
		PlayerTextDrawBoxColor(playerid, TD_InvItemAmount[playerid][i], 1687547336);
		PlayerTextDrawUseBox(playerid, TD_InvItemAmount[playerid][i], 1);
		PlayerTextDrawSetProportional(playerid, TD_InvItemAmount[playerid][i], 1);
		PlayerTextDrawSetSelectable(playerid, TD_InvItemAmount[playerid][i], 0);
	}
	return 1;
}

DestroyInventoryPlayerTextDraws(playerid)
{
	PlayerTextDrawDestroy(playerid, TD_InvItemPreview[playerid]);
	PlayerTextDrawDestroy(playerid, TD_InvItemDesc[playerid]);

	for(new i; i < MAX_PLAYER_ITEMS; i++)
	{
		PlayerTextDrawDestroy(playerid, TD_InvItemName[playerid][i]);
		PlayerTextDrawDestroy(playerid, TD_InvItemAmount[playerid][i]);
	}
	return 1;
}

DestroyInventoryTextDraws()
{
	TextDrawDestroy(PublicTD[0]);
	TextDrawDestroy(PublicTD[1]);
	TextDrawDestroy(PublicTD[2]);
	TextDrawDestroy(PublicTD[3]);
	TextDrawDestroy(PublicTD[4]);
	TextDrawDestroy(TD_InvExitBtn);
	TextDrawDestroy(TD_InvUseBtn);
	TextDrawDestroy(TD_InvDropBtn);
	return 1;
}