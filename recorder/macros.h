/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * macros.h
 *
 * Started: Sunday 23 December 2012, 19:43:14
 * Last Modified: Monday 28 December 2015, 08:19:07
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

extern int llevel;

#define CCA_ERR_CONT(...) syslog(LOG_ERR,__VA_ARGS__)
#define CCA_ERR_EXIT(ccaex,...) if(errno){ CCA_ERR_CONT("error: %d: %s",errno,strerror(errno));} CCA_ERR_CONT(__VA_ARGS__);exit(ccaex);
#define CCAC(...) CCA_ERR_CONT(__VA_ARGS__)
#define CCAE(ccaex,...) CCA_ERR_EXIT(ccaex,__VA_ARGS__)
#define DEBUG(...) if(llevel>=7){ syslog(LOG_DEBUG,__VA_ARGS__);}
#define INFO(...) if(llevel>=6){ syslog(LOG_INFO,__VA_ARGS__);}
#define NOTICE(...) if(llevel>=5){ syslog(LOG_NOTICE,__VA_ARGS__);}
#define WARN(...) if(llevel>=4){ syslog(LOG_WARNING,__VA_ARGS__);}
#define ERROR(...) if(llevel>=3){ syslog(LOG_ERR,__VA_ARGS__);}

#define XDEBUG

#ifndef XDEBUG
#define WHERESTR  "[file %s, line %d]: "
#define WHEREARG  __FILE__, __LINE__
#define DEBUGPRINT2(...)       fprintf(stderr, __VA_ARGS__);syslog(LOG_DEBUG,__VA_ARGS__)
#define DEBUGPRINT3(...)       syslog(LOG_DEBUG,__VA_ARGS__)
#define DEBUGPRINT(_fmt, ...)  DEBUGPRINT3(WHERESTR _fmt, WHEREARG, __VA_ARGS__)
/* #define DBG(_fmt, ...) DEBUGPRINT(_fmt, __VA_ARGS__) */
#define DBGL(_fmt, ...) if(llevel>=7){ DEBUGPRINT2(WHERESTR _fmt"\n", WHEREARG, __VA_ARGS__); }
#define DBG(...) if(llevel>=7){ DEBUGPRINT2(WHERESTR" %s\n",WHEREARG,__VA_ARGS__); }
#else
#define DEBUGPRINT(_fmt, ...) {}
#define DBG(_fmt, ...) {}
#define DBGL(_fmt,...) {}
#endif /* !XDEBUG */
