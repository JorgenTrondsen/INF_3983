#include "ExecCalc_Damage.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/INFGameplayTags.h"
#include "INF_3910/GameplayAbilitySystem/AbilitySystem/INFAttributeSet.h"

struct INFDamageStatics
{
	// Source Captures

	// Target Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);

	INFDamageStatics()
	{
		// Source Defines

		// Target Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(UINFAttributeSet, IncomingDamage, Target, false);
	}
};

static const INFDamageStatics &DamageStatics()
{
	static INFDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// Source Captures

	// Target Captures
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
											  FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
	const FGameplayEffectSpec &EffectSpec = ExecutionParams.GetOwningSpec();

	// Get raw damage value
	float Damage = EffectSpec.GetSetByCallerMagnitude(INFGameplayTags::Combat::Data_Damage);
	Damage = FMath::Max<float>(Damage, 0.f);

	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Additive, Damage));
	}
}