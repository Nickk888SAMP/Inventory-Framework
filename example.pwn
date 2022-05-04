#include <a_samp>
#define NI_DEBUG
#include "ninventory.inc"

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
	ITEM_TYPE_WEAPON
}
main() { AddPlayerClass(0, 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0); }

public OnGameModeInit()
{
	CreateItems();
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
	GivePlayerItem(playerid, GetItemFromName("Ammunition (Desert Eagle)"), 500);
	GivePlayerItem(playerid, GetItemFromName("Ammunition (Minigun)"), 500);

	for(new i; i < 50; i++)
	{
		GivePlayerItem(playerid, GetRandomItem(), 1);
	}
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	if(!strcmp("/eq", cmdtext, true))
	{
		LOOP_PLAYER_ITEMS(playerid, i)
		{
			printf("ItemName: %s | Amount: %i", GetItemName(GetPlayerItemItem(playerid, i)), GetPlayerItemAmount(playerid, i));
		}
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
	item = CreateItem("Ammunition (Desert Eagle)", "Converts enemies to gouda cheese.", 500, ITEM_TYPE_WEAPON, 362);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_MINIGUN);

	item = CreateItem("Ammunition (Minigun)", "Converts enemies to gouda cheese.", 500, ITEM_TYPE_WEAPON, 362);
	SetItemIntData(item, ITEM_DATA_WEAPONID, WEAPON_MINIGUN);
	return 1;
}
