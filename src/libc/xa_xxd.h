/* SPDX-FileCopyrightText: 2008 Ed Rose, Weston Schmidt */
/* SPDX-License-Identifier: Apache-2.0 */
/*
 *  xxd.c - formatted (like xxd) buffer dump
 *
 *  Written by Ed Rose, Weston Schmidt (2008)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -- or --
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  In other words, you are welcome to use, share and improve this program.
 *  You are forbidden to forbid anyone else to use, share and improve
 *  what you give them.   Help stamp out software-hoarding!
 */
#ifndef _XA_XXD_H_
#define _XA_XXD_H_

#include <stddef.h>
#include <stdio.h>

/**
 *  Used to dump a buffer of a specified length to STDOUT.
 *
 *  @param buffer the buffer to dump
 *  @param length the number of bytes to dump
 */
void xxd(const void *buffer, size_t length, FILE *stream);

#endif
