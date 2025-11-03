#include "DialogueWidgetController.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

void UDialogueWidgetController::BroadcastInitialValues()
{
    // Broadcast NPC name when dialogue starts
    OnNPCNameChanged.Broadcast(NPCName);

    // You can broadcast an initial greeting here
    FString InitialGreeting = FString::Printf(TEXT("Hello! I'm %s. How can I help you?"), *NPCName);
    OnDialogueTextChanged.Broadcast(InitialGreeting);
}

void UDialogueWidgetController::SubmitPlayerInput(const FText &PlayerText)
{
    if (PlayerText.IsEmpty())
        return;

    // Add user message to conversation history
    ConversationHistory.Add(TPair<FString, FString>(TEXT("user"), PlayerText.ToString()));

    // Clear the dialogue buffer for new response
    DialogueBuffer.Empty();
    StreamBuffer.Empty();
    LastProcessedBytes = 0;
    bIsStreaming = false;

    SendLLMRequest(PlayerText.ToString());
}

void UDialogueWidgetController::ClearConversationHistory()
{
    ConversationHistory.Empty();
}

void UDialogueWidgetController::SendLLMRequest(const FString &UserMessage)
{
    TSharedRef<IHttpRequest> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(APIURL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

    // Build JSON payload
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
    JsonObject->SetStringField(TEXT("model"), ModelName);
    JsonObject->SetBoolField(TEXT("stream"), true);

    // Build messages array with system message (if NPC exists) + conversation history
    TArray<TSharedPtr<FJsonValue>> MessagesArray;

    TSharedPtr<FJsonObject> SystemMsg = MakeShareable(new FJsonObject());
    SystemMsg->SetStringField(TEXT("role"), TEXT("system"));
    SystemMsg->SetStringField(TEXT("content"), FString::Printf(TEXT("You are %s. Keep responses concise (2-3 sentences)."), *NPCName));
    MessagesArray.Add(MakeShareable(new FJsonValueObject(SystemMsg)));

    for (const auto &Msg : ConversationHistory)
    {
        TSharedPtr<FJsonObject> HistoryMsg = MakeShareable(new FJsonObject());
        HistoryMsg->SetStringField(TEXT("role"), Msg.Key);
        HistoryMsg->SetStringField(TEXT("content"), Msg.Value);
        MessagesArray.Add(MakeShareable(new FJsonValueObject(HistoryMsg)));
    }

    JsonObject->SetArrayField(TEXT("messages"), MessagesArray);

    // Serialize and send
    FString JsonString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&JsonString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);
    HttpRequest->SetContentAsString(JsonString);

    ActiveRequest = HttpRequest;
    HttpRequest->OnRequestProgress64().BindUObject(this, &UDialogueWidgetController::OnHttpRequestProgress64);
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDialogueWidgetController::OnHttpResponseReceived);
    HttpRequest->ProcessRequest();
}

void UDialogueWidgetController::OnHttpRequestProgress64(FHttpRequestPtr Request, uint64 BytesSent, uint64 BytesReceived)
{
    if (!Request.IsValid() || BytesReceived <= LastProcessedBytes)
        return;

    FHttpResponsePtr Response = Request->GetResponse();
    if (!Response.IsValid())
        return;

    FString ResponseContent = Response->GetContentAsString();
    if (ResponseContent.Len() > LastProcessedBytes)
    {
        ParseStreamingData(ResponseContent.RightChop(LastProcessedBytes));
        LastProcessedBytes = ResponseContent.Len();
    }
}

void UDialogueWidgetController::OnHttpResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    ActiveRequest.Reset();

    if (!bWasSuccessful || !Response.IsValid())
    {
        OnDialogueTextChanged.Broadcast(TEXT("Error: Failed to connect to AI service."));
        OnStreamStop.Broadcast(false);
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        OnDialogueTextChanged.Broadcast(FString::Printf(TEXT("Error: Server returned code %d"), Response->GetResponseCode()));
        OnStreamStop.Broadcast(false);
        return;
    }

    // Process any remaining data
    FString ResponseContent = Response->GetContentAsString();
    if (ResponseContent.Len() > LastProcessedBytes)
        ParseStreamingData(ResponseContent.RightChop(LastProcessedBytes));

    // Save assistant's response to history
    if (!DialogueBuffer.IsEmpty())
        ConversationHistory.Add(TPair<FString, FString>(TEXT("assistant"), DialogueBuffer));

    // Broadcast that streaming has stopped
    OnStreamStop.Broadcast(false);
}

void UDialogueWidgetController::ParseStreamingData(const FString &StreamData)
{
    StreamBuffer += StreamData;

    TArray<FString> Lines;
    StreamBuffer.ParseIntoArray(Lines, TEXT("\n"), false);

    // Keep incomplete line in buffer
    if (!StreamBuffer.EndsWith(TEXT("\n")))
    {
        StreamBuffer = Lines.Last();
        Lines.RemoveAt(Lines.Num() - 1);
    }
    else
    {
        StreamBuffer.Empty();
    }

    for (const FString &Line : Lines)
    {
        if (!Line.StartsWith(TEXT("data: ")))
            continue;

        FString JsonData = Line.RightChop(6);
        if (JsonData.TrimStartAndEnd().Equals(TEXT("[DONE]")))
            continue;

        TSharedPtr<FJsonObject> JsonObject;
        TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonData);

        if (FJsonSerializer::Deserialize(Reader, JsonObject) && JsonObject.IsValid())
        {
            const TArray<TSharedPtr<FJsonValue>> *ChoicesArray;
            if (JsonObject->TryGetArrayField(TEXT("choices"), ChoicesArray) && ChoicesArray->Num() > 0)
            {
                TSharedPtr<FJsonObject> Delta = (*ChoicesArray)[0]->AsObject()->GetObjectField(TEXT("delta"));
                if (Delta.IsValid())
                {
                    FString Content;
                    if (Delta->TryGetStringField(TEXT("content"), Content))
                    {
                        DialogueBuffer += Content;
                        OnDialogueTextChanged.Broadcast(DialogueBuffer);

                        if (!bIsStreaming)
                        {
                            bIsStreaming = true;
                            OnStreamStart.Broadcast(true);
                        }
                    }
                }
            }
        }
    }
}
