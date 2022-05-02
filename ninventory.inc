/************************************************************
                        LICENSE

Script: NInventory
Type: Include
Version: Dev 1.1
Author: Nickk888
Date: 02.05.2022

Social Media:
YouTube: https://www.youtube.com/channel/UCvjbpjJXET1QS9Wa0_XVCuw
Facebook: https://www.facebook.com/Nickk888FP/

Functions_________________________


Callbacks_________________________
OnPlayerGetItem(playerid, Item:item, amount, remains);

Loops____________________________
LOOP_ITEMS(variableName) - Loops through the server items, returns item as tag "Item".
LOOP_PLAYER_ITEMS(playerid, variablName) - Loops through the player items, returns item as tag "PlayerItem".

*************************************************************/

#if defined _nitems_included
	#endinput
#endif
#define _nitems_included

#pragma library nitems

/************************************************************
                        INCLUDES
*************************************************************/

#include <YSI_Data\y_foreach>
#include <YSI_Coding\y_hooks>

/************************************************************
                        FORWARDS
*************************************************************/

forward OnPlayerGetItem(playerid, Item:item, amount, remains);

/************************************************************
                        DEFINITIONS
*************************************************************/

#if !defined MAX_SERVER_ITEMS
    #define MAX_SERVER_ITEMS 100
#endif

#if !defined MAX_PLAYER_ITEMS
    #define MAX_PLAYER_ITEMS 20
#endif

#define LOOP_ITEMS(%1) foreach(new Item:%1 : NI_I_ITEMS) 
#define LOOP_PLAYER_ITEMS(%1,%2) foreach(new PlayerItem:%2 : NI_I_PITEMS[%1]) 

/************************************************************
                        INTERNALS
*************************************************************/

#define NI_MAX_DATA_AMOUNT 20
#define NI_VALID_ITER_FREE(%1,%2) %2 >= 0 && %2 < Iter_End(%1)

/************************************************************
                        ITERATORS
*************************************************************/

new Iterator:NI_I_ITEMS<Item:MAX_SERVER_ITEMS>;
new Iterator:NI_I_PITEMS[MAX_PLAYERS]<PlayerItem:MAX_PLAYER_ITEMS>;

/************************************************************
                        ENUMERATORS
*************************************************************/

enum NI_E_ITEM // Stores Server Items
{
	bool:ni_i_valid,
	ni_i_itemid,
    ni_i_dbId,
	ni_i_name[128],
	ni_i_desc[128],
	ni_i_intData[NI_MAX_DATA_AMOUNT],
    ni_i_objModelId,
	ni_i_iconName[128],
	ni_i_stackSize,
	Float:ni_i_floatData[NI_MAX_DATA_AMOUNT]
};
new NI_Item[Item:MAX_SERVER_ITEMS][NI_E_ITEM];

enum NI_E_PLAYERITEM // Stores Player Items
{
    bool:ni_pi_valid,
	ni_pi_dbId,
	ni_pi_itemId,
	Item:ni_pi_item,
	ni_pi_amount
};	
new NI_PlayerItem[MAX_PLAYERS][PlayerItem:MAX_PLAYER_ITEMS][NI_E_PLAYERITEM];

/************************************************************
                        CALLBACKS
*************************************************************/

hook OnScriptInit()
{
    Iter_Init(Item:NI_I_PITEMS);
    return 1;
}

/************************************************************
                        FUNCTIONS
*************************************************************/

/* Creates new item returning the internal item ID */
stock Item:CreateItem(const name[])
{
    new Item:item = Item:Iter_Free(NI_I_ITEMS);
    if(NI_VALID_ITER_FREE(NI_I_ITEMS, _:item))
    {
        Iter_Add(NI_I_ITEMS, item);
        NI_Item[item][ni_i_valid] = true;
        NI_Item[item][ni_i_itemid] = (_:item + 1);
        NI_Item[item][ni_i_dbId] = 0;
        NI_Item[item][ni_i_objModelId] = 0;
        NI_Item[item][ni_i_stackSize] = 1;
        strpack(NI_Item[item][ni_i_name], name, 128);
        strpack(NI_Item[item][ni_i_desc], "", 128);
        strpack(NI_Item[item][ni_i_iconName], "", 128);
        for(new i; i < NI_MAX_DATA_AMOUNT; i++)
        {
            NI_Item[item][ni_i_intData][i] = 0;
            NI_Item[item][ni_i_floatData][i] = 0.0;
        }
        return item;
    }
    return Item:-1;
}

