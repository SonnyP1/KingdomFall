// Fill out your copyright notice in the Description page of Project Settings.


#include "KingdomFallsCharacter.h"


#include "GASGameplayAbility.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AKingdomFallsCharacter::AKingdomFallsCharacter()
{
	//Make it where it only affect camera rotations not controller rotation
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	
	//Spring Arm Stuff
	PlayerEyeSpringArm = CreateDefaultSubobject<USpringArmComponent>("PlayerEyeSpringArm");
	PlayerEyeSpringArm->SetupAttachment(GetRootComponent());
	PlayerEyeSpringArm->TargetArmLength = 300.0f;
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

}

// Called to bind functionality to input
void AKingdomFallsCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis(TEXT("MoveForward"),this, &AKingdomFallsCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("MoveRight"),this, &AKingdomFallsCharacter::MoveRight);
	PlayerInputComponent->BindAxis(TEXT("LookRight"),this, &AKingdomFallsCharacter::LookRightYawInput);
	PlayerInputComponent->BindAxis(TEXT("LookUp"),this, &AKingdomFallsCharacter::LookUpPitchInput);
	
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
	AddMovementInput(ForwardDir,axisValue);
}
void AKingdomFallsCharacter::MoveRight(float axisValue)
{
	FVector RightDir= PlayerEye->GetRightVector();
	AddMovementInput(RightDir,axisValue);
}

void AKingdomFallsCharacter::LookRightYawInput(float axisValue)
{
	AddControllerYawInput(axisValue);
}

void AKingdomFallsCharacter::LookUpPitchInput(float axisValue)
{
	AddControllerPitchInput(axisValue);
}