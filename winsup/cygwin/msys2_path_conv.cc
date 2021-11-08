/*
  The MSYS2 Path conversion source code is licensed under:
  
  CC0 1.0 Universal
  
  Official translations of this legal tool are available
  
  CREATIVE COMMONS CORPORATION IS NOT A LAW FIRM AND DOES NOT PROVIDE
  LEGAL SERVICES. DISTRIBUTION OF THIS DOCUMENT DOES NOT CREATE AN
  ATTORNEY-CLIENT RELATIONSHIP. CREATIVE COMMONS PROVIDES THIS
  INFORMATION ON AN "AS-IS" BASIS. CREATIVE COMMONS MAKES NO WARRANTIES
  REGARDING THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS
  PROVIDED HEREUNDER, AND DISCLAIMS LIABILITY FOR DAMAGES RESULTING FROM
  THE USE OF THIS DOCUMENT OR THE INFORMATION OR WORKS PROVIDED
  HEREUNDER.
  
  Statement of Purpose
  
  The laws of most jurisdictions throughout the world automatically
  confer exclusive Copyright and Related Rights (defined below) upon the
  creator and subsequent owner(s) (each and all, an "owner") of an
  original work of authorship and/or a database (each, a "Work").
  
  Certain owners wish to permanently relinquish those rights to a Work
  for the purpose of contributing to a commons of creative, cultural and
  scientific works ("Commons") that the public can reliably and without
  fear of later claims of infringement build upon, modify, incorporate
  in other works, reuse and redistribute as freely as possible in any
  form whatsoever and for any purposes, including without limitation
  commercial purposes. These owners may contribute to the Commons to
  promote the ideal of a free culture and the further production of
  creative, cultural and scientific works, or to gain reputation or
  greater distribution for their Work in part through the use and
  efforts of others.
  
  For these and/or other purposes and motivations, and without any
  expectation of additional consideration or compensation, the person
  associating CC0 with a Work (the "Affirmer"), to the extent that he or
  she is an owner of Copyright and Related Rights in the Work,
  voluntarily elects to apply CC0 to the Work and publicly distribute
  the Work under its terms, with knowledge of his or her Copyright and
  Related Rights in the Work and the meaning and intended legal effect
  of CC0 on those rights.
  
  1. Copyright and Related Rights. A Work made available under CC0 may
  be protected by copyright and related or neighboring rights
  ("Copyright and Related Rights"). Copyright and Related Rights
  include, but are not limited to, the following:
  
  the right to reproduce, adapt, distribute, perform, display,
  communicate, and translate a Work;
  moral rights retained by the original author(s) and/or performer(s);
  publicity and privacy rights pertaining to a person's image or
  likeness depicted in a Work;
  rights protecting against unfair competition in regards to a Work,
  subject to the limitations in paragraph 4(a), below;
  rights protecting the extraction, dissemination, use and reuse of data
  in a Work;
  database rights (such as those arising under Directive 96/9/EC of the
  European Parliament and of the Council of 11 March 1996 on the legal
  protection of databases, and under any national implementation
  thereof, including any amended or successor version of such
  directive); and
  other similar, equivalent or corresponding rights throughout the world
  based on applicable law or treaty, and any national implementations
  thereof.
  
  2. Waiver. To the greatest extent permitted by, but not in
  contravention of, applicable law, Affirmer hereby overtly, fully,
  permanently, irrevocably and unconditionally waives, abandons, and
  surrenders all of Affirmer's Copyright and Related Rights and
  associated claims and causes of action, whether now known or unknown
  (including existing as well as future claims and causes of action), in
  the Work (i) in all territories worldwide, (ii) for the maximum
  duration provided by applicable law or treaty (including future time
  extensions), (iii) in any current or future medium and for any number
  of copies, and (iv) for any purpose whatsoever, including without
  limitation commercial, advertising or promotional purposes (the
  "Waiver"). Affirmer makes the Waiver for the benefit of each member of
  the public at large and to the detriment of Affirmer's heirs and
  successors, fully intending that such Waiver shall not be subject to
  revocation, rescission, cancellation, termination, or any other legal
  or equitable action to disrupt the quiet enjoyment of the Work by the
  public as contemplated by Affirmer's express Statement of Purpose.
  
  3. Public License Fallback. Should any part of the Waiver for any
  reason be judged legally invalid or ineffective under applicable law,
  then the Waiver shall be preserved to the maximum extent permitted
  taking into account Affirmer's express Statement of Purpose. In
  addition, to the extent the Waiver is so judged Affirmer hereby grants
  to each affected person a royalty-free, non transferable, non
  sublicensable, non exclusive, irrevocable and unconditional license to
  exercise Affirmer's Copyright and Related Rights in the Work (i) in
  all territories worldwide, (ii) for the maximum duration provided by
  applicable law or treaty (including future time extensions), (iii) in
  any current or future medium and for any number of copies, and (iv)
  for any purpose whatsoever, including without limitation commercial,
  advertising or promotional purposes (the "License"). The License shall
  be deemed effective as of the date CC0 was applied by Affirmer to the
  Work. Should any part of the License for any reason be judged legally
  invalid or ineffective under applicable law, such partial invalidity
  or ineffectiveness shall not invalidate the remainder of the License,
  and in such case Affirmer hereby affirms that he or she will not (i)
  exercise any of his or her remaining Copyright and Related Rights in
  the Work or (ii) assert any associated claims and causes of action
  with respect to the Work, in either case contrary to Affirmer's
  express Statement of Purpose.
  
  4. Limitations and Disclaimers.
  
  No trademark or patent rights held by Affirmer are waived, abandoned,
  surrendered, licensed or otherwise affected by this document.
  Affirmer offers the Work as-is and makes no representations or
  warranties of any kind concerning the Work, express, implied,
  statutory or otherwise, including without limitation warranties of
  title, merchantability, fitness for a particular purpose, non
  infringement, or the absence of latent or other defects, accuracy, or
  the present or absence of errors, whether or not discoverable, all to
  the greatest extent permissible under applicable law.
  Affirmer disclaims responsibility for clearing rights of other persons
  that may apply to the Work or any use thereof, including without
  limitation any person's Copyright and Related Rights in the Work.
  Further, Affirmer disclaims responsibility for obtaining any necessary
  consents, permissions or other rights required for any use of the
  Work.
  Affirmer understands and acknowledges that Creative Commons is not a
  party to this document and has no duty or obligation with respect to
  this CC0 or use of the Work.

  Contributions thanks to:
    niXman <i.nixman@autistici.org>
    Ely Arzhannikov <iarzhannikov@gmail.com>
    Alexey Pavlov <alexpux@gmail.com>
    Ray Donnelly <mingw.android@gmail.com>
    Johannes Schindelin <johannes.schindelin@gmx.de>
  
*/

