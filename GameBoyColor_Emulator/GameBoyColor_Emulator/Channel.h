#pragma once

#include <stdint.h>

#include <math.h>

#include <xaudio2.h>
#include <mmsystem.h>
#include <mutex>

#include "VoiceCallBack.h"
#include "DebugUtility.h"
#include "My_Random.h"

using namespace std;

#define CPU_FREQ_D 4194304.0//CPU�̎��g��(hz)

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
	const double SWEEP_INTERVAL_TIME_LIST[7] = {
		7.8 / 1000.0,
		15.6 / 1000.0,
		23.4 / 1000.0,
		31.3 / 1000.0,
		39.1 / 1000.0,
		46.9 / 1000.0,
		54.7 / 1000.0,
	};

	bool sound_enable_flag = false;

	CH_TYPE ch_type;

	IXAudio2* xaudio;
	IXAudio2SourceVoice* source_voice = nullptr;

	WAVEFORMATEX wave_format = { 0 };

	VoiceCallBack* callback;

	mutex mtx;

	float freq_f;

	//bool isPlaying = false;
	bool stop_flag = false;

	uintptr_t stream_thread_handle = NULL;

//#define BUFFER_NUM 2//�X�g���[�~���O�o�b�t�@�̐�
	bool buffer_no_latch = false;//false�Ȃ�wave_data_buffer_1, true�Ȃ�wave_data_buffer_2
	size_t wave_data_size;
	uint8_t* wave_data_buffer_1;
	uint8_t* wave_data_buffer_2;

	bool current_data_ready_flag = false;//�����p�o�b�t�@�Ƀf�[�^�����邩
	uint8_t* current_create_wave_data;//�f�[�^�����p�̃o�b�t�@

	static unsigned int __stdcall play_thread_func(PVOID pv);

	void create_wave_data___none() {//�����𐶐�����
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;

		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			p[i] = 0;
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//void create_wave_data_1(float freq) {
	//	mtx.lock();
	//
	//	short* p;
	//	p = (short*)current_create_wave_data;
	//
	//
	//	for (size_t i = 0; i < (wave_data_size / 2); i++) {
	//		float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��
	//		p[i] = (short)(SHRT_MAX * sinf(i * D3DX_PI / (wave_1cycle_length / 2)));
	//	}
	//
	//	current_data_ready_flag = true;
	//
	//	mtx.unlock();
	//}

	//�f���[�e�B��12.5%
	void create_wave_data___square_wave_12_5(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��
			if (sinf(i * D3DX_PI / (wave_1cycle_length / 2)) >= 0.92387953251128675612818318939679f) {//0.92387953251128675612818318939679 = sin(67.5)
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//�f���[�e�B��25%
	void create_wave_data___square_wave_25(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��
			if (sinf(i * D3DX_PI / (wave_1cycle_length / 2)) >= 0.5f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//�f���[�e�B��50%
	void create_wave_data___square_wave_50(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��
			if (sinf(i * D3DX_PI / (wave_1cycle_length / 2)) >= 0.0f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	//�f���[�e�B��75%
	void create_wave_data___square_wave_75(float freq, uint8_t volume_4bit) {
		mtx.lock();

		short* p;
		p = (short*)current_create_wave_data;


		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��
			if (sinf(i * D3DX_PI / (wave_1cycle_length / 2)) <= -0.5f) {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MAX;
			}
			else {
				p[i] = ((double)volume_4bit / (double)0x0F) * SHRT_MIN;
			}
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	const double WAVE_4BIT_DATA_TABLE[0x0F] = {
		(double)SHRT_MIN,
		(((double)SHRT_MIN) / 7.0) * 6.0,
		(((double)SHRT_MIN) / 7.0) * 5.0,
		(((double)SHRT_MIN) / 7.0) * 4.0,
		(((double)SHRT_MIN) / 7.0) * 3.0,
		(((double)SHRT_MIN) / 7.0) * 2.0,
		(((double)SHRT_MIN) / 7.0),
		0.0,
		(((double)SHRT_MAX) / 7.0),
		(((double)SHRT_MAX) / 7.0) * 2.0,
		(((double)SHRT_MAX) / 7.0) * 3.0,
		(((double)SHRT_MAX) / 7.0) * 4.0,
		(((double)SHRT_MAX) / 7.0) * 5.0,
		(((double)SHRT_MAX) / 7.0) * 6.0,
		(double)SHRT_MAX,
	};
	//const short WAVE_4BIT_DATA_TABLE[0x0F] = {
	//	0,
	//	((SHRT_MAX) / 0x0E) * 1,
	//	((SHRT_MAX) / 0x0E) * 2,
	//	((SHRT_MAX) / 0x0E) * 3,
	//	((SHRT_MAX) / 0x0E) * 4,
	//	((SHRT_MAX) / 0x0E) * 5,
	//	((SHRT_MAX) / 0x0E) * 6,
	//	((SHRT_MAX) / 0x0E) * 7,
	//	((SHRT_MAX) / 0x0E) * 8,
	//	((SHRT_MAX) / 0x0E) * 9,
	//	((SHRT_MAX) / 0x0E) * 0xA,
	//	((SHRT_MAX) / 0x0E) * 0xB,
	//	((SHRT_MAX) / 0x0E) * 0xC,
	//	((SHRT_MAX) / 0x0E) * 0xD,
	//	SHRT_MAX,
	//};
	//�g�`������
	void create_wave_data___waveform_memory(float freq, uint8_t volume_4bit) {
		mtx.lock();
	
		short* p;
		p = (short*)current_create_wave_data;
	
		uint8_t wave_data_4bitx32_list[32];
		for (int i = 0; i < 16; i++) {
			wave_data_4bitx32_list[i * 2] = ((CH3__0xFF30_0xFF3F[i] >> 4) & 0b1111);
			wave_data_4bitx32_list[(i * 2) + 1] = (CH3__0xFF30_0xFF3F[i] & 0b1111);
		}

		for (size_t i = 0; i < (wave_data_size / 2); i++) {
			float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��

			float angle = (i * D3DX_PI / (wave_1cycle_length / 2));
			while (angle >= (D3DX_PI * 2)) {
				angle -= (D3DX_PI * 2);
			}

			uint32_t index;
			if (angle < ((D3DX_PI * 2) / 32.0f)) {
				index = 0;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 2.0f)) {
				index = 1;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 3.0f)) {
				index = 2;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 4.0f)) {
				index = 3;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 5.0f)) {
				index = 4;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 6.0f)) {
				index = 5;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 7.0f)) {
				index = 6;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 8.0f)) {
				index = 7;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 9.0f)) {
				index = 8;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 10.0f)) {
				index = 9;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 11.0f)) {
				index = 10;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 12.0f)) {
				index = 11;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 13.0f)) {
				index = 12;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 14.0f)) {
				index = 13;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 15.0f)) {
				index = 14;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 16.0f)) {
				index = 15;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 17.0f)) {
				index = 16;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 18.0f)) {
				index = 17;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 19.0f)) {
				index = 18;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 20.0f)) {
				index = 19;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 21.0f)) {
				index = 20;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 22.0f)) {
				index = 21;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 23.0f)) {
				index = 22;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 24.0f)) {
				index = 23;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 25.0f)) {
				index = 24;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 26.0f)) {
				index = 25;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 27.0f)) {
				index = 26;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 28.0f)) {
				index = 27;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 29.0f)) {
				index = 28;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 30.0f)) {
				index = 29;
			}
			else if (angle < (((D3DX_PI * 2) / 32.0f) * 31.0f)) {
				index = 30;
			}
			else {
				index = 31;
			}
			
			p[i] = ((double)volume_4bit / (double)0x0F) * (WAVE_4BIT_DATA_TABLE[wave_data_4bitx32_list[index]]);
		}

		current_data_ready_flag = true;

		mtx.unlock();
	}

	void create_wave_data___noise(float freq, uint8_t volume_4bit) {
		mtx.lock();

		bool short_freq_flag = ((CH4__0xFF22 & 0b1000) != 0) ? true : false;

		short* p;
		p = (short*)current_create_wave_data;

		float wave_1cycle_length = wave_format.nSamplesPerSec / freq;//�g��

		size_t i = 0;
		for (;;) {
			short noise_wave_value;
			if (short_freq_flag == false) {
				noise_wave_value = (short)(My_Random::get_instance_ptr()->get_random_uint32() & 0b1111111111111110);
			}
			else {
				uint16_t tmp_value_1 = (uint16_t)(My_Random::get_instance_ptr()->get_random_uint32() & 0b1111111000000000);
				noise_wave_value = (short)(tmp_value_1);
			}

			for (int j = 0; j < wave_1cycle_length; j++, i++) {
				if (!(i < (wave_data_size / 2))) {
					goto create_noise_exit;
				}

				p[i] = ((double)volume_4bit / (double)0x0F) * noise_wave_value;
			}
		}

	create_noise_exit:

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

	bool left_sound_ON_flag = true;//���̃T�E���h���L���Ȃ�true
	bool right_sound_ON_flag = true;//�E�̃T�E���h���L���Ȃ�true

	//====================================================

	double CH1_sweep_counter = 0.0;
	float CH1_sweep_freq_f;

	double CH1_length_counter = 0.0;

	double CH1_envelope_counter = 0.0;
	uint8_t CH1_envelope_volume = 0x0F;//0x0F���ő�0x00������

	uint8_t CH1__0xFF10 = 0;
	uint8_t CH1__0xFF11 = 0;
	uint8_t CH1__0xFF12 = 0;
	uint8_t CH1__0xFF13 = 0;
	uint8_t CH1__0xFF14 = 0;

	//====================================================

	double CH2_length_counter = 0.0;

	double CH2_envelope_counter = 0.0;
	uint8_t CH2_envelope_volume = 0x0F;//0x0F���ő�0x00������

	uint8_t CH2__0xFF16 = 0;
	uint8_t CH2__0xFF17 = 0;
	uint8_t CH2__0xFF18 = 0;
	uint8_t CH2__0xFF19 = 0;

	//====================================================

	double CH3_length_counter = 0.0;

	uint8_t CH3__0xFF1A = 0;
	uint8_t CH3__0xFF1B = 0;
	uint8_t CH3__0xFF1C = 0;
	uint8_t CH3__0xFF1D = 0;
	uint8_t CH3__0xFF1E = 0;

	uint8_t CH3__0xFF30_0xFF3F[16] = { 0 };

	//====================================================

	double CH4_length_counter = 0.0;

	double CH4_envelope_counter = 0.0;
	uint8_t CH4_envelope_volume = 0x0F;//0x0F���ő�0x00������

	uint8_t CH4__0xFF20 = 0;
	uint8_t CH4__0xFF21 = 0;
	uint8_t CH4__0xFF22 = 0;
	uint8_t CH4__0xFF23 = 0;

	//====================================================

	Channel(CH_TYPE ch_type, IXAudio2* xaudio) : ch_type(ch_type), xaudio(xaudio) {
		HRESULT hr;

		wave_format.wFormatTag = WAVE_FORMAT_PCM;
		wave_format.nChannels = 1;//�`�����l����
		wave_format.wBitsPerSample = 16;//�P�T���v��������̃r�b�g��
		wave_format.nSamplesPerSec = 44100;//�T���v�����O���g��
		wave_format.nBlockAlign = wave_format.wBitsPerSample / 8 * wave_format.nChannels;
		wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;

		callback = new VoiceCallBack();

		if (FAILED(hr = xaudio->CreateSourceVoice(&source_voice, &wave_format, 0, XAUDIO2_DEFAULT_FREQ_RATIO, callback))) {
			M_debug_printf("ERROR CreateSourceVoice\n");
		}

		//wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 60;
		//wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 20;//���������߂Ƀf�[�^����������(1/20���Ȃ߂炩)
		wave_data_size = (wave_format.nAvgBytesPerSec * 1) / 20;
		//1/60�b���̃f�[�^
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

	//const float NOISE_FREQ1_TABLE[8] = {
	//	1.0f,
	//	2.0f,
	//	4.0f,
	//	6.0f,
	//	8.0f,
	//	10.0f,
	//	12.0f,
	//	14.0f,
	//};

	void update(uint64_t c_cycle) {
		if (ch_type == CH_TYPE::CH1) {
			const uint8_t sweep_interval_3bit = ((CH1__0xFF10 >> 4) & 0b111);
			if (sweep_interval_3bit != 0) {//�X�C�[�v����ꍇ
				const uint8_t SWEEP_SHIFT_VALUE = (CH1__0xFF10 & 0b111);
				const double SWEEP_SPEED = SWEEP_INTERVAL_TIME_LIST[sweep_interval_3bit - 1];
				CH1_sweep_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				while (CH1_sweep_counter >= SWEEP_SPEED) {
					CH1_sweep_counter -= SWEEP_SPEED;

					if ((CH1__0xFF10 & 0b1000) == 0) {//���g�����傫���Ȃ�Ƃ�
						//CH1_sweep_freq_f = CH1_sweep_freq_f + (CH1_sweep_freq_f / powf(2, (float)SWEEP_SHIFT_VALUE));
						CH1_sweep_freq_f = CH1_sweep_freq_f + (CH1_sweep_freq_f / (float)(1 << SWEEP_SHIFT_VALUE));
					}
					else {//���g�����������Ȃ�Ƃ�
						//CH1_sweep_freq_f = CH1_sweep_freq_f - (CH1_sweep_freq_f / powf(2, (float)SWEEP_SHIFT_VALUE));
						CH1_sweep_freq_f = CH1_sweep_freq_f - (CH1_sweep_freq_f / (float)(1 << SWEEP_SHIFT_VALUE));
					}
				}

				freq_f = CH1_sweep_freq_f;
			}
			else {//�X�C�[�v���Ȃ��ꍇ
				uint32_t freq = (CH1__0xFF13 | ((CH1__0xFF14 & 0b00000111) << 8));
				freq_f = 131072.0f / (2048.0f - (float)freq);
			}

			//if (sound_enable_flag == true) {
			if ((CH1__0xFF12 & 0b00000111) != 0) {
				const uint8_t TMP_SPEED = (CH1__0xFF12 & 0b111);
				const double ENVELOPE_SPEED = (double)TMP_SPEED * (1.0 / 64.0);
				CH1_envelope_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				while (CH1_envelope_counter >= ENVELOPE_SPEED) {
					CH1_envelope_counter -= ENVELOPE_SPEED;

					if ((CH1__0xFF12 & 0b00001000) != 0) {//�傫���Ȃ��Ă���
						if (CH1_envelope_volume < 0x0F) {
							CH1_envelope_volume++;
						}
					}
					else {//�������Ȃ��Ă���
						if (CH1_envelope_volume > 0) {
							CH1_envelope_volume--;
						}
					}
				}

			}


			if ((CH1__0xFF14 & 0b01000000) != 0) {//�����J�E���^�L���t���O���L���ȂƂ�
				const uint8_t TMP_LENGTH = (CH1__0xFF11 & 0b00111111);
				const double SOUND_LENGTH = (64.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
				CH1_length_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				if (CH1_length_counter >= SOUND_LENGTH) {
					sound_enable_flag = false;
				}
			}
			//}

		}
		else if (ch_type == CH_TYPE::CH2) {
			uint32_t freq = (CH2__0xFF18 | ((CH2__0xFF19 & 0b00000111) << 8));
			freq_f = 131072.0f / (2048.0f - (float)freq);

			//if (sound_enable_flag == true) {
			if ((CH2__0xFF17 & 0b00000111) != 0) {
				const uint8_t TMP_SPEED = (CH2__0xFF17 & 0b111);
				const double ENVELOPE_SPEED = (double)TMP_SPEED * (1.0 / 64.0);
				CH2_envelope_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				while (CH2_envelope_counter >= ENVELOPE_SPEED) {
					CH2_envelope_counter -= ENVELOPE_SPEED;

					if ((CH2__0xFF17 & 0b00001000) != 0) {//�傫���Ȃ��Ă���
						if (CH2_envelope_volume < 0x0F) {
							CH2_envelope_volume++;
						}
					}
					else {//�������Ȃ��Ă���
						if (CH2_envelope_volume > 0) {
							CH2_envelope_volume--;
						}
					}
				}

			}


			if ((CH2__0xFF19 & 0b01000000) != 0) {//�����J�E���^�L���t���O���L���ȂƂ�
				const uint8_t TMP_LENGTH = (CH2__0xFF16 & 0b00111111);
				const double SOUND_LENGTH = (64.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
				CH2_length_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				if (CH2_length_counter >= SOUND_LENGTH) {
					sound_enable_flag = false;
				}
			}
			//}

		}
		else if (ch_type == CH_TYPE::CH3) {
			uint32_t freq = (CH3__0xFF1D | ((CH3__0xFF1E & 0b00000111) << 8));
			freq_f = 65536.0f / (2048.0f - (float)freq);

			if ((CH3__0xFF1E & 0b01000000) != 0) {//�����J�E���^�L���t���O���L���ȂƂ�
				const uint8_t TMP_LENGTH = CH3__0xFF1B;
				const double SOUND_LENGTH = (256.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
				CH3_length_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				if (CH3_length_counter >= SOUND_LENGTH) {
					sound_enable_flag = false;
				}
			}
		}
		else {//CH4
			uint8_t noise_freq_2 = ((CH4__0xFF22 >> 4) & 0b1111);
			if (noise_freq_2 == 0b1111 || noise_freq_2 == 0b1110) {
				sound_enable_flag = false;
			}
			float noise_freq_1 = (float)(CH4__0xFF22 & 0b111);// NOISE_FREQ1_TABLE[(CH4__0xFF22 & 0b111)];
			if (noise_freq_1 == 0.0) {
				noise_freq_1 = 0.5;
			}
			freq_f = 524288.0f / noise_freq_1 / (float)(1 << (noise_freq_2 + 1));
			
			//if (sound_enable_flag == true) {
			if ((CH4__0xFF21 & 0b00000111) != 0) {
				const uint8_t TMP_SPEED = (CH4__0xFF21 & 0b111);
				const double ENVELOPE_SPEED = (double)TMP_SPEED * (1.0 / 64.0);
				CH4_envelope_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				while (CH4_envelope_counter >= ENVELOPE_SPEED) {
					CH4_envelope_counter -= ENVELOPE_SPEED;

					if ((CH4__0xFF21 & 0b00001000) != 0) {//�傫���Ȃ��Ă���
						if (CH4_envelope_volume < 0x0F) {
							CH4_envelope_volume++;
						}
					}
					else {//�������Ȃ��Ă���
						if (CH4_envelope_volume > 0) {
							CH4_envelope_volume--;
						}
					}
				}

			}


			if ((CH4__0xFF23 & 0b01000000) != 0) {//�����J�E���^�L���t���O���L���ȂƂ�
				const uint8_t TMP_LENGTH = (CH4__0xFF20 & 0b00111111);
				const double SOUND_LENGTH = (64.0 - (double)TMP_LENGTH) * (1.0 / 256.0);
				CH4_length_counter += (1.0 / CPU_FREQ_D) * c_cycle;
				if (CH4_length_counter >= SOUND_LENGTH) {
					sound_enable_flag = false;
				}
			}
			//}

		}
	}

	void execute() {
		//M_debug_printf("Channel::execute()\n");

		//create_wave_data_1(440.0f);

		if (ch_type == CH_TYPE::CH1) {
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
				create_wave_data___none();//����
			}

			//create_wave_data___none();//����
		}
		else if (ch_type == CH_TYPE::CH2) {
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
				create_wave_data___none();//����
			}

			//create_wave_data___none();//����
		}
		else if (ch_type == CH_TYPE::CH3) {
			if (sound_enable_flag == true) {
				const uint8_t volume_2bit = ((CH3__0xFF1C >> 5) & 0b11);
				uint8_t volume_4bit;
				if (volume_2bit == 0b00) {
					volume_4bit = 0b0000;
				}
				else if (volume_2bit == 0b01) {
					volume_4bit = 0b1111;
				}
				else if (volume_2bit == 0b00) {
					volume_4bit = 0b0111;
				}
				else {
					volume_4bit = 0b0011;
				}
			
				create_wave_data___waveform_memory(freq_f, volume_4bit);
			}
			else {
				create_wave_data___none();//����
			}

			//create_wave_data___none();//����
		}
		else {//CH4
			if (sound_enable_flag == true) {
				create_wave_data___noise(freq_f, (CH4_envelope_volume & 0b1111));
			}
			else {
				create_wave_data___none();//����
			}

			//create_wave_data___none();//����
		}
	}

	void execute_sleep() {
		create_wave_data___none();//����
	}

	bool is_playing() {
		//return false;
		//return isPlaying;
		return sound_enable_flag;
	}

	void set_sound_enable_flag(bool flag) {
		sound_enable_flag = flag;
	}

	void init_CH1_sweep_freq_f() {
		uint32_t freq = (CH1__0xFF13 | ((CH1__0xFF14 & 0b00000111) << 8));
		freq_f = 131072.0f / (2048.0f - (float)freq);

		CH1_sweep_freq_f = freq_f;
	}

	IXAudio2SourceVoice* get_source_voice_ptr() {
		return source_voice;
	}

};

