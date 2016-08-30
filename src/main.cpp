#include "ofApp.h"

bool initvoice() {
	// Find the best matching installed en-US recognizer.
	CComPtr<ISpObjectToken> cpRecognizerToken;
	HRESULT hr;

	hr = SpFindBestToken(SPCAT_RECOGNIZERS, L"language=409", NULL, &cpRecognizerToken);

	// Create the in-process recognizer and immediately set its state to inactive.
	CComPtr<ISpRecognizer> cpRecognizer;

	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer.CoCreateInstance(CLSID_SpInprocRecognizer);
	}

	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer->SetRecognizer(cpRecognizerToken);
	}

	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer->SetRecoState(SPRST_INACTIVE);
	}

	// Create a new recognition context from the recognizer.
	CComPtr<ISpRecoContext> cpContext;

	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer->CreateRecoContext(&cpContext);
	}

	// Subscribe to the speech recognition event and end stream event.
	if (SUCCEEDED(hr))
	{
		ULONGLONG ullEventInterest = SPFEI(SPEI_RECOGNITION) | SPFEI(SPEI_END_SR_STREAM);
		hr = cpContext->SetInterest(ullEventInterest, ullEventInterest);
	}

	// Establish a Win32 event to signal when speech events are available.
	HANDLE hSpeechNotifyEvent = INVALID_HANDLE_VALUE;

	if (SUCCEEDED(hr))
	{
		hr = cpContext->SetNotifyWin32Event();
	}

	if (SUCCEEDED(hr))
	{
		hSpeechNotifyEvent = cpContext->GetNotifyEventHandle();

		if (INVALID_HANDLE_VALUE == hSpeechNotifyEvent)
		{
			// Notification handle unsupported
			//hr = SPERR_UNITIALIZED;
		}
	}

	// Set up an audio input stream using a .wav file and set the recognizer's input.
	CComPtr<ISpStream> cpInputStream;

	if (SUCCEEDED(hr))
	{
		hr = SPBindToFile(L"Test.wav", SPFM_OPEN_READONLY, &cpInputStream);
	}

	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer->SetInput(cpInputStream, TRUE);
	}

	// Create a new grammar and load an SRGS grammar from file.
	CComPtr<ISpRecoGrammar> cpGrammar;

	if (SUCCEEDED(hr))
	{
		hr = cpContext->CreateGrammar(0, &cpGrammar);
	}

	if (SUCCEEDED(hr))
	{
		hr = cpGrammar->LoadCmdFromFile(L"SRGS_Grammar.grxml", SPLO_STATIC);
	}

	// Set all top-level rules in the new grammar to the active state.
	if (SUCCEEDED(hr))
	{
		hr = cpGrammar->SetRuleState(NULL, NULL, SPRS_ACTIVE);
	}

	// Finally, set the recognizer state to active to begin recognition.
	if (SUCCEEDED(hr))
	{
		hr = cpRecognizer->SetRecoState(SPRST_ACTIVE_ALWAYS);
	}
	return true;
}
bool loadgrammers()
{
	// Create a new grammar and load an SRGS grammar from a file.
	CComPtr<ISpRecoContext>    cpContext;
	CComPtr<ISpRecoGrammar>    cpGrammar;
	HRESULT hr;
	if (SUCCEEDED(hr))
	{
		hr = cpContext->CreateGrammar(0, &cpGrammar);
	}

	if (SUCCEEDED(hr))
	{
		hr = cpGrammar->LoadCmdFromFile(L"TestGrammar.grxml", SPLO_STATIC);
	}

	// Activate the grammar.
	if (SUCCEEDED(hr))
	{
		hr = cpGrammar->SetGrammarState(SPGS_ENABLED);
	}

	if (SUCCEEDED(hr))
	{
		// Create and configure a recognizer to begin recognition.
	}
	return true;
}
bool setvoice(const wstring & voicetype= L"Gender=Female;") {
	HRESULT hr = S_OK;
	CComPtr<IEnumSpObjectTokens> cpIEnum;
	CComPtr<ISpObjectToken> cpToken;
	CComPtr<ISpVoice> cpVoice;

	// Enumerate voice tokens that speak US English in a female voice.
	hr = SpEnumTokens(SPCAT_VOICES, L"Language=409", voicetype.c_str(), &cpIEnum);

	// Get the best matching token.
	if (SUCCEEDED(hr))	{
		hr = cpIEnum->Next(1, &cpToken , NULL);
	}

	// Create a voice and set its token to the one we just found.
	if (SUCCEEDED(hr))	{
		hr = cpVoice.CoCreateInstance(CLSID_SpVoice);
	}

	if (SUCCEEDED(hr))	{
		hr = cpVoice->SetVoice(cpToken);
	}	
	
	cpVoice->Speak(L"What did the artist say to the dentist?...", 0, NULL);
	cpVoice->Speak(L"Matisse hurt!", 0, NULL);
	return true;
}
int main() {

	::CoInitialize(NULL);
	//setvoice();
	//initvoice();
	//ISpVoice * pVoice = NULL;
	//HRESULT hr = CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_ALL, IID_ISpVoice, (void **)&pVoice);
	//if (SUCCEEDED(hr))	{
		//hr = SpCreateBestObject(SPCAT_VOICES, L"Gender = Female", NULL, &pVoice);
		//hr = pVoice->Speak(L"Hello world", 0, NULL);
		//pVoice->Release();
		//pVoice = NULL;
		//::CoUninitialize();
	//}

	ofSetupOpenGL(2000, 1000, OF_WINDOW);
	ofRunApp(new ofApp());
}
