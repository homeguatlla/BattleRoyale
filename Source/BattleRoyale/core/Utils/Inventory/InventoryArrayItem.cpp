
#include "InventoryArrayItem.h"

#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


int UInventoryArrayItem::Counter = 0;

UInventoryArrayItem::UInventoryArrayItem()
{
	mID = ++Counter;
}

bool UInventoryArrayItem::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	result |= Channel->ReplicateSubobject(mInventoryItem, *Bunch, *RepFlags);

	return result;
}

void UInventoryArrayItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryArrayItem, mID);
	DOREPLIFETIME(UInventoryArrayItem, mInventoryItem);
}
