/************************************************************
                        LICENSE

Script: NInventory
Type: Include
Version: Dev 1.1
Author: Nickk888
Date: 08.05.2022

************************************************************
                    Social Media
*************************************************************

YouTube: https://www.youtube.com/channel/UCvjbpjJXET1QS9Wa0_XVCuw
Facebook: https://www.facebook.com/Nickk888FP/

************************************************************
            When using a Database eg. MySQL
*************************************************************

Simply create 2 tables in your Database, one for the "Server" Items and the second one 
for the player Items. Look at the Example:

For Server Items: 
CREATE TABLE `items` (
	`id` INT(11) NOT NULL AUTO_INCREMENT,
	`name` VARCHAR(128) NULL DEFAULT NULL,
	`description` VARCHAR(128) NULL DEFAULT NULL,
	`stacksize` INT(11) NULL DEFAULT '1',
	`category` INT(11) NULL DEFAULT '0',
	`objectmodelid` INT(11) NULL DEFAULT '0',
	`iconname` VARCHAR(50) NULL DEFAULT NULL,
	PRIMARY KEY (`id`));

Use the AUTO INCREMENTed ID as the Item ID parameter so every item gets a unique Item ID.

For Player Items: 
CREATE TABLE `player_items` (
	`id` INT(11) NULL DEFAULT '0',
	`userid` INT(11) NULL DEFAULT '0',
	`itemid` INT(11) NULL DEFAULT '0',
	`amount` INT(11) NULL DEFAULT '1',
	PRIMARY KEY (`id`)
)

************************************************************
                        Functions
*************************************************************

************************************************************
                        Callbacks
*************************************************************

OnPlayerItemGet(playerid, Item:item, amount, remains);

************************************************************
                        Loops
*************************************************************

LOOP_ITEMS(variableName) - Loops through the server items, returns item as tag "Item".
LOOP_PLAYER_ITEMS(playerid, variableName) - Loops through the player items, returns item as tag "PlayerItem".

*************************************************************/

#if defined _nitems_included
	#endinput
#endif
#define _nitems_included

/************************************************************
                        INCLUDES
*************************************************************/

#include <YSI_Data\y_foreach>
#include <YSI_Coding\y_hooks>

/************************************************************
                        FORWARDS
*************************************************************/

forward OnPlayerItemGet(playerid, Item:item, amount, remains, PlayerItem:updatedItems[], updatedItemsAmount);
forward OnPlayerItemRemoved(playerid, PlayerItem:pitem);

forward Item:CreateItem(const name[], const description[] = "", stacksize = 1, category = 0, objectmodelid = 0, dbid = 0, itemid = -1, const iconname[] = "");
forward Item:GetPlayerItemItem(playerid, PlayerItem:pitem);
forward Item:GetRandomItem();
forward Item:GetItemFromItemID(itemid);
forward Item:GetItemFromName(const name[], bool:ignorecase = true);
forward Item:GetItemFromIndex(index);
forward PlayerItem:SetPlayerItem(playerid, Item:item, slot, amount);
forward PlayerItem:GetPlayerItemFromItemID(playerid, itemid);
forward PlayerItem:GetPlayerItemFromIndex(playerid, index);
forward PlayerItem:GetRandomPlayerItem(playerid);
forward PlayerItem:GetPlayerItemFromName(playerid, const name[], bool:ignorecase = true);
forward Float:GetItemFloatData(Item:item, arrayId);

/************************************************************
                        DEFINITIONS
*************************************************************/

#if !defined MAX_SERVER_ITEMS
    #define MAX_SERVER_ITEMS 100
#endif

#if !defined MAX_PLAYER_ITEMS
    #define MAX_PLAYER_ITEMS 20
#endif

/* Loops through all Items returning the Item object. */
#define LOOP_ITEMS(%1) foreach(new Item:%1 : NI_I_ITEMS)
/* Loops through all player Items returning the PlayerItem object. */
#define LOOP_PLAYER_ITEMS(%1,%2) foreach(new PlayerItem:%2 : NI_I_PITEMS[%1])

