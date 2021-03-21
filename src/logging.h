// ======================================================================
// ral - Roger Allen's Lisp via https://github.com/kanaka/mal
// Copyright(C) 2020 Roger Allen
//
// logging.h - logging helpers
//
// ======================================================================
// This program is free software : you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// ======================================================================
#pragma once
// helpers for different levels of logging
// info for basics
#define INFO LOG_IF(gInfo, INFO)
// debug for way more info
#define DBG LOG_IF(gDebug, DEBUG)
// detailed debug for memory alloc/dealloc, etc.
#define DBG2 LOG_IF(gDebug2, DEBUG)