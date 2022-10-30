#pragma once
#include "CharacterBase.h"
#include "FirstPersonCharacter.generated.h"

UCLASS(config=Game)
class AFirstPersonCharacter : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	AFirstPersonCharacter();

	virtual UAnimInstance* GetAnimationInstance() const override;
	virtual UCameraComponent* GetCamera() const override { return mFirstPersonCameraComponent; }

	/** Returns Mesh1P subobject **/
	//USkeletalMeshComponent* GetMesh1P() const { return mCharacterMesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return mFirstPersonCameraComponent; }

	UFUNCTION(BlueprintCallable, Category = "First Person Character")
	void ConvertIntoMesh1P() const;
	
	UFUNCTION(BlueprintCallable, Category = "First Person Character")
	void ChangeCharacterMesh1PColor(const FColor& color);
	
private:
	void DoInitialize(bool isLocallyControlled);
	void HideMesh() const;
	virtual USkeletalMeshComponent* GetCurrentMesh(bool isLocallyControlled) const;
	virtual void DoDieClient();
	
	UPROPERTY()
	UMaterialInstanceDynamic* mCharacterMesh1PMaterial = nullptr;
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	//UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	//USkeletalMeshComponent* mCharacterMesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* mFirstPersonCameraComponent;
};
