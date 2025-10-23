#include "ExecCalc_Damage.h"
#include "INF_3910/AbilitySystem/GameplayTags.h"
#include "INF_3910/AbilitySystem/INFAttributeSet.h"

struct INFDamageStatics
{
	// Source Captures

	// Target Captures
	DECLARE_ATTRIBUTE_CAPTUREDEF(IncomingDamage);

	// Constructor initializes attribute capture definitions
	INFDamageStatics()
	{
		// Source Defines

		// Target Defines
		DEFINE_ATTRIBUTE_CAPTUREDEF(UINFAttributeSet, IncomingDamage, Target, false);
	}
};

// Returns singleton instance of damage statics
static const INFDamageStatics &DamageStatics()
{
	static INFDamageStatics DStatics;
	return DStatics;
}

// Constructor sets up relevant attributes to capture
UExecCalc_Damage::UExecCalc_Damage()
{
	// Source Captures

	// Target Captures
	RelevantAttributesToCapture.Add(DamageStatics().IncomingDamageDef);
}

// Executes damage calculation and applies to target
void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters &ExecutionParams,
											  FGameplayEffectCustomExecutionOutput &OutExecutionOutput) const
{
	const FGameplayEffectSpec &EffectSpec = ExecutionParams.GetOwningSpec();

	float Damage = EffectSpec.GetSetByCallerMagnitude(GameplayTags::Combat::Data_Damage);
	Damage = FMath::Max<float>(Damage, 0.f);

	if (Damage > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(DamageStatics().IncomingDamageProperty, EGameplayModOp::Additive, Damage));
	}
}