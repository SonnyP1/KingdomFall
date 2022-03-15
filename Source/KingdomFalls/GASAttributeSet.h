// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "GASAttributeSet.generated.h"

// macros from AttributeSet.h getter and setters
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class KINGDOMFALLS_API UGASAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UGASAttributeSet();
	
	//HitPoints
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData HitPoints;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,HitPoints);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxHitPoints;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,MaxHitPoints);
	
	//Stamina
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Stamina;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,Stamina);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxStamina;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,MaxStamina);

	//Mana
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,Mana);

	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,MaxMana);

	//Other
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData PrimartyAttackPower;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,PrimartyAttackPower);
	
	UPROPERTY(BlueprintReadOnly, Category="Attributes")
	FGameplayAttributeData DmgReduce;
	ATTRIBUTE_ACCESSORS(UGASAttributeSet,DmgReduce);

};
