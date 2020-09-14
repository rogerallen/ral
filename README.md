# ral - Roger Allen's Lisp

I Made a Lisp based on https://github.com/kanaka/mal

There are many others like it, but this one is mine.

## TODO

X double numbers & math functions
X PI math constant & TAU, E
O meta for documentation
O how to do self-doc?  get list of fns from env
O ?read hex numbers?
O ?float numbers & math functions?
O ?rational numbers?
O ?large integers?

## FIXME

- core.cpp
- ral.cpp
- reader.cpp
- types.cpp

## Notes

### Windows
- cmd.exe shell does not prompt and does not do well with Ctrl-D
- Power Shell seems to work just fine

### Examples

(def! radians (fn* (deg) (* TAU (/ deg 360.))))
(def! circumference (fn* [radius] (* 2 PI radius)))

## License

Copyright (C) 2020 Roger Allen

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

### Uses aixlog.hpp

https://github.com/badaix/aixlog
Copyright (C) 2017-2020 Johannes Pohl
This software may be modified and distributed under the terms
of the MIT license.  See the LICENSE file for details.

### Uses linenoise.hpp

https://github.com/yhirose/cpp-linenoise
All credits and commendations have to go to the authors of the
following excellent libraries.

 - linenoise.h and linenose.c (https://github.com/antirez/linenoise)
 - ANSI.c (https://github.com/adoxa/ansicon)
 - Win32_ANSI.h and Win32_ANSI.c (https://github.com/MSOpenTech/redis)

Copyright (c) 2015 yhirose
All rights reserved.
  
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  
1. Redistributions of source code must retain the above copyright notice, this 
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
  
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.