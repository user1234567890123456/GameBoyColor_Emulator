#pragma once

#include <xaudio2.h>
#include <mmsystem.h>

#include "DebugUtility.h"
#include "Channel.h"

class APU
{
private:
	IXAudio2* xaudio = nullptr;
	IXAudio2MasteringVoice* mastering_voice = nullptr;

	Channel* ch1;
	Channel* ch2;
	Channel* ch3;
	Channel* ch4;

public:
	bool all_channel_enable_flag = false;

	APU() {
		HRESULT hr;
		if (FAILED(hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED))) {
			M_debug_printf("ERROR CoInitializeEx\n");
		}
		if (FAILED(hr = XAudio2Create(&xaudio))) {
			M_debug_printf("ERROR XAudio2Create\n");
		}

		if (FAILED(hr = xaudio->CreateMasteringVoice(&mastering_voice))) {
			M_debug_printf("ERROR CreateMasteringVoice\n");
		}

		mastering_voice->SetVolume(0.04);

		ch1 = new Channel(Channel::CH_TYPE::CH1, xaudio);
		ch2 = new Channel(Channel::CH_TYPE::CH2, xaudio);
		ch3 = new Channel(Channel::CH_TYPE::CH3, xaudio);
		ch4 = new Channel(Channel::CH_TYPE::CH4, xaudio);
	}

	~APU() {
		delete ch1;
		delete ch2;
		delete ch3;
		delete ch4;

		if (mastering_voice != nullptr) {
			mastering_voice->DestroyVoice();
		}
		if (xaudio != nullptr) {
			xaudio->Release();
		}
		CoUninitialize();
	}

	Channel* get_channel_1() {
		return ch1;
	}

	Channel* get_channel_2() {
		return ch2;
	}

	Channel* get_channel_3() {
		return ch3;
	}

	Channel* get_channel_4() {
		return ch4;
	}

	void update_all_channel(uint64_t c_cycle) {
		ch1->update(c_cycle);
		ch2->update(c_cycle);
		ch3->update(c_cycle);
		ch4->update(c_cycle);
	}

	void execute_all_channel() {
		if (all_channel_enable_flag == true) {
			ch1->execute();
			ch2->execute();
			ch3->execute();
			ch4->execute();
		}
		else {
			ch1->execute_sleep();
			ch2->execute_sleep();
			ch3->execute_sleep();
			ch4->execute_sleep();
		}
	}
};