/************************************************************
                        INTERNALS
*************************************************************/

#define NI_MAX_STRING_SIZE 128
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
	ni_i_name[NI_MAX_STRING_SIZE],
	ni_i_desc[NI_MAX_STRING_SIZE],
    ni_i_category,
	ni_i_intData[NI_MAX_DATA_AMOUNT],
    ni_i_objModelId,
	ni_i_iconName[NI_MAX_STRING_SIZE],
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
    Iter_Init(PlayerItem:NI_I_PITEMS);
    return 1;
}

/************************************************************
                        FUNCTIONS
*************************************************************/

/* Creates new item returning the item object */
stock Item:CreateItem(const name[], const description[] = "", stacksize = 1, category = 0, objectmodelid = 0, dbid = 0, itemid = -1, const iconname[] = "")
{
    new Item:item = Item:Iter_Free(NI_I_ITEMS);
    if(NI_VALID_ITER_FREE(NI_I_ITEMS, _:item))
    {
        Iter_Add(NI_I_ITEMS, item);
        NI_Item[item][ni_i_valid] = true;
        NI_Item[item][ni_i_itemid] = ((itemid == -1) ? (_:item + 1) : (itemid));
        NI_Item[item][ni_i_dbId] = dbid;
        NI_Item[item][ni_i_category] = category;
        NI_Item[item][ni_i_objModelId] = objectmodelid;
        NI_Item[item][ni_i_stackSize] = ((stacksize > 0) ? (stacksize) : (1));
        strpack(NI_Item[item][ni_i_name], name, 128);
        strpack(NI_Item[item][ni_i_desc], description, 128);
        strpack(NI_Item[item][ni_i_iconName], iconname, 128);
        for(new i; i < NI_MAX_DATA_AMOUNT; i++)
        {
            NI_Item[item][ni_i_intData][i] = 0;
            NI_Item[item][ni_i_floatData][i] = 0.0;
        }
        #if defined NI_DEBUG
        printf("[NINVENTORY - DEBUG] Created Item '%s', Description '%s', ItemID '%i', Stacksize '%i', Index '%i'", 
            name, 
            description, 
            NI_Item[item][ni_i_itemid],
            NI_Item[item][ni_i_stackSize],
            _:item);
        #endif
        return item;
    }
    return Item:-1;
}

/* Gets item float data inside the Item */
stock Float:GetItemFloatData(Item:item, arrayId)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return Float:0.0;

    return NI_Item[item][ni_i_floatData][arrayId];
}

/* Gets item intiger data inside the Item */
stock GetItemIntData(Item:item, arrayId)
{
    if(!IsValidItem(item) || arrayId < 0 || arrayId >= NI_MAX_DATA_AMOUNT)
        return 0;

    return NI_Item[item][ni_i_intData][arrayId];
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

    strunpack(unpackedItemStr, NI_Item[item][ni_i_desc], 128);
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

/* Gets item category ID inside the Item */
stock GetItemCategory(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_category];
}

/* Gets item stack size inside the Item */
stock GetItemStackSize(Item:item)
{
    if(!IsValidItem(item))
        return 0;

    return NI_Item[item][ni_i_stackSize];
}

/* Sets new item icon name inside the Item */
stock SetPlayerItemDatabaseID(playerid, PlayerItem:pitem, dbid)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    NI_PlayerItem[playerid][pitem][ni_pi_dbId] = dbid;
    return 1;
}

/* Gets the item ID from player inventory */
stock GetPlayerItemID(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    return NI_PlayerItem[playerid][pitem][ni_pi_itemId];
}

/* Returns the Player items icon name. */
stock GetPlayerItemIconName(playerid, PlayerItem:pitem)
{
    new unpackedItemStr[128];
    new Item:item = GetPlayerItemItem(playerid, pitem);
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[item][ni_i_iconName], 128);
    return unpackedItemStr;
}