/* Gets item float data inside the Item */
forward Float:GetItemFloatData(Item:item, arrayId);
stock Float:GetItemFloatData(Item:item, arrayId)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return 0;

    return NI_Item[_:item][ni_i_floatData][arrayId];
}

/* Gets item intiger data inside the Item */
stock GetItemIntData(Item:item, arrayId)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return 0;

    return NI_Item[_:item][ni_i_intData][arrayId];
}

/* Gets item icon name inside the Item */
stock GetItemIconName(Item:item)
{
    new unpackedItemStr[128];
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[_:item][ni_i_iconName], 128);
    return unpackedItemStr;
}

/* Gets item description inside the Item */
stock GetItemDescription(Item:item)
{
    new unpackedItemStr[128];
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[_:item][ni_i_desc], 128);
    return unpackedItemStr;
}

/* Gets item name inside the Item */
stock GetItemName(Item:item)
{
    new unpackedItemStr[128];
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[item][ni_i_name], 128);
    return unpackedItemStr;
}

/* Gets item stack size inside the Item */
stock GetItemStackSize(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_stackSize];
}

/* Gets the item ID from player inventory */
stock GetPlayerItemID(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    return NI_PlayerItem[playerid][pitem][ni_pi_itemId];
}

/* Gets the item amount from player inventory */
stock GetPlayerItemAmount(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    return NI_PlayerItem[playerid][pitem][ni_pi_amount];
}

/* Gets the server item object from the player item object */
stock Item:GetPlayerItemServerItem(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return Item:-1;

    if(!IsValidItem(NI_PlayerItem[playerid][pitem][ni_pi_item]))
        return Item:-1;

    return NI_PlayerItem[playerid][pitem][ni_pi_item];
}

/* Gets Object Model ID inside the Item */
stock GetItemModelID(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_objModelId];
}

/* Gets Database ID inside the Item */
stock GetItemDatabaseID(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_dbId];
}

/* Gets Item ID inside the Item */
stock GetItemItemID(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_itemid];
}

/* Sets new item intiger data inside the Item */
stock SetItemFloatData(Item:item, arrayId, Float:floatData)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return 0;

    NI_Item[item][ni_i_floatData][arrayId] = floatData;
    return 1;
}

/* Sets new item intiger data inside the Item */
stock SetItemIntData(Item:item, arrayId, intData)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return 0;

    NI_Item[item][ni_i_intData][arrayId] = intData;
    return 1;
}

/* Sets new item icon name inside the Item */
stock SetItemIconName(Item:item, const newiconname[])
{
    if(!IsValidItem(item))
        return 0;

    strpack(NI_Item[item][ni_i_iconName], newiconname, 128);
    return 1;
}

/* Sets new item description inside the Item */
stock SetItemDescription(Item:item, const newdescription[])
{
    if(!IsValidItem(item))
        return 0;

    strpack(NI_Item[item][ni_i_desc], newdescription, 128);
    return 1;
}

/* Sets new item name inside the Item */
stock SetItemName(Item:item, const newname[])
{
    if(!IsValidItem(item))
        return 0;

    strpack(NI_Item[item][ni_i_name], newname, 128);
    return 1;
}

/* Sets new item stack size inside the Item */
stock SetItemStackSize(Item:item, newstacksize)
{
    if(!IsValidItem(item))
        return 0;

    NI_Item[item][ni_i_stackSize] = newstacksize;
    return 1;
}

/* Sets new Object Model ID inside the Item */
stock SetItemModelID(Item:item, newmodelid)
{
    if(!IsValidItem(item))
        return 0;

    NI_Item[item][ni_i_objModelId] = newmodelid;
    return 1;
}

/* Sets new Database ID inside the Item */
stock SetItemDatabaseID(Item:item, newdbid)
{
    if(!IsValidItem(item))
        return 0;

    NI_Item[item][ni_i_dbId] = newdbid;
    return 1;
}

/* Sets new Item ID inside the Item */
stock SetItemItemID(Item:item, newItemID)
{
    if(!IsValidItem(item))
        return 0;

    NI_Item[item][ni_i_itemid] = newItemID;
    return 1;
}

