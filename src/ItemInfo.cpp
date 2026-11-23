#include "common.h"
#include "MsgStrings.h"
#include "framework/Locale.h"
#include "ItemInfo.h"
#include "pdb_enums.h"
#include "GameMode.h"
#include "Session.h"
#include "base/File.h"
#include "framework/Language.h"

extern const char* MsgStr(MSGSTRINGID id);
extern CModeMgr g_modeMgr;
extern CSession g_session;
extern ServiceType g_serviceType;
extern char* DataTxtDecode(char* str);

using std::pair;

std::map<int, ITEM_DESC> ITEM_INFO::s_itemDescMap;
std::map<int, CARD_DESC> ITEM_INFO::s_cardDescMap;
std::map<int, std::vector<char const *> > ITEM_INFO::s_maketargetdesc;
CommonObject ITEM_INFO::s_obj;
std::map<int, std::vector<char const *> > ITEM_INFO::s_bookDescMap;
std::map<int, QUEST_DESC> ITEM_INFO::s_questDescMap;


inline bool IsPCharEmpty(const char* pChar)
{
	if ( pChar == NULL || *pChar == NULL )
		return true;
	else
		return false;
}

template<class T>
typename T::referent_type& GetMapObject(typename T::key_type itemName, T& table)
{
	T::iterator iter = table.find(itemName);

	if(iter != table.end()) return iter->second;

	pair<T::iterator, bool>& result =
		table.insert(T::value_type(itemName, T::referent_type()));

	sAssert(result.second == true);

	return result.first->second;
}


inline void CheckItemTable(PCharListIter tableiter, PCharList& table)
{
	if ( tableiter >= table.end() ) ErrorMsg("Please check the item.txt script.");
}

void CommonObject::Clear()
{
	std::vector<char *>::iterator it;
	for ( it = m_deleteList.begin(); it != m_deleteList.end(); ++it )
	{
		char * buf = *it;
		if (buf) delete [] buf;
		*it = NULL;
	}
	m_deleteList.clear();
}

bool CommonObject :: TokenFileToPCharList(PCharList& pCharList, LPCSTR fName, int type)
{
	pCharList.clear();
	pCharList.reserve(4096);

	CFile fp;
	if(!fp.Open(g_language ? g_language->LanguageFile(fName):fName, CFile::modeRead) ) return false;

	int size = fp.GetLength();
	char * buf = new char[size+1];
#ifdef _TRACEDEBUG
	static int iMem = 0;
	iMem += size+1;
	Trace("TokenFileToPCharList Memory Usage : %d",iMem);
#endif
	fp.Read(buf, size);
	buf[size]=0;
	fp.Close();

	char * base = buf;
	for (int i=0; i<size; i++)
	{
		switch (buf[i])
		{
		case '#':
			if ( strncmp(base,"//",2) == 0 )
				break;
			buf[i] = 0;
			pCharList.push_back(base);
			if ( type == 2 )
				pCharList.push_back(NULL);
			base = buf+i+1;
			break;
		case 10:
			base = buf+i+1;
			break;
		case 13:
			buf[i] = 0;
			if(type && strncmp(base,"//",2) != 0 && base != NULL && *base != NULL)
				pCharList.push_back(base);
			base = buf+i+1;
			break;
		}
	}
	Trace("%s size = %d capacity = %d",fName,pCharList.size()+1,pCharList.capacity());
	m_deleteList.push_back(buf);
	return TRUE;
}

bool IsNumberString(LPCSTR str)
{
	static bool number[256] = {		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
									1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
									0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, };

	for(int i=0; i < strlen(str); ++i)
	{
		if(!number[(BYTE)str[i]]) return false;
	}

	return true;
}

void ITEM_INFO::SetItemId(int itemId)
{
	char buf[16];
	m_itemName = _itoa(itemId, buf, 10);
}