/* Returns the Player items name. */
stock GetPlayerItemName(playerid, PlayerItem:pitem)
{
    new unpackedItemStr[128];
    new Item:item = GetPlayerItemItem(playerid, pitem);
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[item][ni_i_name], 128);
    return unpackedItemStr;
}

/* Returns the Player items description. */
stock GetPlayerItemDescription(playerid, PlayerItem:pitem)
{
    new unpackedItemStr[128];
    new Item:item = GetPlayerItemItem(playerid, pitem);
    if(!IsValidItem(item))
        return unpackedItemStr;

    strunpack(unpackedItemStr, NI_Item[item][ni_i_desc], 128);
    return unpackedItemStr;
}

/* Returns the player items database ID, good for storing and reading in a MySQL Database. */
stock GetPlayerItemDatabaseID(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;
    
    return NI_PlayerItem[playerid][pitem][ni_pi_dbId];
}

/* Returns the player items integer data from the array. */
stock GetPlayerItemIntData(playerid, PlayerItem:pitem, arrayId)
{
    return GetItemIntData(GetPlayerItemItem(playerid, pitem), arrayId);   
}

/* Returns the player items floating point data from the array. */
stock Float:GetPlayerItemFloatData(playerid, PlayerItem:pitem, arrayId)
{
    return GetItemFloatData(GetPlayerItemItem(playerid, pitem), arrayId);   
}

/* Returns the player items category. */
stock GetPlayerItemCategory(playerid, PlayerItem:pitem)
{
    return GetItemCategory(GetPlayerItemItem(playerid, pitem));
}

/* Returns the Player items stack size. */
stock GetPlayerItemStackSize(playerid, PlayerItem:pitem)
{
    return GetItemStackSize(GetPlayerItemItem(playerid, pitem));
}

/* Returns the amount of server items created. */
stock CountItems()
{
    return Iter_Count(NI_I_ITEMS);
}

/* Returns the amount of items in the Player Inventory. */
stock CountPlayerItems(playerid)
{
    if(!IsPlayerConnected(playerid))
        return 0;
    
    return Iter_Count(NI_I_PITEMS[playerid]);
}

/* Gets the item amount from player inventory. */
stock GetPlayerItemAmount(playerid, PlayerItem:pitem)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    return NI_PlayerItem[playerid][pitem][ni_pi_amount];
}

/* Gets the item object from the player item object */
stock Item:GetPlayerItemItem(playerid, PlayerItem:pitem)
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

