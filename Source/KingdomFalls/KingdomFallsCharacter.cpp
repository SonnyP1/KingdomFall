// Fill out your copyright notice in the Description page of Project Settings.


#include "KingdomFallsCharacter.h"


#include "GASGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AKingdomFallsCharacter::AKingdomFallsCharacter()
{
	//Init Variables
	bIsLockOn = false;
	_lookMultipler = 1;
	_moveMultipler = 1;

	//Make it where it only affect camera rotations not controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//Spring Arm Stuff
	PlayerEyeSpringArm = CreateDefaultSubobject<USpringArmComponent>("PlayerEyeSpringArm");
	PlayerEyeSpringArm->SetupAttachment(GetRootComponent());
	PlayerEyeSpringArm->TargetArmLength = 400.0f;
	PlayerEyeSpringArm->TargetOffset = FVector(0,0,50);
	PlayerEyeSpringArm->bUsePawnControlRotation = true;

	//Camera
	PlayerEye= CreateDefaultSubobject<UCameraComponent>("PlayerEye");
	PlayerEye->SetupAttachment(PlayerEyeSpringArm, USpringArmComponent::SocketName);
	PlayerEye->bUsePawnControlRotation = false;

	//Configure Controller Movements
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0,600,0);
	GetCharacterMovement()->AirControl = 0.0f;

	//Ability Stuff
	PlayerAbilitySystemComponent = CreateDefaultSubobject<UGASAbilitySystemComponent>("AbilitySystemComp");
		//Save this when loading levels
	PlayerAttributes = CreateDefaultSubobject<UGASAttributeSet>("Attributes");
}

// Called when the game starts or when spawned
void AKingdomFallsCharacter::BeginPlay()
{
	
	PlayerAbilitySystemComponent->InitAbilityActorInfo(this,this);
	timeLine = FTimeline{};
	FOnTimelineFloat func{};
	func.BindUFunction(this, "OnCameraTurnUpdate");
	timeLine.AddInterpFloat(CenterCamCurveFloat, func);
	
	InitializeAttributes();
	GiveAbilities();

	if(PlayerAbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm","Cancel","EGASAbilityInputID",
			static_cast<int32>(EGASAbilityInputID::Confirm),
			static_cast<int32>(EGASAbilityInputID::Cancel));

		PlayerAbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,Binds);
	}
	Super::BeginPlay();

}

// Called every frame
void AKingdomFallsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	timeLine.TickTimeline(DeltaTime);

	if (bIsLockOn)
	{
		float Speed = GetCharacterMovement()->Velocity.Size();
		if (Speed > 675.0f || bIsDodging)
		{
			bUseControllerRotationYaw = false;
		}
		else
		{
			bUseControllerRotationYaw = true;
		}

		if (lockOnTarget != NULL)
		{
			FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),FVector(lockOnTarget->GetActorLocation().X,lockOnTarget->GetActorLocation().Y,lockOnTarget->GetActorLocation().Z-150.0));
			FRotator InterpTo = UKismetMathLibrary::RInterpTo(GetControlRotation(), LookAtRot, GetWorld()->DeltaTimeSeconds, 5.0f);
			FRotator rotToSet = UKismetMathLibrary::MakeRotator(GetControlRotation().Roll,InterpTo.Pitch,InterpTo.Yaw);
			GetController()->SetControlRotation(rotToSet);
		}
	}

}

// Called to bind functionality to input
void AKingdomFallsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this, &AKingdomFallsCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this, &AKingdomFallsCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"),this, &AKingdomFallsCharacter::LookRightYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),this, &AKingdomFallsCharacter::LookUpPitchInput);
	PlayerInputComponent->BindAction(TEXT("Sprint"),IE_Released,this,&AKingdomFallsCharacter::SprintReleased);
	PlayerInputComponent->BindAction(TEXT("Shield"),IE_Released,this,&AKingdomFallsCharacter::BlockingReleased);
	PlayerInputComponent->BindAction(TEXT("LockOn"), IE_Pressed, this, &AKingdomFallsCharacter::LockOnPressed);
	PlayerInputComponent->BindAction(TEXT("Punch"), IE_Pressed, this, &AKingdomFallsCharacter::AttackPressed);

	if(PlayerAbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm","Cancel","EGASAbilityInputID",
            static_cast<int32>(EGASAbilityInputID::Confirm),
            static_cast<int32>(EGASAbilityInputID::Cancel));

		PlayerAbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent,Binds);
	}
}

