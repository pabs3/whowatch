/*

SPDX-License-Identifier: GPL-2.0+

Copyright 1999, 2001 Michal Suszycki <mt_suszycki@yahoo.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

/*
 * Functions needed for printing process owner in the tree.
 */
#include <pwd.h>
#include "whowatch.h"

#define HASHSIZE 32
#define NAME_SIZE	8

#define list_add(l,p) ({			\
	(p)->next = (l);			\
	(l) = (p);				\
})

struct owner{
	char name[NAME_SIZE + 1];
	uid_t uid;
	struct owner *next;
};
static struct owner *hash_table[HASHSIZE];

static inline int hash_fun(int n)
{
	return n&(HASHSIZE-1);
}

static struct owner* find_by_uid(uid_t n)
{
	struct owner* p;

	p = hash_table[hash_fun(n)];
	while(p) {
		if(p->uid == n) break;
		p=p->next;
	}
	return p;
}

static struct owner* new_owner(uid_t n)
{
	struct owner* p;
	struct passwd *u;
	p = (struct owner*) malloc(sizeof *p);
	if (!p) allocate_error();
	memset(p, 0, sizeof *p);
	u = getpwuid(n);
	if (!u) sprintf(p->name, "%d", n);
	else {
		strncpy(p->name, u->pw_name, NAME_SIZE);
		p->name[NAME_SIZE] = 0;
	}
	p->uid = n;
	list_add(hash_table[hash_fun(n)], p);
	return p;
}

char *get_owner_name(uid_t u)
{
	struct owner *p;
	p = find_by_uid(u);
	if (!p) p = new_owner(u);
	return p->name;
}