/* Gets Object Model ID inside the Player Item. */
stock GetPlayerItemModelID(playerid, PlayerItem:pitem)
{
    return GetItemModelID(GetPlayerItemItem(playerid, pitem));
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

/* Sets new item float data inside the Item */
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

/* Sets new item category inside the Item */
stock SetItemCategory(Item:item, categoryid)
{
    if(!IsValidItem(item))
        return 0;

    NI_Item[item][ni_i_category] = categoryid;
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
    LOOP_ITEMS(i)
    {
        if(NI_Item[i][ni_i_valid] && NI_Item[i][ni_i_itemid] == itemid)
        {
            return Item:i;
        }
    }
    return Item:-1;
}

/* Gets Item object from name */
stock Item:GetItemFromName(const name[], bool:ignorecase = true)
{
    LOOP_ITEMS(i)
    {
        if(!strcmp(GetItemName(i), name, ignorecase))
        {
            return i;
        }
    }
    return Item:-1;
}

/* Checks if item ID exists and is valid */
stock IsValidItemID(itemid)
{
    LOOP_ITEMS(i)
    {
        if(NI_Item[i][ni_i_valid] && NI_Item[i][ni_i_itemid] == itemid)
        {
            return 1;
        }
    }
    return 0;
}

/* Checks if a Player Item exists passing the name. */
stock PlayerItemExistsByName(playerid, const name[], bool:ignorecase = true)
{
    LOOP_PLAYER_ITEMS(playerid, i)
    {
        if(NI_PlayerItem[playerid][i][ni_pi_valid] && !strcmp(GetPlayerItemName(playerid, i), name, ignorecase))
        {
            return 1;
        }
    }
    return 0
}

/* Checks if a Player Item exists passing the item ID. */
stock PlayerItemExistsByItemID(playerid, itemid)
{
    LOOP_PLAYER_ITEMS(playerid, i)
    {
        if(NI_PlayerItem[playerid][i][ni_pi_valid] && NI_PlayerItem[playerid][i][ni_pi_itemId] == itemid)
        {
            return 1;
        }
    }
    return 0
}

/* Gets Player Item object from itemid */
stock PlayerItem:GetPlayerItemFromItemID(playerid, itemid)
{
    LOOP_PLAYER_ITEMS(playerid, i)
    {
        if(NI_PlayerItem[playerid][i][ni_pi_valid] && NI_PlayerItem[playerid][i][ni_pi_itemId] == itemid)
        {
            return i;
        }
    }
    return PlayerItem:-1;
}

/* Gets a Player Item object from the name. */
stock PlayerItem:GetPlayerItemFromName(playerid, const name[], bool:ignorecase = true)
{
    LOOP_PLAYER_ITEMS(playerid, i)
    {
        if(NI_PlayerItem[playerid][i][ni_pi_valid] && !strcmp(GetPlayerItemName(playerid, i), name, ignorecase))
        {
            return i;
        }
    }
    return PlayerItem:-1;
}

/* Checks if player item ID exists and is valid */
stock IsValidPlayerItem(playerid, PlayerItem:item)
{
    if(_:item >= 0 && _:item < MAX_PLAYER_ITEMS && NI_PlayerItem[playerid][item][ni_pi_valid])
    {
        return 1;
    }
    return 0;
}


/* Checks if item exists using the name */
stock ItemNameExists(const name[], bool:ignorecase = true)
{
    LOOP_ITEMS(i)
    {
        if(!strcmp(GetItemName(i), name, ignorecase))
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
stock RemovePlayerItem(playerid, PlayerItem:pitem, bool:callCallback = true)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    if(Iter_Remove(NI_I_PITEMS[playerid], pitem))
    {
        if(callCallback)
            CallLocalFunction("OnPlayerItemRemoved", "ii", playerid, _:pitem);
        NI_PlayerItem[playerid][pitem][ni_pi_valid] = false;
        return 1;
    }
    return 0;
}

/* Safely removes player item, mostly used in iterators */
stock SafeRemovePlayerItem(playerid, PlayerItem:pitem, & PlayerItem:nextItem, bool:callCallback = true)
{
    if(!IsValidPlayerItem(playerid, pitem))
        return 0;

    new next;
    if(Iter_SafeRemove(NI_I_PITEMS[playerid], pitem, next))
    {
        nextItem = PlayerItem:next;
        if(callCallback)
            CallLocalFunction("OnPlayerItemRemoved", "ii", playerid, _:pitem);
        NI_PlayerItem[playerid][pitem][ni_pi_valid] = false;
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
        if(TakePlayerItem(playeridA, item, amount))
        {
            GivePlayerItem(playeridB, GetPlayerItemItem(playeridA, item), amount);
        }
        
        return 1;
    }
    return 0;
}

/* Gets the PlayerItem from the Iterator from the given Index.  */
stock Item:GetItemFromIndex(index)
{
    new Item:item = Item:Iter_Index(NI_I_ITEMS, Item:index);
    if(!IsValidItem(item))
        return Item:-1;
    
    return item;
}

/* Gets the PlayerItem from the Iterator from the given Index.  */
stock PlayerItem:GetPlayerItemFromIndex(playerid, index)
{
    new PlayerItem:pitem = PlayerItem:Iter_Index(NI_I_PITEMS[playerid], PlayerItem:index);
    if(!IsValidPlayerItem(playerid, pitem))
        return PlayerItem:-1;
    
    return pitem;
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
        return 1;
    }
    return 0;
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
        return 1;
    }
    return 0;
}