/* Checks if item exists and is valid */
stock IsValidItem(Item:item)
{
    if(_:item >= 0 && _:item < MAX_SERVER_ITEMS && NI_Item[item][ni_i_valid])
    {
        return 1;
    }
    return 0;
}

/* Gets Item object from itemid */
stock Item:GetItemFromItemID(itemid)
{
    foreach(new Item:i : NI_I_ITEMS)
    {
        if(NI_Item[i][ni_i_valid] && NI_Item[i][ni_i_itemid] == itemid)
        {
            return Item:i;
        }
    }
    return Item:-1;
}

/* Checks if item ID exists and is valid */
stock IsValidItemID(itemid)
{
    foreach(new Item:i : NI_I_ITEMS)
    {
        if(NI_Item[i][ni_i_valid] && NI_Item[i][ni_i_itemid] == itemid)
        {
            return 1;
        }
    }
    return 0;
}

/* Gets Player Item object from itemid */
stock PlayerItem:GetPlayerItemFromItemID(playerid, itemid)
{
    foreach(new PlayerItem:i : NI_I_PITEMS[playerid])
    {
        if(NI_PlayerItem[playerid][i][ni_pi_valid] && NI_PlayerItem[playerid][i][ni_pi_itemId] == itemid)
        {
            return PlayerItem:i;
        }
    }
    return PlayerItem:-1;
}

/* Checks if player item ID exists and is valid */
stock IsValidPlayerItem(playerid, PlayerItem:item)
{
    if(_:item >= 0 && _:item < MAX_SERVER_ITEMS && NI_PlayerItem[playerid][item][ni_pi_valid])
    {
        return 1;
    }
    return 0;
}


/* Checks if item exists using the name */
stock ItemNameExists(const name[], bool:ignorecase = true)
{
    foreach(new Item:i : NI_I_ITEMS)
    {
        if(!strcmp(NI_Item[i][ni_i_name], name, ignorecase))
        {
            return 1;
        }
    }
    return 0;
}

/* Removes server item */
stock RemoveItem(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    if(Iter_Remove(NI_I_ITEMS, item))
    {
        NI_Item[item][ni_i_valid] = false;
        return 1;
    }
    return 0;
}

/* Safely removes server item, mostly used in iterators */
stock SafeRemoveItem(Item:item, & Item:nextItem)
{
    if(!IsValidItem(item))
        return 0;

    new next;
    if(Iter_SafeRemove(NI_I_ITEMS, item, next))
    {
        nextItem = Item:next;
        NI_Item[item][ni_i_valid] = false;
        return 1;
    }
    return 0;
}

/* Removes player item */
stock RemovePlayerItem(playerid, PlayerItem:item)
{
    if(!IsValidPlayerItem(playerid, item))
        return 0;

    if(Iter_Remove(NI_I_PITEMS[playerid], item))
    {
        NI_PlayerItem[playerid][item][ni_pi_valid] = false;
        return 1;
    }
    return 0;
}

/* Safely removes player item, mostly used in iterators */
stock SafeRemovePlayerItem(playerid, PlayerItem:item, & PlayerItem:nextItem)
{
    if(!IsValidPlayerItem(playerid, item))
        return 0;

    new next;
    if(Iter_SafeRemove(NI_I_PITEMS[playerid], item, next))
    {
        nextItem = PlayerItem:next;
        NI_PlayerItem[playerid][item][ni_pi_valid] = false;
        return 1;
    }
    return 0;
}

/* Transfers Player Item from player A to player B */
stock TransferPlayerItemToPlayer(playeridA, playeridB, PlayerItem:item, amount)
{
    if(amount <= 0 || !IsPlayerConnected(playeridA) || !IsPlayerConnected(playeridB))
        return 0;

    if(!IsValidPlayerItem(playeridA, item))
        return 0;

    if(GetPlayerItemAmount(playeridA, item) >= amount)
    {
        GivePlayerItem(playeridB, GetPlayerItemServerItem(playeridA, item), amount);
        TakePlayerItem(playeridA, item, amount);
        return 1;
    }
    return 0;
}

/* Take player item with amount */
stock TakePlayerItem(playerid, PlayerItem:pitem, amount)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    if(GetPlayerItemAmount(playerid, pitem) >= amount)
    {
        NI_PlayerItem[playerid][pitem][ni_pi_amount] -= amount;
        if(NI_PlayerItem[playerid][pitem][ni_pi_amount] == 0)
        {
            RemovePlayerItem(playerid, pitem);
        }
    }
    return 1;
}