int ITEM_INFO::GetRefining(char* refining) const
{
	return sprintf(refining, "+%d ", m_refiningLevel);
}

int ITEM_INFO::GetNamed(char* named) const
{
	return sprintf(named, MsgStr(MSI_NAMED_PET) );
}

int ITEM_INFO::GetPrefix(char* prefix) const
{
	char* prefixIter = prefix;
	int slot[4] = { m_slot[0], m_slot[1], m_slot[2], m_slot[3] };

	for(int i=0; i<4; ++i) {
		if(!slot[i]) continue;
		if( IsPCharEmpty(GetCardDesc(slot[i]).prefix) ) continue;

		int cardDuplication = 1;

		for(int j=i+1; j<4; ++j) {
			if(slot[i] == slot[j]) { slot[j] = 0; ++cardDuplication; }
		}
		switch(cardDuplication)
		{
		case 2:
			prefixIter += sprintf(prefixIter, "%s ", MsgStr(MSI_DOUBLE));
			break;
		case 3:
			prefixIter += sprintf(prefixIter, "%s ", MsgStr(MSI_TRIPLE));
			break;
		case 4:
			prefixIter += sprintf(prefixIter, "%s ", MsgStr(MSI_QUADROPLE));
			break;
		}
		prefixIter += sprintf(prefixIter, "%s ", GetCardDesc(slot[i]).prefix);
	}

	return prefixIter - prefix;
}

int ITEM_INFO::GetPostfix(char* postfix) const
{
	char* postfixIter = postfix;
	int slot[4] = { m_slot[0], m_slot[1], m_slot[2], m_slot[3] };

	for(int i=0; i<4; ++i) {
		if(!slot[i]) continue;
		if( IsPCharEmpty(GetCardDesc(slot[i]).postfix) ) continue;

		int cardDuplication = 1;
		for(int j=i+1; j<4; ++j) {
			if(slot[i] == slot[j]) { slot[j] = 0; ++cardDuplication; }
		}

		postfixIter += sprintf(postfixIter, " %s", GetCardDesc(slot[i]).postfix);

		switch(cardDuplication)
		{
		case 2:
			postfixIter += sprintf(postfixIter, " %s", MsgStr(MSI_DOUBLE));
			break;
		case 3:
			postfixIter += sprintf(postfixIter, " %s", MsgStr(MSI_TRIPLE));
			break;
		case 4:
			postfixIter += sprintf(postfixIter, " %s", MsgStr(MSI_QUADROPLE));
			break;
		}
	}

	return postfixIter - postfix;
}

int ITEM_INFO::GetManufacturedItemPrefix(char* text) const
{
	BYTE	attack = HIBYTE(m_slot[1]);

	switch(attack)
	{
	case 5:
		strcpy(text, MsgStr(MSI_ONE_STARPIECE));
		break;
	case 10:
		strcpy(text, MsgStr(MSI_TWO_STARPIECE));
		break;
	case 15:
		strcpy(text, MsgStr(MSI_THREE_STARPIECE));
		break;
	default:
		return 0;
	}

	return strlen(text);
}

int ITEM_INFO::GetManufacturedItemProducer(char* text) const
{
	DWORD	gid = m_slot[2] + m_slot[3]*0x10000;

	const std::string& cName = g_modeMgr.GetGameMode()->GetActorNameByGID(gid).cName;

	strcpy(text, cName.c_str());

	return strlen(text);
}

int ITEM_INFO::GetManufacturedItemPostfix(char* text) const
{
	BYTE	property = LOBYTE(m_slot[1]);

	switch(property)
	{
	case 0:
		strcpy(text, MsgStr(MSI_MANUFACTURED_NORMAL_ITEM));
		break;
	case 1:
		strcpy(text, MsgStr(MSI_MANUFACTURED_ICE_ITEM));
		break;
	case 2:
		strcpy(text, MsgStr(MSI_MANUFACTURED_EARTH_ITEM));
		break;
	case 3:
		strcpy(text, MsgStr(MSI_MANUFACTURED_FIRE_ITEM));
		break;
	case 4:
		strcpy(text, MsgStr(MSI_MANUFACTURED_WIND_ITEM));
		break;
	default:
		return 0;
	}

	return strlen(text);
}