#include "winsup.h"
#include "miscfuncs.h"
#include <ctype.h>
#include <winioctl.h>
#include <shlobj.h>
#include <sys/param.h>
#include <sys/cygwin.h>
#include "cygerrno.h"
#include "security.h"
#include "path.h"
#include "fhandler.h"
#include "dtable.h"
#include "cygheap.h"
#include "shared_info.h"
#include "cygtls.h"
#include "tls_pbuf.h"
#include "environ.h"
#include <assert.h>
#include <ntdll.h>
#include <wchar.h>
#include <wctype.h>

#include "msys2_path_conv.h"

typedef enum PATH_TYPE_E {
    NONE = 0,
    SIMPLE_WINDOWS_PATH,
    ESCAPE_WINDOWS_PATH,
    WINDOWS_PATH_LIST,
    UNC,
    ESCAPED_PATH,
    ROOTED_PATH,
    POSIX_PATH_LIST,
    RELATIVE_PATH,
    URL
} path_type;

int is_special_posix_path(const char* from, const char* to, char** dst, const char* dstend);
void posix_to_win32_path(const char* from, const char* to, char** dst, const char* dstend);


path_type find_path_start_and_type(const char** src, int recurse, const char* end);
void copy_to_dst(const char* from, const char* to, char** dst, const char* dstend);
void convert_path(const char** from, const char* to, path_type type, char** dst, const char* dstend);

//Transformations
//SIMPLE_WINDOWS_PATH converter. Copy as is. Hold C:\Something\like\this
void swp_convert(const char** from, const char* to, char** dst, const char* dstend);
//ESCAPE_WINDOWS_PATH converter. Turn backslashes to slashes and skip first /. Hold /C:\Somethind\like\this
void ewp_convert(const char** from, const char* to, char** dst, const char* dstend);
//WINDOWS_PATH_LIST converter. Copy as is. Hold /something/like/this;
void wpl_convert(const char** from, const char* to, char** dst, const char* dstend);
//UNC convert converter. Copy as is. Hold //somethig/like/this
void unc_convert(const char** from, const char* to, char** dst, const char* dstend);
//ESCAPED_PATH converter. Turn backslashes to slashes and skip first /. Hold //something\like\this
void ep_convert(const char** from, const char* to, char** dst, const char* dstend);
//ROOTED_PATH converter. Prepend root dir to front. Hold /something/like/this
void rp_convert(const char** from, const char* to, char** dst, const char* dstend);
//URL converter. Copy as is.
void url_convert(const char** from, const char* to, char** dst, const char* dstend);
//POSIX_PATH_LIST. Hold x::x/y:z
void ppl_convert(const char** from, const char* to, char** dst, const char* dstend);


