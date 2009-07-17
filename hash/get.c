/*-
 * Copyright (c) 2009 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Caoimhe Chaos <caoimhechaos@protonmail.com>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "clib_internal.h"

/**
 * Retrieve the first record with the specified key. Otherwise, return NULL.
 */
void *
c_hashtable_lookup(struct c_hashtable *h, const void *key)
{
	uint32_t hvalue, hvalue_lookup;
	struct c_hashtable_value *val;

	if (!h)
		return NULL;

	hvalue = h->h_hash(key);
	hvalue_lookup = hvalue % h->h_size;

	SLIST_FOREACH(val, &h->m_values[hvalue_lookup], v_vlist)
	{
		if (!h->h_equals(val->key, key))
			continue;

		return val->value;
	}

	/* Nothing found, too bad. */
	return NULL;
}

/**
 * Iterate over all hash table entries matching the specified key and call
 * the callback with the given data.
 */
int
c_hashtable_lookup_cb(struct c_hashtable *h, const void *key, c_htcallback cb,
	const void *userdata)
{
	uint32_t hvalue, hvalue_lookup;
	struct c_hashtable_value *val;

	if (!h)
		return 0;

	hvalue = h->h_hash(key);
	hvalue_lookup = hvalue % h->h_size;

	SLIST_FOREACH(val, &h->m_values[hvalue_lookup], v_vlist)
	{
		if (!h->h_equals(val->key, key))
			continue;

		if (cb(val->value, userdata))
			return 1;
	}

	return 1;
}

/**
 * Call the given callback for each member in the entire hash. Note that
 * of course there is no ordering at all.
 */
int
c_hashtable_foreach(struct c_hashtable *h, c_htcallback cb,
	const void *userdata)
{
	uint32_t i;
	struct c_hashtable_value *val;

	if (!h)
		return 0;

	for (i = 0; i < h->h_size; i++)
	{
		SLIST_FOREACH(val, &h->m_values[i], v_vlist)
		{
			if (cb(val->value, userdata))
				return 1;
		}
	}

	return 1;
}