int ITEM_INFO::GetSocketNumber(char* text) const
{
	if (GetItemDesc(m_itemName.c_str()).slotCount == 0)
		return 0;
	else
		sprintf(text," [%d]",GetItemDesc(m_itemName.c_str()).slotCount);

	return strlen(text);
}


int ITEM_INFO::GetIdDisplayName(char* name) const
{
	const ITEM_DESC& itemdesc = GetItemDesc(m_itemName.c_str());

	if (itemdesc.identifiedDisplayName == NULL)
	{
		strcpy(name, " ");
		return strlen(name);
	}

	strcpy(name, itemdesc.identifiedDisplayName);
	return strlen(name);
}

int ITEM_INFO::GetUnidDisplayName(char* name) const
{
	const ITEM_DESC& itemdesc = GetItemDesc(m_itemName.c_str());

	strcpy(name, itemdesc.unidentifiedDisplayName);
	return strlen(name);
}

char* ITEM_INFO::GetItemName(char* itemName)
{
	if (m_isIdentified) {
		if (m_slot[0] == 0 && m_slot[1] == 0 && m_slot[2] == 0 && m_slot[3] == 1)
		{
			char* itemNameIter = itemName;
			itemNameIter += GetNamed(itemNameIter);
			GetIdDisplayName(itemNameIter);
		}
		else if (m_slot[0] == 254 && m_slot[1] == 0)
		{
				char* itemNameIter = itemName;

				if(m_refiningLevel)
					itemNameIter += GetRefining(itemNameIter);
				itemNameIter += GetManufacturedItemProducer(itemNameIter);
				itemNameIter += GetManufacturedItemPostfix(itemNameIter);
				GetIdDisplayName(itemNameIter);
		}
		else if(IsAttachableItem()) {
			char* itemNameIter = itemName;

			if(m_refiningLevel)
				itemNameIter += GetRefining(itemNameIter);

			if(HasCardItem()) {
				itemNameIter += GetPrefix(itemNameIter);
				itemNameIter += GetIdDisplayName(itemNameIter);
				itemNameIter += GetPostfix(itemNameIter);
			} else if(IsManufacturedItem()) {
				itemNameIter += GetManufacturedItemPrefix(itemNameIter);
				itemNameIter += GetManufacturedItemProducer(itemNameIter);
				itemNameIter += GetManufacturedItemPostfix(itemNameIter);
				GetIdDisplayName(itemNameIter);
			} else {
				GetIdDisplayName(itemNameIter);
			}
		}
		else
		{
			GetIdDisplayName(itemName);
		}
	} else {
		GetUnidDisplayName(itemName);
	}

	return itemName;
}

ITEM_INFO& ITEM_INFO::operator=(const ITEM_INFO& iteminfo)
{
	m_itemType			= iteminfo.m_itemType;
	m_location			= iteminfo.m_location;
	m_itemIndex			= iteminfo.m_itemIndex;
	m_wearLocation		= iteminfo.m_wearLocation;
	m_num				= iteminfo.m_num;
	m_price				= iteminfo.m_price;
	m_realPrice			= iteminfo.m_realPrice;
	memcpy(m_slot, iteminfo.m_slot, sizeof(m_slot));
	m_itemName			= iteminfo.m_itemName;
	m_isIdentified		= iteminfo.m_isIdentified;
	m_isDamaged			= iteminfo.m_isDamaged;
	m_refiningLevel		= iteminfo.m_refiningLevel;

	return *this;
}

