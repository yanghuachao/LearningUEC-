// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include <EnhancedInputSubsystems.h>
#include "EnhancedInputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");//创建相机杆
	CameraBoom->SetupAttachment(RootComponent);//附着到各组件上
	CameraBoom->TargetArmLength = 600.f; 
	CameraBoom->SetRelativeRotation(FRotator(-40.f, 0.f, 0.f));
	//如果想修改根组件
	//SetRootComponent(CameraBoom);
	//相机附着相机杆
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>("Player Camera");
	PlayerCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);


	//1.不要让角色随着控制器旋转
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	//2.相机杆旋转，而相机不旋转
	CameraBoom->bUsePawnControlRotation = true;
	PlayerCamera->bUsePawnControlRotation = false;

	//角色要根据其运动方向进行旋转
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//MyBlueprintFunction();
	
	if(const ULocalPlayer* Player=(GEngine && GetWorld())?GEngine->GetFirstGamePlayer(GetWorld()):nullptr)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(Player);
		if(DefaultMapping)
		{
			Subsystem->AddMappingContext(DefaultMapping, 0);
		}
	}

}

void APlayerCharacter::CallableFunction()
{
	UE_LOG(LogTemp, Warning, TEXT("Callable"));
}

bool APlayerCharacter::CallFunction()
{
	return false;
}

void APlayerCharacter::Move(const FInputActionValue& value)
{
	FVector2D moveVector = value.Get<FVector2D>();
	if(Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, moveVector.X);
		AddMovementInput(RightDirection, moveVector.Y);
	}
}

void APlayerCharacter::Look(const FInputActionValue& value)
{
	FVector2D LookVector = value.Get<FVector2D>();
	if(Controller)
	{
		AddControllerYawInput(LookVector.X);
		AddControllerPitchInput(LookVector.Y);
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	}
}

