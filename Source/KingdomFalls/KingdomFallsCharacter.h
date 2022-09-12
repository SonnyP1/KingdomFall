// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystemInterface.h"
#include "GASAbilitySystemComponent.h"
#include "GASAttributeSet.h"
#include "GameplayEffectTypes.h"

#include "KingdomFallsCharacter.generated.h"

UCLASS()
class KINGDOMFALLS_API AKingdomFallsCharacter : public ACharacter , public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AKingdomFallsCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsLockOn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* lockOnTarget;

	//Components
		
		//Camera Comps
	UPROPERTY(BlueprintReadWrite)
	USpringArmComponent * PlayerEyeSpringArm;
	UCameraComponent* PlayerEye;
	
		//Abilities Comps
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="AbilitySystem")
	class UGASAbilitySystemComponent * PlayerAbilitySystemComponent;
	UPROPERTY()
	class UGASAttributeSet * PlayerAttributes;
	virtual class UAbilitySystemComponent * GetAbilitySystemComponent() const override;
	
		//Abilites Functions
	virtual void InitializeAttributes();
	virtual void GiveAbilities();

		//Default AAttributes eg: Hitpoints , Stamina , Mana
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Gameplay Effects")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
	
		//Default Abilities
	UPROPERTY(BlueprintReadOnly,EditDefaultsOnly,Category="Gameplay Abilities")
	TArray<TSubclassOf<class UGASGameplayAbility>> DefaultAbilities;

private:
	float _lookMultipler;
	float _moveMultipler;
	void QuickTurnCamera(bool turn);

protected:
	//Input Functions
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	void LookRightYawInput(float axisValue);
	void LookUpPitchInput(float axisValue);
	void SprintReleased();
	void LockOnPressed();
	UFUNCTION(BlueprintImplementableEvent)
	void CancelSprint();
	UFUNCTION(BlueprintCallable)
	void TurnOffInputs();
};