/* Savely takes player item with amount. Used in loops. */
stock SafeTakePlayerItem(playerid, PlayerItem:pitem, amount, & PlayerItem:nextItem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    if(GetPlayerItemAmount(playerid, pitem) >= amount)
    {
        NI_PlayerItem[playerid][pitem][ni_pi_amount] -= amount;
        if(NI_PlayerItem[playerid][pitem][ni_pi_amount] == 0)
        {
            SafeRemovePlayerItem(playerid, pitem, nextItem);
        }
        else nextItem = PlayerItem:Iter_Next(NI_I_PITEMS[playerid], pitem);
    }
    return 1;
}

/* Gives player an item with amount */
stock GivePlayerItem(playerid, Item:item, amount, bool:callCallback = true)
{
    // Check if the item object is valid.
    if(!IsValidItem(item))
        return 0;
    
    // Save the amount to insert in cache.
    new amountCache = amount;

    // Amount counts how much items could be added to inventory.
    new amountGot = 0;

    // Gets the item ID from the Item object.
    new itemid = GetItemItemID(item);

    // Gets the stacksize from the Item object.
    new stacksize = GetItemStackSize(item);

    // Check if stacksize is valid.
    if(stacksize <= 0)
        return 0;
    
    /******************************************************
      Try adding item to existing items in the inventory
    *******************************************************/
    foreach(new PlayerItem:i : NI_I_PITEMS[playerid])
    {
        // When item has been found.
        if(NI_PlayerItem[playerid][i][ni_pi_itemId] == itemid)
        {
            // Try inserting item to current slot.
            while(amountCache)
            {
                // Check if there is place to insert the item in current slot.
                if(NI_PlayerItem[playerid][i][ni_pi_amount] < stacksize)
                {
                    // Add the item amount to current slot.
                    NI_PlayerItem[playerid][i][ni_pi_amount]++;
                    amountCache--;
                    amountGot++;
                }
                // Exit loop when not possible to insert.
                else break;
            }
        }
    }

    /******************************************************
      Add item as new when there's no free slot available.
    *******************************************************/

    // Creates an invalid PlayerItem object.
    new PlayerItem:pitem = PlayerItem:-1;

    // Try adding the amount to new slot/s
    while(amountCache)
    {
        // Check if PlayerItem object is Invalid
        if(pitem == PlayerItem:-1)
        {
            // Try getting a free inventory slot.
            pitem = PlayerItem:Iter_Free(NI_I_PITEMS[playerid]);
        }
        // Check if the free inventory slot is valid (Eg. The player has full inventory.)
        if(NI_VALID_ITER_FREE(NI_I_PITEMS[playerid], _:pitem))
        {
            // Create the new inventory slot using the free slot ID.
            Iter_Add(NI_I_PITEMS[playerid], PlayerItem:pitem);

            // Insert data to the inventory slot.
            NI_PlayerItem[playerid][pitem][ni_pi_valid] = true;
            NI_PlayerItem[playerid][pitem][ni_pi_item] = item;
            NI_PlayerItem[playerid][pitem][ni_pi_amount] = 0;
            NI_PlayerItem[playerid][pitem][ni_pi_itemId] = itemid;
            NI_PlayerItem[playerid][pitem][ni_pi_dbId] = -1;

            // Try inserting how much you can to the current slot.
            while(amountCache)
            {
                // Is there room to insert the item?
                if(NI_PlayerItem[playerid][pitem][ni_pi_amount] < stacksize)
                {
                    // Insert the amount to the current slot.
                    NI_PlayerItem[playerid][pitem][ni_pi_amount]++;
                    amountCache--;
                    amountGot++;
                }
                // Exit loop when no room inside the slot.
                else break;
            }
            // Resetting the object variable to be invalid so the loop can repeat creating a new slot
            // when current slot is full 
            pitem = PlayerItem:-1;
        }
        // Exit insert loop when no new slot could be created.
        else break;
    }

    // Call "OnPlayerGetItem" callback inserting the ID of the player, the Item object, the amount
    // which has been added and the remainig amount which couldn't be added.
    if(callCallback)
        CallLocalFunction("OnPlayerGetItem", "iiii", playerid, _:item, amountGot, amountCache);
    return 1;
}


