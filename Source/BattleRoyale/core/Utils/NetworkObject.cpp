// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkObject.h"

bool UNetworkObject::IsSupportedForNetworking() const
{
	return true;
}

bool UNetworkObject::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	return false;
}
