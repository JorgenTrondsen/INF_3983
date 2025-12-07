#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "Interfaces/IHttpRequest.h"
#include "DialogueWidgetController.generated.h"

class IHttpRequest;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCNameChanged, const FString &, NPCName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTextChanged, const FString &, DialogueText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTalkStart, bool, bIsTalking);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTalkStop, bool, bIsTalking);

/**
 * Widget controller for managing dialogue UI interactions with NPCs
 */
UCLASS(BlueprintType, Blueprintable)
class INF_3910_API UDialogueWidgetController : public UWidgetController
{
    GENERATED_BODY()

public:
    // Called to broadcast initial values to the widget
    UFUNCTION(BlueprintCallable)
    void BroadcastInitialValues();

    // Delegates to notify widget of changes
    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnNPCNameChanged OnNPCNameChanged;
    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnDialogueTextChanged OnDialogueTextChanged;
    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnTalkStart OnTalkStart;
    UPROPERTY(BlueprintAssignable, Category = "Dialogue")
    FOnTalkStop OnTalkStop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|NPC")
    FString NPCName;

    UFUNCTION(BlueprintCallable)
    void SubmitPlayerInput(const FText &PlayerText);

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|API")
    FString APIURL = TEXT("https://jorg1-4090.tail892650.ts.net/v1/chat/completions");

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|API")
    FString ModelName = TEXT("cpatonn/Qwen3-30B-A3B-Instruct-2507-AWQ-4bit");

    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void ClearConversationHistory();

private:
    void SendLLMRequest(const FString &UserMessage);
    void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
    void OnHttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived);
    void ParseStreamingData(const FString &StreamData);

    TArray<TPair<FString, FString>> ConversationHistory; // Pairs of (role, content)
    FString DialogueBuffer;
    FString StreamBuffer;
    uint64 LastProcessedBytes;
    TSharedPtr<IHttpRequest> ActiveRequest;
    bool bIsTalking = false;
    bool bIsThinking = false;
    int32 ThinkingAnimationIndex = 0;
};