ITEM_INFO& ITEM_INFO::operator=(const SHOP_ITEM& iteminfo)
{
	m_itemName		= iteminfo.itemName;
	m_price			= iteminfo.price;
	m_realPrice		= iteminfo.price;
	m_itemIndex		= iteminfo.index;
	m_itemType		= iteminfo.type;
	m_realPrice		= iteminfo.realPrice;
	m_isIdentified	= iteminfo.isIdentified;
	return *this;
}

const char* ITEM_INFO::GetResName()
{
	if (m_isIdentified)
		return GetItemDesc(m_itemName.c_str()).identifiedResourceName;
	else
		return GetItemDesc(m_itemName.c_str()).unidentifiedResourceName;
}

const std::vector<char const *>& ITEM_INFO::GetDescription()
{
	if (m_isIdentified)
		return GetItemDesc(m_itemName.c_str()).identifiedDescriptionName;
	else
		return GetItemDesc(m_itemName.c_str()).unidentifiedDescriptionName;
}

int ITEM_INFO::GetSlotCount()
{
	return GetItemDesc(m_itemName.c_str()).slotCount;
}

bool ITEM_INFO::IsManufacturedItem() const
{
	if(m_slot[0] == 0x00FF) return true;

	return false;
}

bool ITEM_INFO::IsRecoveryItem() const
{
	if (atoi(m_itemName.c_str()) > 500 && atoi(m_itemName.c_str()) < 600)
		return true;
	return false;
}

bool ITEM_INFO::IsAttachableItem() const
{
	switch (m_itemType)
	{
	case TYPE_ARMOR:
	case TYPE_ARMORTM:
	case TYPE_ARMORTB:
	case TYPE_ARMORMB:
	case TYPE_ARMORTMB:
	case TYPE_WEAPON:
	case TYPE_BOW:
	case TYPE_BOTHHAND:
	case TYPE_GUN:
	case TYPE_LAST:
		return true;
	default:
		return false;
	}
}

bool ITEM_INFO::IsPropertyItem() const
{
	int id = atoi(m_itemName.c_str());
	if (id >= 994 && id <= 997)
		return true;
	return false;
}

bool ITEM_INFO::IsMaterialItem() const
{
	if (atoi(m_itemName.c_str()) != 1000) {
		int id = atoi(m_itemName.c_str());
		if (id < 994 || id > 997)
			return false;
	}
	return true;
}

int ITEM_INFO::SetType()
{
	int id = atoi(m_itemName.c_str());
	if ( id > 500 && id < 700 )
		m_itemType = TYPE_HEAL;
	else if ( id > 1100 && id < 3000 )
		m_itemType = TYPE_WEAPON;
	else if ( id > 5000 && id < 6000 )
		m_itemType = TYPE_ARMOR;
	else
		m_itemType = TYPE_CARD;

	return m_itemType;
}

void ITEM_INFO::CopyItemInfo(const DRAG_INFO* dragInfo)
{
	m_price			= dragInfo->m_dragItemPrice;
	m_realPrice		= dragInfo->m_dragItemRealPrice;
	m_num			= dragInfo->m_numDragItem;
	m_itemName		= dragInfo->m_dragItemName;
	m_itemIndex		= dragInfo->m_dragItemIndex;
	m_itemType		= dragInfo->m_dragItemType;
	m_isIdentified	= dragInfo->m_isIdentified;
	m_refiningLevel = dragInfo->m_refiningLevel;
	m_slot[0] = dragInfo->m_slot[0];
	m_slot[1] = dragInfo->m_slot[1];
	m_slot[2] = dragInfo->m_slot[2];
	m_slot[3] = dragInfo->m_slot[3];
}




