/*
 * Copyright (C) 2014 Jared Boone, ShareBrained Technology, Inc.
 * Copyright (C) 2016 Furrtek
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#include "audio.hpp"

#include "portapack.hpp"
using portapack::clock_manager;

#include "portapack_hal.hpp"

#include "i2s.hpp"
using namespace lpc43xx;

namespace audio {

namespace {

constexpr i2s::ConfigTX i2s0_config_tx {
	.dao = i2s::DAO {
		.wordwidth = i2s::WordWidth::Bits16,
		.mono = 0,
		.stop = 1,
		.reset = 0,
		.ws_sel = 0,			// Master
		.ws_halfperiod = 0x0f,
		.mute = 1,
	},
	.txrate = i2s::MCLKRate {
		.x_divider = 0,
		.y_divider = 0,
	},
	.txbitrate = i2s::BitRate {
		.bitrate = 7,
	},
	.txmode = i2s::Mode {
		.clksel = i2s::ClockSelect::BaseAudioClkOrExternalMCLK,
		.four_pin = 0,
		.mclk_out_en = 1,
	},
};

constexpr i2s::ConfigRX i2s0_config_rx {
	.dai = i2s::DAI {
		.wordwidth = i2s::WordWidth::Bits16,
		.mono = 0,
		.stop = 1,
		.reset = 0,
		.ws_sel = 0,			// Master
		.ws_halfperiod = 0x0f,
	},
	.rxrate = i2s::MCLKRate {
		.x_divider = 0,
		.y_divider = 0,
	},
	.rxbitrate = i2s::BitRate {
		.bitrate = 7,
	},
	.rxmode = i2s::Mode {
		.clksel = i2s::ClockSelect::BaseAudioClkOrExternalMCLK,
		.four_pin = 1,
		.mclk_out_en = 0,
	},
};

constexpr i2s::ConfigDMA i2s0_config_dma {
	.dma1 = i2s::DMA {
		.rx_enable = 1,
		.tx_enable = 0,
		.rx_depth = 4,
		.tx_depth = 0,
	},
	.dma2 = i2s::DMA {
		.rx_enable = 0,
		.tx_enable = 1,
		.rx_depth = 0,
		.tx_depth = 4,
	},
};

static audio::Codec* audio_codec = nullptr;

} /* namespace */

namespace output {

void start() {
	i2s::i2s0::tx_start();
	unmute();
}

void stop() {
	mute();
	i2s::i2s0::tx_stop();
}

void mute() {
	i2s::i2s0::tx_mute();
	audio_codec->headphone_disable();
}

void unmute() {
	i2s::i2s0::tx_unmute();
	audio_codec->headphone_enable();
}

} /* namespace output */

namespace input {

void start() {
	audio_codec->microphone_enable();
	i2s::i2s0::rx_start();
}

void stop() {
	i2s::i2s0::rx_stop();
	audio_codec->microphone_disable();
}

} /* namespace input */

namespace headphone {

volume_range_t volume_range() {
	return audio_codec->headphone_gain_range();
}

void set_volume(const volume_t volume) {
	audio_codec->set_headphone_volume(volume);
}

} /* namespace headphone */

namespace debug {

size_t reg_count() {
	return audio_codec->reg_count();
}

uint32_t reg_read(const size_t register_number) {
	return audio_codec->reg_read(register_number);
}

std::string codec_name() {
	return audio_codec->name();
}

size_t reg_bits() {
	return audio_codec->reg_bits();
}

} /* namespace debug */

void init(audio::Codec* const codec) {
	audio_codec = codec;

	clock_manager.start_audio_pll();
	audio_codec->init();

	i2s::i2s0::configure(
		i2s0_config_tx,
		i2s0_config_rx,
		i2s0_config_dma
	);

	// Set pin mode, since it's likely GPIO (as left after CPLD JTAG interactions).
	portapack::pin_i2s0_rx_sda.mode(3);
}

void shutdown() {
	audio_codec->reset();
	output::stop();
}

void set_rate(const Rate rate) {
	clock_manager.set_base_audio_clock_divider(toUType(rate));
}

} /* namespace audio */
