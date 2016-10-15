/*
 * vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 foldmethod=marker:
 *
 * configfile.h
 *
 * Started: Monday 24 December 2012, 13:11:56
 * Last Modified: Saturday 15 October 2016, 11:06:25
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
#ifndef CCA_CONFIGFILE_H
#define CCA_CONFIGFILE_H

#include "defs.h"

struct ConfigItem{
    char *key;
    char *value;
    struct ConfigItem *next;
};

int initConfig( void );
void getConfigFromFile(char *filename);
struct ConfigItem *addConfig(char *key, char *value);
struct ConfigItem *updateConfig(char *key, char *value);
int deleteConfigItem(char *key);
void unlinkConfig(struct ConfigItem *prev, struct ConfigItem *dc);
void deleteConfig( void );
struct ConfigItem *searchConfigP(char *key);
char *configValueDup(char *key);
char *configValue(char *key);
void printConfig( void );

#endif