static const char* const strNull = "";
static const char* const strUnknown = "Unknown Item";
static const char* const strApple = "Apple";
static const char* const strSorry = "sorry";
void ITEM_INFO::InitItemInfoTables()
{
	int itemName;
	PCharList table;
	PCharListIter tableiter;

	s_itemDescMap.clear();
	s_cardDescMap.clear();
	s_maketargetdesc.clear();
	s_obj.Clear();
	s_bookDescMap.clear();
	s_questDescMap.clear();

	const int ReserveSize = 20000;
	table.reserve( ReserveSize );

	bool bLoadItemScript = false;
	const char * itemScript[2] = { "item.txt","eventItem.txt" };

	for ( int i = 0; i < 2; i++ )
	{
		if ( CFile::IsFileExist(itemScript[i]) )
		{
			s_obj.TokenFileToPCharList(table, itemScript[i], 2);
			for(tableiter=table.begin();tableiter<table.end();++tableiter)
			{

				ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
				tableiter +=2; CheckItemTable(tableiter,table);

				if ( *tableiter != NULL )
					g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
				itemdesc.identifiedDisplayName = *tableiter;

				tableiter +=2; CheckItemTable(tableiter,table);
				while( *tableiter != NULL )
				{
					itemdesc.identifiedDescriptionName.push_back(*tableiter);
					tableiter++;
				}

				tableiter ++; CheckItemTable(tableiter,table);

				bool bNeedIdentify = true;
				if ( *tableiter != NULL )
				{
					if (strstr( *tableiter,"idenitfied equal unidentified" ) )
						bNeedIdentify = false;
					else
						g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
				}
				else
					bNeedIdentify = false;

				if ( bNeedIdentify == true )
				{
					itemdesc.unidentifiedDisplayName = *tableiter;

					tableiter +=2; CheckItemTable(tableiter,table);
					while( *tableiter != NULL )
					{
						itemdesc.unidentifiedDescriptionName.push_back(*tableiter);
						tableiter++;
					}
				}
				else
				{
					itemdesc.unidentifiedDisplayName = itemdesc.identifiedDisplayName;
					itemdesc.unidentifiedDescriptionName = itemdesc.identifiedDescriptionName;
					tableiter++;
				}
			}
			bLoadItemScript = true;
		}
	}

	if ( bLoadItemScript == false )
	{
		if ( CFile::IsFileExist("EventNum2ItemDisplayNameTable.txt") )
		{
			s_obj.TokenFileToPCharList(table, "EventNum2ItemDisplayNameTable.txt", 0);
			for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

				ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
				++tableiter; sAssert(tableiter != table.end());
				g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
				itemdesc.unidentifiedDisplayName = *tableiter;
			}
		}

		if ( CFile::IsFileExist("EventNum2itemDescTable.txt") )
		{
			itemName = 0;
			s_obj.TokenFileToPCharList(table, "EventNum2itemDescTable.txt", true);
			for(tableiter=table.begin();tableiter!=table.end();++tableiter) {
				if(IsPCharEmpty(*tableiter)) continue;
				if(IsNumberString(*tableiter)) {

					itemName = atoi(*tableiter);
				} else {
					DataTxtDecode((char *)*tableiter);
					GetMapObject(itemName, s_itemDescMap).unidentifiedDescriptionName.push_back(*tableiter);
				}
			}
		}

		if ( CFile::IsFileExist("EventIdNum2ItemDisplayNameTable.txt") )
		{
			s_obj.TokenFileToPCharList(table, "EventIdNum2ItemDisplayNameTable.txt", 0);
			for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

				ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
				++tableiter; sAssert(tableiter != table.end());
				g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
				itemdesc.identifiedDisplayName = *tableiter;
			}
		}

		if ( CFile::IsFileExist("EventIdNum2itemDescTable.txt") )
		{
			itemName = 0;
			s_obj.TokenFileToPCharList(table, "EventIdNum2itemDescTable.txt", true);
			for(tableiter=table.begin();tableiter!=table.end();++tableiter) {
				if(IsPCharEmpty(*tableiter)) continue;
				if(IsNumberString(*tableiter)) {

					itemName = atoi(*tableiter);
				} else {
					DataTxtDecode((char * )*tableiter);
					GetMapObject(itemName, s_itemDescMap).identifiedDescriptionName.push_back(*tableiter);
				}
			}
		}

		s_obj.TokenFileToPCharList(table, "Num2ItemDisplayNameTable.txt", 0);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

			ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
			++tableiter; sAssert(tableiter != table.end());
			g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
			itemdesc.unidentifiedDisplayName = *tableiter;
		}

		itemName = 0;
		s_obj.TokenFileToPCharList(table, "Num2itemDescTable.txt", true);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {
			if(IsPCharEmpty(*tableiter)) continue;
			if(IsNumberString(*tableiter)) {

				itemName = atoi(*tableiter);
			} else {
				DataTxtDecode((char *)*tableiter);
				GetMapObject(itemName, s_itemDescMap).unidentifiedDescriptionName.push_back(*tableiter);
			}
		}

		s_obj.TokenFileToPCharList(table, "IdNum2ItemDisplayNameTable.txt", 0);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

			ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
			++tableiter; sAssert(tableiter != table.end());
			g_session.RemoveUnderbar(*tableiter, (char *)*tableiter);
			itemdesc.identifiedDisplayName = *tableiter;
		}

		itemName = 0;
		s_obj.TokenFileToPCharList(table, "IdNum2itemDescTable.txt", true);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {
			if(IsPCharEmpty(*tableiter)) continue;
			if(IsNumberString(*tableiter)) {

				itemName = atoi(*tableiter);
			} else {
				DataTxtDecode((char * )*tableiter);
				GetMapObject(itemName, s_itemDescMap).identifiedDescriptionName.push_back(*tableiter);
			}
		}
	}

	s_obj.TokenFileToPCharList(table, "Num2ItemResNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.unidentifiedResourceName = *(tableiter);
	}

	s_obj.TokenFileToPCharList(table, "IdNum2ItemResNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.identifiedResourceName = *(tableiter);
	}

	if ( CFile::IsFileExist("EventNum2ItemResNameTable.txt") )
	{
		s_obj.TokenFileToPCharList(table, "EventNum2ItemResNameTable.txt", 0);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

			ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
			++tableiter; sAssert(tableiter != table.end());
			itemdesc.unidentifiedResourceName = *(tableiter);
		}
	}

	if ( CFile::IsFileExist("EventIdNum2ItemResNameTable.txt") )
	{
		s_obj.TokenFileToPCharList(table, "EventIdNum2ItemResNameTable.txt", 0);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

			ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
			++tableiter; sAssert(tableiter != table.end());
			itemdesc.identifiedResourceName = *(tableiter);
		}
	}

	s_obj.TokenFileToPCharList(table, "itemSlotTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.slot = atoi(*tableiter);
	}

	s_obj.TokenFileToPCharList(table, "itemParamTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.parameter.push_back(atoi(*tableiter));
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.parameter.push_back(atoi(*tableiter));
	}

	s_obj.TokenFileToPCharList(table, "itemSlotCountTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		ITEM_DESC& itemdesc = GetMapObject(atoi(*tableiter), s_itemDescMap);
		++tableiter; sAssert(tableiter != table.end());
		itemdesc.slotCount = atoi(*tableiter);
	}

	s_obj.TokenFileToPCharList(table, "CardItemNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		GetMapObject(atoi(*tableiter), s_cardDescMap);
	}

	s_obj.TokenFileToPCharList(table, "Num2CardIllustNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		CARD_DESC& carddesc = GetMapObject(atoi(*tableiter), s_cardDescMap);
		++tableiter; sAssert(tableiter != table.end());
		carddesc.illustName = *(tableiter);
	}

	s_obj.TokenFileToPCharList(table, "CardPrefixNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		CARD_DESC& carddesc = GetMapObject(atoi(*tableiter), s_cardDescMap);
		++tableiter; sAssert(tableiter != table.end());
		carddesc.prefix = *(tableiter);
	}

	s_obj.TokenFileToPCharList(table, "CardPostfixNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		CARD_DESC& carddesc = GetMapObject(atoi(*tableiter), s_cardDescMap);
		carddesc.postfix = carddesc.prefix;
		carddesc.prefix = strNull;
	}

	s_obj.TokenFileToPCharList(table, "BookItemNameTable.txt", 0);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {

		GetMapObject(atoi(*tableiter), s_bookDescMap).push_back(*tableiter);
	}

	if ( g_serviceType == ServiceKorea )
	{
		s_obj.TokenFileToPCharList(table, "questID2display.txt", 0);
		for(tableiter=table.begin();tableiter!=table.end();++tableiter)
		{

			QUEST_DESC& questdesc = GetMapObject(atoi(*tableiter), s_questDescMap);

			++tableiter; sAssert(tableiter != table.end());
			questdesc.questName = *(tableiter);
			++tableiter; sAssert(tableiter != table.end());
			questdesc.iconName = *(tableiter);
			++tableiter; sAssert(tableiter != table.end());
			questdesc.detailBmp = *(tableiter);
			++tableiter; sAssert(tableiter != table.end());
			questdesc.summaryTXT = *(tableiter);
			++tableiter; sAssert(tableiter != table.end());
			questdesc.objectTXT = *(tableiter);
		}
	}

	itemName = 0;
	s_obj.TokenFileToPCharList(table, "MetalProcessItemList.txt", true);
	for(tableiter=table.begin();tableiter!=table.end();++tableiter) {
		if(IsPCharEmpty(*tableiter)) continue;
		if(IsNumberString(*tableiter)) {

			itemName = atoi(*tableiter);
		} else if(strlen(*tableiter) > 0) {
			GetMapObject(itemName, s_maketargetdesc).push_back(*tableiter);
		}
	}

	ITEM_DESC& unknownitemdesc = GetDesc(0, s_itemDescMap);

	unknownitemdesc.identifiedDisplayName = strUnknown;
	unknownitemdesc.identifiedResourceName = strApple;
	unknownitemdesc.identifiedDescriptionName.push_back(strUnknown);
	unknownitemdesc.unidentifiedDisplayName = strUnknown;
	unknownitemdesc.unidentifiedResourceName = strApple;
	unknownitemdesc.unidentifiedDescriptionName.push_back(strUnknown);

	ITEM_DESC& unknownitemdesc2 = GetDesc(-1, s_itemDescMap);
	unknownitemdesc2 = unknownitemdesc;

	CARD_DESC& unknowncarddesc = GetDesc(0, s_cardDescMap);
	unknowncarddesc.illustName = strSorry;
	unknowncarddesc.prefix = strNull;
	unknowncarddesc.postfix = strNull;

	CARD_DESC& unknowncarddesc2 = GetDesc(-1, s_cardDescMap);
	unknowncarddesc2 = unknowncarddesc;
}


const ITEM_DESC& ITEM_INFO::GetItemDesc(int itemName)
{
	return GetDesc(itemName, s_itemDescMap);
}

bool ITEM_INFO::IsCardItem(int itemName)
{
	return (s_cardDescMap.find(itemName) != s_cardDescMap.end());
}

const CARD_DESC& ITEM_INFO::GetCardDesc(int itemName)
{
	return GetDesc(itemName, s_cardDescMap);
}

const std::vector<char const *>& ITEM_INFO::GetMakeTargetDesc(int itemName)
{
	return GetDesc(itemName, s_maketargetdesc);
}

bool ITEM_INFO::IsBookItem(int itemName)
{
	return (s_bookDescMap.find(itemName) != s_bookDescMap.end());
}

const QUEST_DESC& ITEM_INFO::GetQuestDesc(int itemName)
{
	return GetDesc(itemName, s_questDescMap);
}