/* Returns a random player Item */
stock PlayerItem:GetRandomPlayerItem(playerid)
{
    if(!IsPlayerConnected(playerid))
        return PlayerItem:-1;
    
    return PlayerItem:Iter_Random(NI_I_PITEMS[playerid]);
}

/* Returns a random Item */
stock Item:GetRandomItem() {
    return Item:Iter_Random(NI_I_ITEMS);
}

/* Clears player inventory */
stock ClearPlayerItems(playerid)
{
    LOOP_PLAYER_ITEMS(playerid, i)
    {
        NI_PlayerItem[playerid][i][ni_pi_valid] = false;
    }
    Iter_Clear(NI_I_PITEMS[playerid]);
    return 1;
}
    

PlayerItem:SetPlayerItem(playerid, Item:item, slot, amount)
{
    if(!IsValidItem(item))
        return PlayerItem:-1;

    // Gets the item ID from the Item object.
    new itemid = GetItemItemID(item);

    // Gets the stacksize from the Item object.
    new stacksize = GetItemStackSize(item);

    new PlayerItem:pitem = PlayerItem:slot;
    if(NI_VALID_ITER_FREE(NI_I_PITEMS[playerid], _:pitem))
    {
        if(NI_PlayerItem[playerid][pitem][ni_pi_valid] == false)
        {
            // Create the new inventory slot using the free slot ID.
            Iter_Add(NI_I_PITEMS[playerid], PlayerItem:pitem);

            // Insert data to the inventory slot.
            NI_PlayerItem[playerid][pitem][ni_pi_valid] = true;
            NI_PlayerItem[playerid][pitem][ni_pi_item] = item;
            NI_PlayerItem[playerid][pitem][ni_pi_amount] = clamp(amount, 0, stacksize);
            NI_PlayerItem[playerid][pitem][ni_pi_itemId] = itemid;
            NI_PlayerItem[playerid][pitem][ni_pi_dbId] = -1;
            return PlayerItem:pitem;
        }
    }
    return PlayerItem:-1;
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

    // Variables for identifying updated items returned in the callback
    // Good for a saving system like MySQL
    new PlayerItem:currentItem = PlayerItem:-1;
    new updatedItemsAmount = 0;
    new PlayerItem:updatedItems[MAX_PLAYER_ITEMS];
    
    /******************************************************
      Try adding item to existing items in the inventory
    *******************************************************/
    LOOP_PLAYER_ITEMS(playerid, i)
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
                    if(currentItem != i)
                    {
                        updatedItemsAmount++;
                        currentItem = i;
                    }
                    updatedItems[updatedItemsAmount - 1] = i;
                }
                // Exit loop when not possible to insert.
                else 
                {
                    break;
                }
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
                    if(currentItem != pitem)
                    {
                        updatedItemsAmount++;
                        currentItem = pitem;
                    }
                    updatedItems[updatedItemsAmount - 1] = pitem;
                }
                // Exit loop when no room inside the slot.
                else 
                {
                    break;
                }
            }
            // Resetting the object variable to be invalid so the loop can repeat creating a new slot
            // when current slot is full 
            pitem = PlayerItem:-1;
        }
        // Exit insert loop when no new slot could be created.
        else break;
    }

    // Call "OnPlayerItemGet" callback inserting the ID of the player, the Item object, the amount
    // which has been added and the remainig amount which couldn't be added.
    if(callCallback)
        CallLocalFunction("OnPlayerItemGet", "iiiiad", playerid, _:item, amountGot, amountCache, _:updatedItems, updatedItemsAmount);
    return 1;
}