void find_end_of_posix_list(const char** to, int* in_string) {
    for (; **to != '\0' && (in_string ? (**to != *in_string) : **to != ' '); ++*to) {
    }

    if (**to == *in_string) {
        *in_string = 0;
    }
}

void find_end_of_rooted_path(const char** from, const char** to, int* in_string) {
    for (const char* it = *from; *it != '\0' && it != *to; ++it)
        if (*it == '.' && *(it + 1) == '.' && *(it - 1) == '/') {
            *to = it - 1;
            return;
        }

    for (; **to != '\0'; ++*to) {
        if (*in_string == 0 && **to == ' ') {
            return;
        }

        if (**to == *in_string) {
            *in_string = 0;
            return;
        }

        if (**to == '/') {
            if (*(*to - 1) == ' ') {
                *to -= 1;
                return;
            }
        }
    }
}

void sub_convert(const char** from, const char** to, char** dst, const char* dstend, int* in_string) {
    const char* copy_from = *from;
    path_type type = find_path_start_and_type(from, false, *to);

    if (type == POSIX_PATH_LIST) {
        find_end_of_posix_list(to, in_string);
    }

    if (type == ROOTED_PATH) {
        find_end_of_rooted_path(from, to, in_string);
    }

    copy_to_dst(copy_from, *from, dst, dstend);

    if (type != NONE) {
        convert_path(from, *to, type, dst, dstend);
    }

    if (*dst != dstend) {
        **dst = **to;
        *dst += 1;
    }
}

const char* convert(char *dst, size_t dstlen, const char *src) {
    if (dst == NULL || dstlen == 0 || src == NULL) {
        return dst;
    }

    int need_convert = false;
    for (const char* it = src; *it != '\0'; ++it) {
        if (*it == '\\' || *it == '/') {
            need_convert = true;
            break;
        }
        if (isspace(*it)) {
            need_convert = false;
            break;
        }
    }

    char* dstit = dst;
    char* dstend = dst + dstlen;
    if (!need_convert) {
        copy_to_dst(src, NULL, &dstit, dstend);
        *dstit = '\0';
        return dst;
    }
    *dstend = '\0';

    const char* srcit = src;
    const char* srcbeg = src;

    int in_string = false;

    for (; *srcit != '\0'; ++srcit) {
        if (*srcit == '\'' || *srcit == '"') {
            if (in_string == *srcit) {
                if (*(srcit + 1) != in_string) {
                    in_string = 0;
                }
            } else {
                in_string = *srcit;
            }
            continue;
        }

        if (isspace(*srcit)) {
            //sub_convert(&srcbeg, &srcit, &dstit, dstend, &in_string);
            //srcbeg = srcit + 1;
            break;
        }
    }

    sub_convert(&srcbeg, &srcit, &dstit, dstend, &in_string);
    if (!*srcit) {
      *dstit = '\0';
      return dst;
    }
    srcbeg = srcit + 1;
    for (; *srcit != '\0'; ++srcit) {
      continue;
    }
    copy_to_dst(srcbeg, srcit, &dstit, dstend);
    *dstit = '\0';

    /*if (dstit - dst < 2) {
        dstit = dst;
        copy_to_dst(src, NULL, &dstit, dstend);
        *dstit = '\0';
    }*/

    return dst;
}

void copy_to_dst(const char* from, const char* to, char** dst, const char* dstend) {
    for (; (*from != '\0') && (from != to) && (*dst != dstend); ++from, ++(*dst)) {
        **dst = *from;
    }
}

const char** move(const char** p, int count) {
    *p += count;
    return p;
}

