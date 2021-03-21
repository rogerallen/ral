# ral - Roger Allen's Lisp

I Made a Lisp based on https://github.com/kanaka/mal

There are many others like it, but this one is mine.

## TODO
```
X double numbers & math functions
X PI math constant & TAU, E
O clean up error handling
O meta for documentation
O how to do self-doc?  get list of fns from env
O ?read hex numbers?
O ?float numbers & math functions?
O ?rational numbers?
O ?large integers?
```
## FIXME

- ral.cpp
  - make repl_env global into self-contained class. perhaps a thread? 
  - reconsider apply() to not use list, rather begin/end ptrs?
  - reconsider ral_eval() loc
- types.cpp
  - throw error on apply not to list
  - RalFunction::eval()
  - RalLambda::eval(), equal()
  - RalAtom::eval(), equal()

## Notes

Works on Windows via Visual Studio 2019, WSL2 Ubuntu 20.04 and normal Ubuntu 20.04 via g++.

Tests don't run on Windows because they require python and I don't feel like getting that working.

Add from 1 to 3 "-v" args to the commandline to get INFO, DEBUG & even moar DEBUG info.

Add a file to the commandline to run it.

### Examples
``` 
(def! a-number 7334)
(def! another-number 73.34)
(def! a-string "hello")
(def! radians (fn* (deg) (* TAU (/ deg 360.))))
(defn! circumference [radius] (* 2 PI radius)))
```
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

### Uses easylogging++

https://github.com/amrayn/easyloggingpp

The MIT License (MIT)

Copyright (c) 2012-2018 Amrayn Web Services
Copyright (c) 2012-2018 @abumusamq

https://github.com/amrayn/
https://amrayn.com
https://muflihun.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

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