#pragma once

#include "CoreMinimal.h"
#include "WidgetController.h"
#include "Interfaces/IHttpRequest.h"
#include "DialogueWidgetController.generated.h"

class ANPCharacter;
class IHttpRequest;
class IHttpResponse;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNPCNameChanged, const FString &, NPCName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTextChanged, const FString &, DialogueText);

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

    UPROPERTY()
    TObjectPtr<ANPCharacter> CurrentNPC;

    // Called when player submits text input
    UFUNCTION(BlueprintCallable)
    void SubmitPlayerInput(const FText &PlayerText);

    // API URL for the LLM service
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|API")
    FString APIURL = TEXT("https://desktop-aoartue.tail892650.ts.net/v1/chat/completions");

    // Model name to use for LLM requests
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue|API")
    FString ModelName = TEXT("cpatonn/Qwen3-30B-A3B-Instruct-2507-AWQ-4bit");

    // Clear the conversation history
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void ClearConversationHistory();

private:
    // Send HTTP request to LLM API
    void SendLLMRequest(const FString &UserMessage);

    // Handle HTTP response when complete
    void OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);

    // Handle streaming data as it arrives (called multiple times)
    void OnHttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived);

    // Parse streaming SSE data
    void ParseStreamingData(const FString &StreamData);

    // Conversation history to maintain context
    TArray<TPair<FString, FString>> ConversationHistory; // Pairs of (role, content)

    // Buffer to accumulate the complete dialogue response
    FString DialogueBuffer;

    // Buffer for incomplete streaming chunks
    FString StreamBuffer;

    // Track total bytes processed to only parse new data
    uint64 LastProcessedBytes;

    // Keep reference to active request
    TSharedPtr<IHttpRequest> ActiveRequest;
};