UAbilitySystemComponent* AKingdomFallsCharacter::GetAbilitySystemComponent() const
{
	return PlayerAbilitySystemComponent;
}

void AKingdomFallsCharacter::InitializeAttributes()
{
	if(PlayerAbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = PlayerAbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = PlayerAbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect,1,EffectContext);

		if(SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = PlayerAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AKingdomFallsCharacter::GiveAbilities()
{
	if(PlayerAbilitySystemComponent)
	{
		for(TSubclassOf<UGASGameplayAbility>& StartupAbility : DefaultAbilities)
		{
			PlayerAbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(StartupAbility,1,static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID),this));
		}
	}
}

//******************************************INPUTS***************************************************
void AKingdomFallsCharacter::MoveForward(float axisValue)
{
	FVector ForwardDir = GetControlRotation().Vector();
	AddMovementInput(ForwardDir,axisValue* _moveMultipler);
}
void AKingdomFallsCharacter::MoveRight(float axisValue)
{
	FVector RightDir= PlayerEye->GetRightVector();
	AddMovementInput(RightDir,axisValue*_moveMultipler);
}

void AKingdomFallsCharacter::LookRightYawInput(float axisValue)
{
	AddControllerYawInput(axisValue*_lookMultipler);
}

void AKingdomFallsCharacter::LookUpPitchInput(float axisValue)
{
	AddControllerPitchInput(axisValue*_lookMultipler);
}

void AKingdomFallsCharacter::SprintReleased()
{
	CancelSprint();
}

void AKingdomFallsCharacter::BlockingReleased()
{
	CancelBlocking();
}

void AKingdomFallsCharacter::AttackPressed()
{
	UE_LOG(LogTemp, Warning, TEXT("ATTACK"));
	HandleAttack();
}


void AKingdomFallsCharacter::LockOnPressed()
{
	FVector forwardVectorOfPlayerEye;
	FVector actorLoc = GetActorLocation();
	forwardVectorOfPlayerEye = PlayerEye->GetForwardVector();

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));

	TArray<AActor*, FDefaultAllocator> ActorToIgnore;
	ActorToIgnore.Add(this);


	FHitResult OutHit;
	UKismetSystemLibrary::SphereTraceSingleForObjects(
		GetWorld(), 
		actorLoc, actorLoc+(forwardVectorOfPlayerEye*2000.0f), 300.f,
		ObjectTypesArray, false, ActorToIgnore, 
		EDrawDebugTrace::None, OutHit, true);
	
	if (OutHit.IsValidBlockingHit() && bIsLockOn == false)
	{
		//UE_LOG(LogTemp, Warning, TEXT("This is the target %s"), *OutHit.GetActor()->GetName());
		lockOnTarget = OutHit.GetActor();
		bIsLockOn = true;
		_lookMultipler = 0;
		bUseControllerRotationYaw = true;
		UpdateTargetUIWidget(false);
	}
	else
	{
		QuickTurnCamera(bIsLockOn);
		UpdateTargetUIWidget(true);
		lockOnTarget = NULL;
		bIsLockOn = false;
		_lookMultipler = 1;
		bUseControllerRotationYaw = false;
	}
}


void AKingdomFallsCharacter::QuickTurnCamera(bool turn)
{
	if (!turn)
	{
		ActorTurnStartRot = GetControlRotation();
		ActorOrignalRoatation = GetActorRotation();
		timeLine.PlayFromStart();
	}
}

void AKingdomFallsCharacter::TurnOffInputs()
{
	_lookMultipler = 0;
	_moveMultipler = 0;
}

void AKingdomFallsCharacter::OnCameraTurnUpdate(float val)
{
	
	FRotator goalRot = UKismetMathLibrary::RLerp(ActorTurnStartRot, ActorOrignalRoatation, val, true);
	GetController()->SetControlRotation(goalRot);
	if (val == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("lerp end time: %f"), GetWorld()->TimeSeconds);
	}
}
