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

#include "ui.hpp"
#include <cstring>
#include <string>

#ifndef __MODEMS_H__
#define __MODEMS_H__

namespace modems {
	
#define MODEM_DEF_COUNT 7

enum modulation_enum {
	AFSK = 0,
	FSK,
	PSK,
	SSB
};

struct modem_def_t {
	std::string name;
	modulation_enum modulation;
	uint16_t mark_freq;
	uint16_t space_freq;
	uint16_t baudrate;
};

const modem_def_t modem_defs[MODEM_DEF_COUNT] = {
	{ "Bell202", 	AFSK,	1200,	2200, 	1200 },
	{ "Bell103", 	AFSK,	1270,	1070, 	300 },
	{ "V21",		AFSK,	980,	1180, 	300 },
	{ "V23 M1",		AFSK,	1300,	1700,	600 },
	{ "V23 M2",		AFSK,	1300,	2100,	1200 },
	{ "RTTY US",	SSB,	2295,	2125,	45 },
	{ "RTTY EU",	SSB,	2125,	1955,	45 }
};

void generate_data(const std::string& in_message, uint16_t * out_data);

} /* namespace modems */

#endif/*__MODEMS_H__*/
