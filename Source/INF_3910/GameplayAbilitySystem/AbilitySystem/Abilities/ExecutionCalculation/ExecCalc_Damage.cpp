#include "ExecCalc_Damage.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/GASGameplayTags.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/GASAttributeSet.h"

struct GASDamageStatics
{
	// Source Captures

	// Target Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);
	
	GASDamageStatics()
	{
		// Source Defines

		// Target Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(UGASAttributeSet, IncomingDamage, Target, false);
	}
};

static const GASDamageStatics& DamageStatics()
{
	static GASDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Source Captures

	// Target Captures
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec = ExecutionParams.GetOwningSpec();

	// Get raw damage value
	float Damage = EffectSpec.GetSetByCallerMagnitude(GASGameplayTags::Combat::Data_Damage);
	Damage = FMath::Max<float>(Damage, 0.f);

	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Additive, Damage));
	}
}