path_type find_path_start_and_type(const char** src, int recurse, const char* end) {
    const char* it = *src;

    if (*it == '\0' || it == end) return NONE;

    if (!isalnum(*it) && *it != '/' && *it != '\\' && *it != ':' && *it != '-' && *it != '.') {
        return find_path_start_and_type(move(src, 1), true, end);
    }

    path_type result = NONE;

    if (isalpha(*it) && *(it + 1) == ':') {
        if (*(it + 2) == '\\') {
            return SIMPLE_WINDOWS_PATH;
        }

        if (*(it + 2) == '/' && memchr(it + 2, ':', end - (it + 2)) == NULL) {
            return SIMPLE_WINDOWS_PATH;
        }

        if (*(it + 2) == '/' && memchr(it + 2, ';', end - (it + 2))) {
            return WINDOWS_PATH_LIST;
        }
    }

    if (*it == '.' && (*(it + 1) == '.' || *(it + 1) == '/') && memchr(it + 2, ':', end - (it + 2)) == NULL) {
        return RELATIVE_PATH;
    }

    if (*it == '/') {
        it += 1;

        if (isalpha(*it) && *(it + 1) == ':') {
            return ESCAPE_WINDOWS_PATH;
        }

        if (*it == '.' && *(it + 1) == '.') {
            return SIMPLE_WINDOWS_PATH;
        }

        if (*it == '/') {
            it += 1;
            switch(*it) {
              case ':': return URL;
              case '/': return ESCAPED_PATH;
            }
            if (memchr(it, '/', end - it))
              return UNC;
            else
              return ESCAPED_PATH;
        }

        for (; *it != '\0' && it != end; ++it) {
            switch(*it) {
                case ':': {char ch = *(it + 1); if (ch == '/' || ch == ':' || ch == '.') return POSIX_PATH_LIST;} return WINDOWS_PATH_LIST;
                case ';': return WINDOWS_PATH_LIST;
            }
        }

        if (result != NONE) {
            return result;
        }

        return ROOTED_PATH;
    }

    int starts_with_minus = 0;
    int starts_with_minus_alpha = 0;
    if (*it == '-') {
      starts_with_minus = 1;
      it += 1;
      if (isalpha(*it)) {
        it += 1;
        starts_with_minus_alpha = 1;
        if (memchr(it, ';', end - it)) {
        	return WINDOWS_PATH_LIST;
        }
      }
    }

    for (const char* it2 = it; *it2 != '\0' && it2 != end; ++it2) {
        char ch = *it2;
        if (starts_with_minus_alpha) {
            if (isalpha(ch) && (*(it2+1) == ':') && (*(it2+2) == '/')) {
                return SIMPLE_WINDOWS_PATH;
            }
            if (ch == '/'&& memchr(it2, ',', end - it) == NULL) {
                *src = it2;
                return find_path_start_and_type(src, true, end);
            }
            starts_with_minus_alpha = 0;
        }
        if (ch == '\'' || ch == '"')
            starts_with_minus = false;
        if ((ch == '=') || (ch == ':' && starts_with_minus) || ((ch == '\'' || ch == '"') && result == NONE)) {
            *src = it2 + 1;
            return find_path_start_and_type(src, true, end);
        }

        if (ch == ',' && starts_with_minus) {
            *src = it2 + 1;
            return find_path_start_and_type(src, true, end);
        }

        if (ch == ':' && it2 + 1 != end) {
            it2 += 1;
            ch = *it2;
            if (ch == '/' || ch == ':' || ch == '.') {
                if (ch == '/' && *(it2 + 1) == '/') {
                    return URL;
                } else {
                    return POSIX_PATH_LIST;
                }
            } else if (memchr(it2, '=', end - it) == NULL) {
                return SIMPLE_WINDOWS_PATH;
            }
        }
    }

    if (result != NONE) {
        *src = it;
        return result;
    }

    return SIMPLE_WINDOWS_PATH;
}

void convert_path(const char** from, const char* to, path_type type, char** dst, const char* dstend) {
    switch(type) {
        case SIMPLE_WINDOWS_PATH: swp_convert(from, to, dst, dstend); break;
        case ESCAPE_WINDOWS_PATH: ewp_convert(from, to, dst, dstend); break;
        case WINDOWS_PATH_LIST: wpl_convert(from, to, dst, dstend); break;
        case RELATIVE_PATH: swp_convert(from, to, dst, dstend); break;
        case UNC: unc_convert(from, to, dst, dstend); break;
        case ESCAPED_PATH: ep_convert(from, to, dst, dstend); break;
        case ROOTED_PATH: rp_convert(from, to, dst, dstend); break;
        case URL: url_convert(from, to, dst, dstend); break;
        case POSIX_PATH_LIST: ppl_convert(from, to, dst, dstend); break;
        case NONE: // prevent warnings;
        default:
                return;
    }
}

