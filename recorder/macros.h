/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * macros.h
 *
 * Started: Sunday 23 December 2012, 19:43:14
 * Last Modified: Sunday 27 July 2014, 15:25:50
 *
 * Copyright (c) 2014 Chris Allison chris.allison@hotmail.com
 *
 * This file is part of cristel.
 * 
 * cristel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * cristel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with cristel.  If not, see <http://www.gnu.org/licenses/>.
 */
/* #define PRODUCTION */
#ifndef PRODUCTION
#define CCA_ERR_CONT(...) syslog(LOG_ERR,__VA_ARGS__);fprintf(stderr, __VA_ARGS__);
#define CCA_ERR_EXIT(ccaex,...) CCA_ERR_CONT(__VA_ARGS__);exit(ccaex);
#define CCAC(...) CCA_ERR_CONT(__VA_ARGS__)
#define CCAE(ccaex,...) CCA_ERR_EXIT(ccaex,__VA_ARGS__)
#define CCAL(...) syslog(LOG_INFO,__VA_ARGS__)
#else
#define CCA_ERR_CONT(...) syslog(LOG_ERR,__VA_ARGS__)
#define CCA_ERR_EXIT(ccaex,...) CCA_ERR_CONT(__VA_ARGS__);exit(ccaex);
#define CCAC(...) CCA_ERR_CONT(__VA_ARGS__)
#define CCAE(ccaex,...) CCA_ERR_EXIT(ccaex,__VA_ARGS__)
#define CCAL(...) syslog(LOG_INFO,__VA_ARGS__)
#endif

/* define NDEBUG to remove debug msgs */
#ifdef PRODUCTION
#define NDEBUG
#endif

#ifndef NDEBUG
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__);syslog(LOG_DEBUG,__VA_ARGS__)
#define DEBUGPRINT3(...)       syslog(LOG_DEBUG,__VA_ARGS__)
#define DEBUGPRINT(_fmt, ...)  DEBUGPRINT3(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
/* #define DBG(_fmt, ...) DEBUGPRINT(_fmt, __VA_ARGS__) */
#define DBGL(_fmt, ...) DEBUGPRINT2(WHERESTR _fmt"\n", WHEREARG, __VA_ARGS__)
#define DBG(...) DEBUGPRINT2(WHERESTR" %s\n",WHEREARG,__VA_ARGS__)
#else
#define CCADBG(ccastr) {} 
#define DEBUGPRINT(_fmt, ...) {}
#define DBG(_fmt, ...) {}
#define DBGL(_fmt,...) {}
#endif /* !NDEBUG */
