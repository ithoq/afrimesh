/*
    pmacct (Promiscuous mode IP Accounting package)
    pmacct is Copyright (C) 2003-2008 by Paolo Lucente
*/

/*
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

/* defines */
#define __PRETAG_C

/* includes */
#include "pmacct.h"
#include "nfacctd.h"
#include "pretag_handlers.h"
#include "pretag-data.h"

int pre_tag_map_allocated = FALSE;

void load_id_file(int acct_type, char *filename, struct id_table *t, struct plugin_requests *req)
{
  struct id_table tmp;
  struct id_entry *ptr, *ptr2;
  FILE *file;
  char buf[SRVBUFLEN];
  int v4_num = 0, x, tot_lines = 0, err, index, label_solved, sz;
  struct stat st;

#if defined ENABLE_IPV6
  int v6_num = 0;
#endif

  /* parsing engine vars */
  char *start, *key = NULL, *value = NULL;
  int len;

  Log(LOG_INFO, "INFO ( default/core ): Trying to reload pre tag map\n") ;

  memset(&st, 0, sizeof(st));

  if (!config.pre_tag_map_entries) config.pre_tag_map_entries = MAX_PRETAG_MAP_ENTRIES;

  if (filename) {
    if ((file = fopen(filename, "r")) == NULL) {
      Log(LOG_ERR, "ERROR: Pre-Tagging map '%s' not found.\n", filename);
      goto handle_error;
    }

    sz = sizeof(struct id_entry)*config.pre_tag_map_entries;

    if (!pre_tag_map_allocated) {
      memset(t, 0, sizeof(struct id_table));
      t->e = (struct id_entry *) malloc(sz);
      pre_tag_map_allocated = TRUE;
    }
    else {
      ptr = t->e ;
      memset(t, 0, sizeof(struct id_table));
      t->e = ptr ;
    }

    memset(&tmp, 0, sizeof(struct id_table));
    tmp.e = (struct id_entry *) malloc(sz);


    memset(tmp.e, 0, sz);
    memset(t->e, 0, sz);

    /* first stage: reading Agent ID file and arranging it in a temporary memory table */
    while (!feof(file)) {
      tot_lines++;
      if (tmp.num >= config.pre_tag_map_entries) break; /* XXX: we shouldn't exit silently */
      memset(buf, 0, SRVBUFLEN);
      if (fgets(buf, SRVBUFLEN, file)) {
        if (!iscomment(buf) && !isblankline(buf)) {
          if (!check_not_valid_char(filename, buf, '|')) {
            mark_columns(buf);
            trim_all_spaces(buf);
	    strip_quotes(buf);

	    /* resetting the entry and enforcing defaults */
            memset(&tmp.e[tmp.num], 0, sizeof(struct id_entry));
	    tmp.e[tmp.num].ret = TRUE;

            err = FALSE; key = NULL; value = NULL;
            start = buf;
            len = strlen(buf);

            for (x = 0; x <= len; x++) {
              if (buf[x] == '=') {
                if (start == &buf[x]) continue;
                if (!key) {
                  buf[x] = '\0';
		  key = start;
                  x++;
                  start = &buf[x];
		}
              }
              if ((buf[x] == '|') || (buf[x] == '\0')) {
                if (start == &buf[x]) continue;
                buf[x] = '\0';
                if (value || !key) {
                  Log(LOG_ERR, "ERROR ( %s ): malformed line %d. Ignored.\n", filename, tot_lines);
                  err = TRUE;
                  break;
                }
                else value = start;
                x++;
                start = &buf[x];
              }

              if (key && value) {
                int dindex; /* dictionary index */

                for (dindex = 0; strcmp(map_dictionary[dindex].key, ""); dindex++) {
                  if (!strcmp(map_dictionary[dindex].key, key)) {
                    err = (*map_dictionary[dindex].func)(filename, &tmp.e[tmp.num], value, req);
                    break;
                  }
                  else err = E_NOTFOUND; /* key not found */
                }
                if (err) {
                  if (err == E_NOTFOUND) Log(LOG_ERR, "ERROR ( %s ): unknown key '%s' at line %d. Ignored.\n", filename, key, tot_lines);
                  else Log(LOG_ERR, "Line %d ignored.\n", tot_lines);
                  break; 
                }
                key = NULL; value = NULL;
              }
            }
            /* verifying errors and required fields */
	    if (acct_type == ACCT_NF || acct_type == ACCT_SF) {
              if (!err && tmp.e[tmp.num].id && tmp.e[tmp.num].agent_ip.a.family) {
                int j;

                for (j = 0; tmp.e[tmp.num].func[j]; j++);
                tmp.e[tmp.num].func[j] = pretag_id_handler;
	        if (tmp.e[tmp.num].agent_ip.a.family == AF_INET) v4_num++;
#if defined ENABLE_IPV6
	        else if (tmp.e[tmp.num].agent_ip.a.family == AF_INET6) v6_num++;
#endif
                tmp.num++;
              }
	      /* if any required field is missing and other errors have been signalled
	         before we will trap an error message */
	      else if ((!tmp.e[tmp.num].id || !tmp.e[tmp.num].agent_ip.a.family) && !err)
	        Log(LOG_ERR, "ERROR ( %s ): required key missing at line: %d. Required keys are: 'id', 'ip'.\n", filename, tot_lines); 
	    }
	    else if (acct_type == ACCT_PM) {
	      if (tmp.e[tmp.num].agent_ip.a.family)
		Log(LOG_ERR, "ERROR ( %s ): key 'ip' not applicable here. Invalid line: %d.\n", filename, tot_lines);
	      else if (!err && tmp.e[tmp.num].id) {
                int j;

		for (j = 0; tmp.e[tmp.num].func[j]; j++);
		tmp.e[tmp.num].agent_ip.a.family = AF_INET; /* we emulate a dummy '0.0.0.0' IPv4 address */
		tmp.e[tmp.num].func[j] = pretag_id_handler;
		v4_num++; tmp.num++;
	      } 
	    }
          }
          else Log(LOG_ERR, "ERROR ( %s ): malformed line: %d. Ignored.\n", filename, tot_lines);
        }
      }
    }
    fclose(file);

    stat(filename, &st);
    t->timestamp = st.st_mtime;

    /* second stage: segregating IPv4, IPv6. No further reordering
       in order to deal smoothly with jumps (ie. JEQs) */

    x = 0;
    t->num = tmp.num;
    t->ipv4_num = v4_num; 
    t->ipv4_base = &t->e[x];
    for (index = 0; index < tmp.num; index++) {
      if (tmp.e[index].agent_ip.a.family == AF_INET) { 
        memcpy(&t->e[x], &tmp.e[index], sizeof(struct id_entry));
	t->e[x].pos = x;
	x++;
      }
    }
#if defined ENABLE_IPV6
    t->ipv6_num = v6_num;
    t->ipv6_base = &t->e[x];
    for (index = 0; index < tmp.num; index++) {
      if (tmp.e[index].agent_ip.a.family == AF_INET6) {
        memcpy(&t->e[x], &tmp.e[index], sizeof(struct id_entry));
	t->e[x].pos = x;
        x++;
      }
    }
#endif

    /* third stage: building short circuits basing on jumps and labels. Only
       forward references are solved. Backward and unsolved references will
       generate errors. */
    for (ptr = t->ipv4_base, x = 0; x < t->ipv4_num; ptr++, x++) {
      if (ptr->jeq.label) {
	label_solved = FALSE;

	/* honouring reserved labels (ie. "next"). Then resolving unknown labels */
	if (!strcmp(ptr->jeq.label, "next")) {
	  ptr->jeq.ptr = ptr+1;
	  label_solved = TRUE;
	}
	else {
	  for (ptr2 = ptr+1, index = x+1; index < t->ipv4_num; ptr2++, index++) {
	    if (!strcmp(ptr->jeq.label, ptr2->label)) {
	      ptr->jeq.ptr = ptr2;
	      label_solved = TRUE;
	    }
	  }
	}
	if (!label_solved) {
	  ptr->jeq.ptr = NULL;
	  Log(LOG_ERR, "ERROR ( %s ): Unresolved label '%s'. Ignoring it.\n", filename, ptr->jeq.label);
	}
	free(ptr->jeq.label);
	ptr->jeq.label = NULL;
      }
    }

#if defined ENABLE_IPV6
    for (ptr = t->ipv6_base, x = 0; x < t->ipv6_num; ptr++, x++) {
      if (ptr->jeq.label) {
        label_solved = FALSE;
        for (ptr2 = ptr+1, index = x+1; index < t->ipv6_num; ptr2++, index++) {
          if (!strcmp(ptr->jeq.label, ptr2->label)) {
            ptr->jeq.ptr = ptr2;
            label_solved = TRUE;
          }
        }
        if (!label_solved) {
          ptr->jeq.ptr = NULL;
          Log(LOG_ERR, "ERROR ( %s ): Unresolved label '%s'. Ignoring it.\n", filename, ptr->jeq.label);
        }
        free(ptr->jeq.label);
        ptr->jeq.label = NULL;
      }
    }
#endif
  }

  free(tmp.e) ;
  Log(LOG_INFO, "INFO ( default/core ): Pre tag map successfully reloaded.\n") ;

  return;

  handle_error:
  free(tmp.e) ;
  if (t->timestamp) {
    Log(LOG_WARNING, "WARN: Rolling back the old Pre-Tagging Map.\n");

    /* we update the timestamp to avoid loops */
    stat(filename, &st);
    t->timestamp = st.st_mtime;
  }
  else exit_all(1);
}

u_int8_t pt_check_neg(char **value)
{
  if (**value == '-') {
    (*value)++;
    return TRUE;
  }
  else return FALSE;
}

char *pt_check_range(char *str)
{
  char *ptr;

  if (ptr = strchr(str, '-')) {
    *ptr = '\0';
    ptr++;
    return ptr;
  }
  else return NULL;
}