void swp_convert(const char** from, const char* to, char** dst, const char* dstend) {
    copy_to_dst(*from, to, dst, dstend);
}

void ewp_convert(const char** from, const char* to, char** dst, const char* dstend) {
    *from += 1;
    unc_convert(from, to, dst, dstend);
}

void wpl_convert(const char** from, const char* to, char** dst, const char* dstend) {
    swp_convert(from, to, dst, dstend);
}

void unc_convert(const char** from, const char* to, char** dst, const char* dstend) {
    const char* it = *from;
    for (; (*it != '\0' && it != to) && (*dst != dstend); ++it, ++(*dst)) {
        if (*it == '\\') {
            **dst = '/';
        } else {
            **dst = *it;
        }
    }
}

void ep_convert(const char** from, const char* to, char** dst, const char* dstend) {
    ewp_convert(from, to, dst, dstend);
}

void rp_convert(const char** from, const char* to, char** dst, const char* dstend) {
    const char* it = *from;
    const char* real_to = to;

    if (*real_to == '\0')  {
        real_to -= 1;
        if (*real_to != '\'' && *real_to != '"') {
            real_to += 1;
        }
    }

    if (!is_special_posix_path(*from, real_to, dst, dstend)) {
        posix_to_win32_path(it, real_to, dst, dstend);
    }

    if (*dst != dstend && real_to != to) {
        **dst = *real_to;
        *dst += 1;
    }
}

void url_convert(const char** from, const char* to, char** dst, const char* dstend) {
    unc_convert(from, to, dst, dstend);
}

void subp_convert(const char** from, const char* end, int is_url, char** dst, const char* dstend) {
    const char* begin = *from;
    path_type type = is_url ? URL : find_path_start_and_type(from, 0, end);
    copy_to_dst(begin, *from, dst, dstend);

    if (type == NONE) {
        return;
    }

    char* start = *dst;
    convert_path(from, end, type, dst, dstend);

    if (!is_url) {
        for (; start != *dst; ++start) {
            if (*start == '/') {
                *start = '\\';
            }
        }
    }
}

void ppl_convert(const char** from, const char* to, char** dst, const char* dstend) {
    const char *orig_dst = *dst;
    const char* it = *from;
    const char* beg = it;
    int prev_was_simc = 0;
    int is_url = 0;
    for (; (*it != '\0' && it != to) && (*dst != dstend); ++it) {
        if (*it == ':') {
            if (prev_was_simc) {
                continue;
            }
            if (*(it + 1) == '/' && *(it + 2) == '/' && isalpha(*beg)) {
                is_url = 1;
                /* double-check: protocol must be alnum (or +) */
                for (const char *p = beg; p != it; ++p)
                    if (!isalnum(*p) && *p != '+') {
                        is_url = 0;
                        break;
                    }
                if (is_url)
                    continue;
            }
            prev_was_simc = 1;
            subp_convert(&beg, it, is_url, dst, dstend);
            is_url = 0;

            if (*dst == dstend) {
                system_printf("Path cut off during conversion: %s\n", orig_dst);
                break;
            }

            **dst = ';';
            *dst += 1;
        }

        if (*it != ':' && prev_was_simc) {
            prev_was_simc = 0;
            beg = it;
        }
    }

    if (!prev_was_simc) {
        subp_convert(&beg, it, is_url, dst, dstend);
    } else {
        *dst -= 1;
    }
}

int is_special_posix_path(const char* from, const char* to, char** dst, const char* dstend) {
    const char dev_null[] = "/dev/null";

    if ((to - from) == (sizeof(dev_null) - 1) && strncmp(from, "/dev/null", to - from) == 0) {
        copy_to_dst("nul", NULL, dst, dstend);
        return true;
    }
    return false;
}

void posix_to_win32_path(const char* from, const char* to, char** dst, const char* dstend) {
    if ( from != to ) {
        tmp_pathbuf tp;
        char *one_path = tp.c_get();
        strncpy(one_path, from, to-from);
        one_path[to-from] = '\0';

        path_conv conv (one_path, 0);
        if (conv.error)
        {
          set_errno(conv.error);
          copy_to_dst(one_path, NULL, dst, dstend);
        } else {
          char* win32_path = tp.c_get();
          stpcpy (win32_path, conv.get_win32 ());
          for (; (*win32_path != '\0') && (*dst != dstend); ++win32_path, ++(*dst)) {
             **dst = (*win32_path == '\\') ? '/' : *win32_path;
          }
        }
    }
}

