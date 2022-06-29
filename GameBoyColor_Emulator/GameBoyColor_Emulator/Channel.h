#pragma once

#include <stdint.h>

#include <xaudio2.h>
#include <mmsystem.h>
#include <mutex>

#include "VoiceCallBack.h"
#include "DebugUtility.h"

using namespace std;

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
	bool sound_enable_flag = false;

	CH_TYPE ch_type;

	IXAudio2* xaudio;
	IXAudio2SourceVoice* source_voice = nullptr;

	WAVEFORMATEX wave_format = { 0 };

	VoiceCallBack* callback;

	mutex mtx;

	bool isPlaying = false;
	bool stop_flag = false;

	uintptr_t stream_thread_handle = NULL;

//#define BUFFER_NUM 2//ストリーミングバッファの数
	bool buffer_no_latch = false;//falseならwave_data_buffer_1, trueならwave_data_buffer_2
	size_t wave_data_size;
	uint8_t* wave_data_buffer_1;
	uint8_t* wave_data_buffer_2;

	bool current_data_ready_flag = false;//供給用バッファにデータがあるか
	uint8_t* current_create_wave_data;//データ供給用のバッファ

	static unsigned int __stdcall play_thread_func(PVOID pv);

	void create_wave_data___none() {//無音を生成する
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			p[i] = 0;
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	void create_wave_data_1(float freq) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float length = wave_format.nSamplesPerSec / freq;//波長
			p[i] = (short)(SHRT_MAX * sinf(i * 3.1415926535f / (length / 2)));
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//デューティ比12.5%
	void create_wave_data___square_wave_12_5(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float length = wave_format.nSamplesPerSec / freq;//波長
			if (sinf(i * 3.1415926535f / (length / 2)) >= 0.75f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//デューティ比25%
	void create_wave_data___square_wave_25(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float length = wave_format.nSamplesPerSec / freq;//波長
			if (sinf(i * 3.1415926535f / (length / 2)) >= 0.5f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//デューティ比50%
	void create_wave_data___square_wave_50(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float length = wave_format.nSamplesPerSec / freq;//波長
			if (sinf(i * 3.1415926535f / (length / 2)) >= 0.0f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//デューティ比75%
	void create_wave_data___square_wave_75(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float length = wave_format.nSamplesPerSec / freq;//波長
			if (sinf(i * 3.1415926535f / (length / 2)) <= -0.5f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	uint8_t* consume_stream_buffer__th() {
		uint8_t* new_buffer_ptr;

		if (buffer_no_latch == false) {
			new_buffer_ptr = wave_data_buffer_1;
		}
		else {
			new_buffer_ptr = wave_data_buffer_2;
		}

		memcpy(new_buffer_ptr, current_create_wave_data, wave_data_size);

		buffer_no_latch = !buffer_no_latch;

		current_data_ready_flag = false;

		return new_buffer_ptr;
	}

public:

	//====================================================

	double CH1_length_counter = 0.0;

	double CH1_envelope_counter = 0.0;
	uint8_t CH1_envelope_volume = 0x0F;//0x0Fが最大0x00が無音

	uint8_t CH1__0xFF10 = 0;
	uint8_t CH1__0xFF11 = 0;
	uint8_t CH1__0xFF12 = 0;
	uint8_t CH1__0xFF13 = 0;
	uint8_t CH1__0xFF14 = 0;

	//====================================================

	double CH2_length_counter = 0.0;

	double CH2_envelope_counter = 0.0;
	uint8_t CH2_envelope_volume = 0x0F;//0x0Fが最大0x00が無音

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

		callback = new VoiceCallBack();

		if (FAILED(hr = xaudio->CreateSourceVoice(&source_voice, &wave_format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback))) {
			M_debug_printf("ERROR CreateSourceVoice\n");
		}

		//wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 60;
		//wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 20;//すこし長めにデータを持たせる(1/20がなめらか)
		wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 20;
		//1/60秒分のデータ
		wave_data_buffer_1 = (uint8_t*)malloc(wave_data_size);
		wave_data_buffer_2 = (uint8_t*)malloc(wave_data_size);
		current_create_wave_data = (uint8_t*)malloc(wave_data_size);

		unsigned int thread_ID;
		stream_thread_handle = _beginthreadex(
			NULL,
			0,
			Channel::play_thread_func,
			(PVOID)this,
			0,
			&thread_ID
		);
	}

	~Channel() {
		mtx.lock();
		stop_flag = true;
		mtx.unlock();

		if (stream_thread_handle != NULL) {
			DWORD exit_code;
			GetExitCodeThread((HANDLE)stream_thread_handle, &exit_code);

			if (exit_code == STILL_ACTIVE) {
				WaitForSingleObject((HANDLE)stream_thread_handle, INFINITE);
			}

			CloseHandle((HANDLE)stream_thread_handle);
			stream_thread_handle = NULL;
		}

		if (source_voice != nullptr) {
			source_voice->DestroyVoice();
		}

		free(wave_data_buffer_1);
		free(wave_data_buffer_2);
		free(current_create_wave_data);

		delete callback;
	}

	void execute() {
		//M_debug_printf("Channel::execute()\n");

		//create_wave_data_1(440.0f);

		if (ch_type == CH_TYPE::CH1) {
			uint32_t freq = (CH1__0xFF13 | ((CH1__0xFF14 & 0b00000111) << 8));
			float freq_f = 131072.0f / (2048.0f - (float)freq);

			if (sound_enable_flag == true) {
				if ((CH1__0xFF12 & 0b00000111) != 0) {
					const uint8_t TMP_SPEED = (CH1__0xFF12 & 0b111);
					const double ENVELOPE_SPEED = (double)TMP_SPEED * (1.0 / 64.0);
					CH1_envelope_counter += (1.0 / 60.0);
					while (CH1_envelope_counter >= ENVELOPE_SPEED) {
						CH1_envelope_counter -= ENVELOPE_SPEED;

						if ((CH1__0xFF12 & 0b00001000) != 0) {//大きくなっていく
							if (CH1_envelope_volume < 0x0F) {
								CH1_envelope_volume++;
							}
						}
						else {//小さくなっていく
							if (CH1_envelope_volume > 0) {
								CH1_envelope_volume--;
							}
						}
					}

				}


				if ((CH1__0xFF14 & 0b01000000) != 0) {//長さカウンタ有効フラグが有効なとき
					const uint8_t TMP_LENGTH = (CH1__0xFF11 & 0b00111111);
					const double SOUND_LENGTH = (64.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
					CH1_length_counter += (1.0 / 60.0);
					if (CH1_length_counter >= SOUND_LENGTH) {
						sound_enable_flag = false;
					}
				}
			}

			//リスタート
			if ((CH1__0xFF14 & 0b10000000) != 0) {//restart flag
				if ((CH1__0xFF11 & 0b00111111) == 0) {
					CH1_envelope_volume = ((CH1__0xFF12 >> 4) & 0b1111);

					CH1_length_counter = 0;

					sound_enable_flag = true;
				}
			}

			if (sound_enable_flag == true) {
				uint8_t duty_type = ((CH1__0xFF11 & 0b11000000) >> 6);
				if (duty_type == 0b00) {
					create_wave_data___square_wave_12_5(freq_f, (CH1_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b01) {
					create_wave_data___square_wave_25(freq_f, (CH1_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b10) {
					create_wave_data___square_wave_50(freq_f, (CH1_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b11) {
					create_wave_data___square_wave_75(freq_f, (CH1_envelope_volume & 0b1111));
				}
			}
			else {
				create_wave_data___none();//無音
			}
		}
		else if (ch_type == CH_TYPE::CH2) {
			uint32_t freq = (CH2__0xFF18 | ((CH2__0xFF19 & 0b00000111) << 8));
			float freq_f = 131072.0f / (2048.0f - (float)freq);
			
			if (sound_enable_flag == true) {
				if ((CH2__0xFF17 & 0b00000111) != 0) {
					const uint8_t TMP_SPEED = (CH2__0xFF17 & 0b111);
					const double ENVELOPE_SPEED = (double)TMP_SPEED * (1.0 / 64.0);
					CH2_envelope_counter += (1.0 / 60.0);
					while (CH2_envelope_counter >= ENVELOPE_SPEED) {
						CH2_envelope_counter -= ENVELOPE_SPEED;

						if ((CH2__0xFF17 & 0b00001000) != 0) {//大きくなっていく
							if (CH2_envelope_volume < 0x0F) {
								CH2_envelope_volume++;
							}
						}
						else {//小さくなっていく
							if (CH2_envelope_volume > 0) {
								CH2_envelope_volume--;
							}
						}
					}

				}



				if ((CH2__0xFF19 & 0b01000000) != 0) {//長さカウンタ有効フラグが有効なとき
					const uint8_t TMP_LENGTH = (CH2__0xFF16 & 0b00111111);
					const double SOUND_LENGTH = (64.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
					CH2_length_counter += (1.0 / 60.0);
					if (CH2_length_counter >= SOUND_LENGTH) {
						sound_enable_flag = false;
					}
				}
			}

			//リスタート
			if ((CH2__0xFF19 & 0b10000000) != 0) {//restart flag
				if ((CH2__0xFF16 & 0b00111111) == 0) {
					CH2_envelope_volume = ((CH2__0xFF17 >> 4) & 0b1111);

					CH2_length_counter = 0;

					sound_enable_flag = true;
				}
			}

			if (sound_enable_flag == true) {
				uint8_t duty_type = ((CH2__0xFF16 & 0b11000000) >> 6);
				if (duty_type == 0b00) {
					create_wave_data___square_wave_12_5(freq_f, (CH2_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b01) {
					create_wave_data___square_wave_25(freq_f, (CH2_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b10) {
					create_wave_data___square_wave_50(freq_f, (CH2_envelope_volume & 0b1111));
				}
				else if (duty_type == 0b11) {
					create_wave_data___square_wave_75(freq_f, (CH2_envelope_volume & 0b1111));
				}
			}
			else {
				create_wave_data___none();//無音
			}
		}
		else if (ch_type == CH_TYPE::CH3) {
			create_wave_data___none();//無音
		}
		else {//CH4
			create_wave_data___none();//無音
		}
	}

	void execute_sleep() {
		create_wave_data___none();//無音
	}

	bool is_playing() {
		//return false;
		//return isPlaying;
		return sound_enable_flag;
	}

};

