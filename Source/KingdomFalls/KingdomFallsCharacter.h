// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"

#include "KingdomFallsCharacter.generated.h"

UCLASS()
class KINGDOMFALLS_API AKingdomFallsCharacter : public ACharacter
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

	//Components
	UPROPERTY(BlueprintReadWrite)
	USpringArmComponent * PlayerEyeSpringArm;
	UCameraComponent* PlayerEye;



protected:
	//Input Functions
	void MoveForward(float axisValue);
	void MoveRight(float axisValue);
	void LookRightYawInput(float axisValue);
	void LookUpPitchInput(float axisValue);
	void Dodge();
};
