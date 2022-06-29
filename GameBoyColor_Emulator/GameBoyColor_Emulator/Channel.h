#pragma once

#include <stdint.h>

#include <xaudio2.h>
#include <mmsystem.h>

#include "DebugUtility.h"

class Channel
{
public:
	enum class CH_TYPE {
		CH1,
		CH2,
		CH3,
		CH4,
	};

private:
	CH_TYPE ch_type;

	IXAudio2* xaudio;
	IXAudio2SourceVoice* source_voice;

	WAVEFORMATEX wave_format = { 0 };

public:

	//====================================================

	uint8_t CH1__0xFF10 = 0;
	uint8_t CH1__0xFF11 = 0;
	uint8_t CH1__0xFF12 = 0;
	uint8_t CH1__0xFF13 = 0;
	uint8_t CH1__0xFF14 = 0;

	//====================================================

	uint8_t CH2__0xFF16 = 0;
	uint8_t CH2__0xFF17 = 0;
	uint8_t CH2__0xFF18 = 0;
	uint8_t CH2__0xFF19 = 0;

	//====================================================

	uint8_t CH3__0xFF1A = 0;
	uint8_t CH3__0xFF1B = 0;
	uint8_t CH3__0xFF1C = 0;
	uint8_t CH3__0xFF1D = 0;
	uint8_t CH3__0xFF1E = 0;

	uint8_t CH3__0xFF30_0xFF3F[16] = { 0 };

	//====================================================

	uint8_t CH4__0xFF20 = 0;
	uint8_t CH4__0xFF21 = 0;
	uint8_t CH4__0xFF22 = 0;
	uint8_t CH4__0xFF23 = 0;

	//====================================================

	Channel(CH_TYPE ch_type, IXAudio2* xaudio) : ch_type(ch_type), xaudio(xaudio) {
		HRESULT hr;

		wave_format.wFormatTag = WAVE_FORMAT_PCM;
		wave_format.nChannels = 1;//チャンネル数
		wave_format.wBitsPerSample = 16;//１サンプルあたりのビット数
		wave_format.nSamplesPerSec = 44100;//サンプリング周波数
		wave_format.nBlockAlign = wave_format.wBitsPerSample / 8 * wave_format.nChannels;
		wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;

		if (FAILED(hr = xaudio->CreateSourceVoice(&source_voice, &wave_format))) {
			M_debug_printf("ERROR CreateSourceVoice\n");
		}
	}

	~Channel() {
	}

	void execute() {

	}

	bool is_playing() {
		return false;
	}

};

